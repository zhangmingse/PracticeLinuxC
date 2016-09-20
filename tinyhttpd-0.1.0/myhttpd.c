#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

#define SERVER_STRING "Server : zmhttpd/0.1.0\r\n"
#define IS_SPACE(x) isspace((int)x)
void err_die(char * info)
{
	perror(info);
	exit(-1);
}

void file_not_found(int client_fd)
{
	int buf_size = 1024;
	char buf[buf_size] ;
	sprintf(buf,"HTTP/1.0 404 not found\r\n");
	send(client_fd,buf,strlen(buf),0);

	sprintf(buf,SERVER_STRING);
	send(client_fd,buf,strlen(buf),0);
	sprintf(buf,"Content-Type: text/html\r\n");
	send(client_fd,buf,strlen(buf),0);
	sprintf(buf,"\r\n");
	send(client_fd,buf,strlen(buf),0);
	sprintf(buf,"<html><title>file not found</title>\r\n");
	send(client_fd,buf,strlen(buf),0);
	sprintf(buf,"<body><p>the server could not fulfill\r\n");
	send(client_fd,buf,strlen(buf),0);
	sprintf(buf,"your request because the resource specified\r\n");
	send(client_fd,buf,strlen(buf),0);
	sprintf(buf,"is unavailable or nonexistent.\r\n");
	send(client_fd,buf,strlen(buf),0);
	sprintf(buf,"</p></body></html>\r\n");
	send(client_fd,buf,strlen(buf),0);
	
}

int get_line(int sock_fd,char * buf,int buf_size)
{
	char c = '\0';
	int count = 0;
	while(count < buf_size-1 && c != '\n')
	{
		recv(sock_fd,&c,1,0);

		if(c == '\r')
		{
			recv(sock_fd,&c,1,MSG_PEEK);
			if(c == '\n')
			{
				recv(sock_fd,&c,1,0);
			}
			else
			{
				c = '\n';
			}
		}


		buf[count++] = c;
	}
	buf[count] = '\0';
	return count + 1;
}

void unimplemented(int sock_fd)
{
	int buf_size = 1024;
	char buf[buf_size];
	sprintf(buf,"HTTP/1.0 501 Method Not Implemented\r\n");
	send(sock_fd,buf,strlen(buf),0);
	sprintf(buf,SERVER_STRING);
	send(sock_fd,buf,strlen(buf),0);
	sprintf(buf,"Content-Type: text/html\r\n");
	send(sock_fd,buf,strlen(buf),0);
	sprintf(buf,"\r\n");
	send(sock_fd,buf,strlen(buf),0);
	sprintf(buf,"<html><head><title>Method Not Implemented</title></head>\r\n");
	send(sock_fd,buf,strlen(buf),0);
	sprintf(buf,"<body><p>HTTP Request method not supported.</p></body>\r\n");
	send(sock_fd,buf,strlen(buf),0);
	sprintf(buf,"</html>\r\n");
	send(sock_fd,buf,strlen(buf),0);

}

void header(int sock_fd)
{
	int buf_size = 1024;
	char buf[buf_size];
	sprintf(buf,"HTTP/1.0 200 OK\r\n");
	send(sock_fd,buf,strlen(buf),0);
	sprintf(buf,SERVER_STRING);
	send(sock_fd,buf,strlen(buf),0);
	sprintf(buf,"Content-Type: text/html\r\n");
	send(sock_fd,buf,strlen(buf),0);
	sprintf(buf,"Connection: close\r\n");
	send(sock_fd,buf,strlen(buf),0);
	sprintf(buf,"\r\n");
	send(sock_fd,buf,strlen(buf),0);
	return;
}

void serve_file(int sock_fd,char * path)
{
	FILE * file ;
	printf("file path = %s\n",path);
	file = fopen(path,"r");
	int buf_size = 1024;
	char buf[buf_size];
	int read_len = 0;

	read_len = get_line(sock_fd,buf,buf_size);
	while(read_len > 0 && buf[0] != '\n')
	{
		read_len = get_line(sock_fd,buf,buf_size);
	}

	if(file == NULL)
	{
		printf("file not exist\n");
		file_not_found(sock_fd);
		return;
	}

	header(sock_fd);
	fgets(buf,buf_size,file);
	while(!feof(file))
	{
//		printf("%s\n",buf);
		send(sock_fd,buf,strlen(buf),0);
		fgets(buf,buf_size,file);
	}
	fclose(file);
	return;
}

