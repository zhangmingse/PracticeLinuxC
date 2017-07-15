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
#include <sys/epoll.h>
#include <fcntl.h>
#include <assert.h>

#define SERVER_STRING "Server : zmhttpd/0.1.0\r\n"
#define IS_SPACE(x) isspace((int)x)

#define MAXCONN 200
#define BUFSIZE 512
#define MAX_EVENTS MAXCONN

struct sockfd_opt
{
	int fd;
	int (* do_task)(struct sockfd_opt *p_so);
};

typedef struct sockfd_opt SOCKOPT;

int epfd;
int num;			//current total num of connections
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
	int buf_size = 30;
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
	if(file == NULL)
	{
		printf("file not exist\n");
		file_not_found(sock_fd);
		return;
	}
	int buf_size = 1024;
	char buf[buf_size];
	int read_len = 0;

	read_len = get_line(sock_fd,buf,buf_size);
	while(read_len > 0 && buf[0] != '\n')
	{
		read_len = get_line(sock_fd,buf,buf_size);
	}


	header(sock_fd);
	fgets(buf,buf_size,file);
	while(!feof(file))
	{
		printf("==>serve_file :  %s\n",buf);
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
	struct sockfd_opt * ptr_s_opt = ((struct sockfd_opt *)arg);
	int client_sock_fd = ptr_s_opt->fd;
//	free(arg);
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
		close(client_sock_fd);
		free(ptr_s_opt);
		return NULL;
	}
	printf("----------------------------------------------------\n");
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
	free(ptr_s_opt);
	printf("close client sock\n");
	return NULL;//to do
}


void set_fd_nonblocking(int sock_fd)
{
	int opts;

	opts = fcntl(sock_fd,F_GETFL);
	if(opts < 0)
		err_die("fcntl get options\n");
	opts = opts | O_NONBLOCK;
	if(fcntl(sock_fd,F_SETFL,opts))
		err_die("fcntl set options\n");
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

	set_fd_nonblocking(server_sock);	
	
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
	printf("accept sigpipe. %d \n",arg);
}

int init_epoll_fd()
{
	
	epfd = epoll_create(MAXCONN);
	if(epfd<0)
	{
		if(epfd==-1)
		{
			printf("epoll_create error.errno:%d\n",errno);
		}
		else
		{
			printf("epoll_create error. return value is not -1\n");
		}
		exit(1);
	}
	return epfd;
}



int create_conn(struct sockfd_opt * p_so)
{
	int new_client_fd;
	struct sockaddr_in client_addr;
	socklen_t sin_size;
	sin_size = sizeof(struct sockaddr_in);
	new_client_fd = accept(p_so->fd,(struct sockaddr*)(&client_addr),&sin_size);

	if(new_client_fd == -1)
	{
		err_die("accept error\n");
		exit(1);
	}
	
	set_fd_nonblocking(new_client_fd);	

	p_so = malloc(sizeof(struct sockfd_opt));
	if(p_so == NULL )
	{
		err_die("malloc sockfd_opt\n");
	}
	p_so->fd = new_client_fd;
	p_so->do_task = parse_request;

	num++;
	
	struct epoll_event ep_event;

	ep_event.data.ptr = p_so;
	ep_event.events = EPOLLIN;
	int ret_val;
	ret_val = epoll_ctl(epfd,EPOLL_CTL_ADD,new_client_fd,&ep_event);
	if(ret_val )
	{
		err_die("epoll_ctl add error\n");
		return -1;
	}
	return 0;

}

int init(int fd)
{
	struct sockfd_opt *p_so;
	p_so = malloc(sizeof(struct sockfd_opt));
	if(p_so == NULL)
	{
		err_die("malloc sockfd_opt\n");
	}
	p_so->fd = fd;
	p_so->do_task = create_conn;
	

	struct epoll_event epollevent;
	epollevent.data.ptr = p_so;
	epollevent.events = EPOLLIN;
	
	int ret_val = epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&epollevent);
	if(ret_val)
		err_die("epoll_ctl\n");
	num++;

	return 0;

}
void main()
{
	int port = 80;
	int server_sock_fd = 0;
	int * client_sock_fd_p = NULL;
	int client_sock_fd = 0;
	struct epoll_event * events;
	int events_num;
	unsigned int hash;
	struct sockfd_opt *p_so;
	signal(SIGPIPE,handle_sigpipe);
	
	server_sock_fd = init_server_sock(&port);
	init_epoll_fd();	
	init(server_sock_fd);
	printf("running on port : %d\n",port);
/*	pthread_t newThread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);*/
	
	events = malloc(sizeof(struct epoll_event) * MAXCONN);
	if(!events)
	{
		err_die("malloc epoll events\n");
	}
	while(1)
	{

		events_num = epoll_wait(epfd,events,MAX_EVENTS,-1);
		if(events_num < 0)
		{
			free(events);
			err_die("epoll_wait\n");
		}
		int i = 0;
		for(i =0;i<events_num;i++)
		{
			p_so = (struct sockfd_opt *)(events[i].data.ptr);

			p_so->do_task(p_so);
		}



/*		client_sock_fd = accept(server_sock_fd,NULL,NULL);
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
		parse_request(&client_sock_fd);*/
	}
	return 0;
}