void bad_request(int sock_fd)
{
	int buf_size = 1024;
	char buf[buf_size];

	sprintf(buf,"HTTP/1.0 400 BAD REQUEST\r\n");
	send(sock_fd,buf,strlen(buf),0);
	sprintf(buf,SERVER_STRING);
	send(sock_fd,buf,strlen(buf),0);
	sprintf(buf,"Content-type: text/html\r\n");
	send(sock_fd,buf,strlen(buf),0);
	sprintf(buf,"\r\n");
	send(sock_fd,buf,strlen(buf),0);
	sprintf(buf,"<p>your broswer sent a bad request,");
	send(sock_fd,buf,strlen(buf),0);
	sprintf(buf,"such as a POST without a Content-Length.\r\n</p>");
	send(sock_fd,buf,strlen(buf),0);
}

void execute_cgi(int sock_fd,char * path,char* method,char * query_string)
{
	int content_length = -1;
	int buf_size = 1024;
	char buf[buf_size];

	int pipe_to_child[2];
	int pipe_to_parents[2];

	if(strcasecmp(method,"POST") == 0)
	{//POST
		get_line(sock_fd,buf,buf_size);
		while(strcasecmp(buf,"\n")!=0)
		{
			buf[15]= '\0';
			if(strcasecmp(buf,"Content-Length:") == 0)
			{
				content_length = atoi(&(buf[16]));
//				printf("content-length = %d\n",content_length);
			}
			get_line(sock_fd,buf,buf_size);
		}
		if(content_length == -1)
		{
			bad_request(sock_fd);
			return;
		}
	}
	else
	{//GET
		buf[0] = '\0';
		int read_len =0;
		read_len = get_line(sock_fd,buf,buf_size);
		while(buf[0]!='\n' && read_len > 0)
		{
			read_len = get_line(sock_fd,buf,buf_size);
		}
	}
	
	sprintf(buf,"HTTP/1.0 200 OK\r\n");
	send(sock_fd,buf,strlen(buf),0);

	pipe(pipe_to_child);
	pipe(pipe_to_parents);

	int pid = 0;
	pid = fork();
	if(pid == 0)
	{//in child process
	//	printf("--------------------in child process\n");
		int env_size = 255;
		char method_env[env_size];
		char query_env[env_size];
		char length_env[env_size];


		sprintf(method_env,"REQUEST_METHOD=%s",method);
		putenv(method_env);

	//	printf("-----------------------in child process\n");
		if(strcasecmp(method,"GET") == 0)
		{//GET
			sprintf(query_env,"QUERY_STRING=%s",query_string);
			putenv(query_env);
			printf("----------query_string:%s\n",query_env);
		}
		else
		{//POST
			sprintf(length_env,"CONTENT_LENGTH=%d",content_length);
			putenv(length_env);
	//		printf("----------environment post content_length :%s\n" , length_env);
		}
		close(pipe_to_parents[0]);
		close(pipe_to_child[1]);
		dup2(pipe_to_parents[1],1);
		dup2(pipe_to_child[0],0);
		

		execl(path,path,NULL);
		exit(0);
	}
	else
	{//in parents process
		close(pipe_to_child[0]);
		close(pipe_to_parents[1]);
		char c = '\0';
		if(strcasecmp(method,"POST") == 0)
		{
			int i = 0;
			for(i=0;i<content_length;i++)
			{
				recv(sock_fd,&c,1,0);
				write(pipe_to_child[1],&c,1);
	//			printf("in parents process %c\n",c);
			}
		}
		while(read(pipe_to_parents[0],&c,1)>0)
		{
			send(sock_fd,&c,1,0);
		}
	}
	close(sock_fd);
	close(pipe_to_parents[0]);
	close(pipe_to_child[1]);
	int status;
	waitpid(pid,&status,0);
	return;
}

void * parse_request(void * arg)
{
	int buf_size = 1024;
	char buf[buf_size];
	int client_sock_fd = *((int *)arg);
	free(arg);
	int cgi = 0;
	int method_size = 100;
	char buf_method[method_size];
	int url_size = 500;
	char buf_url[url_size];
//	int query_size = 500;
	char *  buf_query;
	int temp_i = 0;
	int position_in_line = 0;
	char temp_c = '\0';
	int path_size = 500;
	char buf_path[path_size];
	struct stat dest_file_stat;
	get_line(client_sock_fd,buf,buf_size);
	
	while(!IS_SPACE(buf[position_in_line]) && temp_i < (method_size - 1))
	{
		buf_method[temp_i++] = buf[position_in_line++];
	}

	buf_method[temp_i] = '\0';

	//neither GET nor POST
	if(strcasecmp(buf_method,"GET")&&strcasecmp(buf_method,"POST"))
	{
		unimplemented(client_sock_fd);
		return NULL;
	}

	if(strcasecmp(buf_method,"POST") == 0)
	{
		cgi = 1;
		printf("cgi = 1 cause of POST\n");
	}

	while(IS_SPACE(buf[position_in_line]) && position_in_line < buf_size)
	{
		position_in_line++;
	}

	temp_i = 0;
	while(!IS_SPACE(buf[position_in_line]) 
			&& temp_i < (url_size-1) 
			&& position_in_line < buf_size)
	{
		buf_url[temp_i++] = buf[position_in_line++];
	}
	buf_url[temp_i] = '\0';
	
	temp_i = 0;
	char c = '\0';
	while(temp_i < strlen(buf_url)  && c != '?')
	{
		c = buf_url[temp_i++];
	}
	if(c == '?')
	{
		buf_url[temp_i-1] = '\0';
		int temp_j = 0;
//		while(temp_j < query_size-1 && temp_i < strlen(buf_url))
//		{
//			buf_query[temp_j++] = buf_url[temp_i++];
//		}
		buf_query = &(buf_url[temp_i]);
		printf("===========%s\n",buf_query);
	}
	if(buf_url[strlen(buf_url)-1] == '/')
	{
		strcat(buf_url,"index.html");
//		printf("%s\n",buf_url);
	}
	sprintf(buf_path,"htdocs%s",buf_url);
	printf("url = %s\n",buf_path);
	if(stat(buf_path,&dest_file_stat) == -1)
	{
		file_not_found(client_sock_fd);	
	}
	else
	{
		if((dest_file_stat.st_mode & S_IFMT) == S_IFDIR)
		{
			strcat(buf_path,"/index.html");
		}
		if((dest_file_stat.st_mode & S_IXUSR) ||
			(dest_file_stat.st_mode & S_IXGRP) ||
			(dest_file_stat.st_mode & S_IXOTH))
		{
			printf("cgi = 1 cause of file stat\n");
			cgi = 1;
		}

		if(!cgi)
		{
			serve_file(client_sock_fd,buf_path);
		}
		else
		{
			execute_cgi(client_sock_fd,buf_path,buf_method,buf_query);
		}

	}

	close(client_sock_fd);
	printf("close client sock\n");
	return NULL;//to do
}


int init_server_sock(int * port_p)
{
	int server_sock = 0;
	int fun_return = 0;
	struct sockaddr_in server_addr ;
	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(*port_p);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	server_sock = socket(server_addr.sin_family,SOCK_STREAM,0);

	if(server_sock <= 0)
	{
		err_die("create server socket error.");
	}
	fun_return = bind(server_sock , 
			(struct sockaddr *)&server_addr,
			sizeof(server_addr));
	if(fun_return != 0)
	{
		err_die("bind server socket error.");
	}

	if(*port_p == 0)
	{
		int addr_len = 0;
		addr_len = sizeof(server_addr);
		fun_return =  getsockname(server_sock, 
				(struct sockaddr *)&server_addr,
				&addr_len);
		if(fun_return != 0)
		{
			err_die("get server socket port error.");
		}
		*port_p = ntohs(server_addr.sin_port);
	}

	fun_return = listen(server_sock,5);
	if(fun_return != 0)
	{
		err_die("server socket listen error.");
	}

	return server_sock;
}

void handle_sigpipe(int arg)
{
	printf("accept sigpipe.\n");
}

void main()
{
	int port = 7777;
	int server_sock_fd = 0;
	int * client_sock_fd_p = NULL;
	int client_sock_fd = 0;
	server_sock_fd = init_server_sock(&port);

	printf("running on port : %d\n",port);
	pthread_t newThread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	signal(SIGPIPE,handle_sigpipe);
	while(1)
	{
		client_sock_fd = accept(server_sock_fd,NULL,NULL);
		if(client_sock_fd < 0)
		{
			err_die("server socket accept error.");
		}
		client_sock_fd_p = (int *)malloc(sizeof(int));
		if(client_sock_fd_p == NULL)
		{
			err_die("malloc error in main thread.");
		}
		*client_sock_fd_p = client_sock_fd;
		pthread_create(&newThread,&attr,parse_request,(void *)client_sock_fd_p);
	}
}
