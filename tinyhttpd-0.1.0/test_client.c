#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<string.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>


void * client_do(void * arg)
{

	printf("thread %lu start working \n",pthread_self());
	struct sockaddr_in* saddr = (struct sockaddr_in*)arg;
	int sock_fd;
	sock_fd = socket(saddr->sin_family,SOCK_STREAM,0);
	int result_val = 0;
	result_val	= connect(sock_fd,(struct sockaddr *)saddr,sizeof(struct sockaddr_in));
	if(result_val !=0)
	{
		perror("connect error");
		printf("connect result : %d\n",result_val);
		return NULL;
	}
	char request[] = "GET / HTTP/1.1\r\nsomeheader=headervaluer\r\n\r\n\0";
	int request_len =0;
	request_len = strlen(request);
	int bufsize = 1024;
	char buf[bufsize];
	int send_len = 0;
	send_len =  send(sock_fd,request,request_len,0);

	/*********************************************/
//	close(sock_fd);
//	return NULL;
	/*********************************************/

	if(send_len != request_len)
	{
		printf("send_len != request_len,s:%d  r:%d",send_len,request_len);
	}
	int read_len =0;
	read_len = recv(sock_fd,buf,bufsize,0);
	while(read_len>0)
	{
		if(read_len >= bufsize)
		{
			read_len = bufsize-1;
		}
		buf[read_len]= '\0';
		printf("%s",buf);
		read_len=recv(sock_fd,buf,bufsize,0);
	}
	close(sock_fd);
	printf("done %lu\n",pthread_self());

}


struct sockaddr_in * get_server_sockaddr_in(char * server_ip,char * port)
{
	struct sockaddr_in * saddr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
	memset(saddr,0,sizeof(*saddr));
	saddr->sin_family = AF_INET;
	saddr->sin_port =htons( atoi(port));
	saddr->sin_addr.s_addr = inet_addr(server_ip);

	return saddr;
}

void main(int ac ,char * args[])
{
	if(ac !=4)
	{
		printf("useage: ./testclient serverIP serverPORT clientCount\n");
		return;
	}

	struct sockaddr_in * saddr = get_server_sockaddr_in(args[1],args[2]);

	int client_count = 0;
	client_count = atoi(args[3]);
	int max_client_count = 30000;
	if(client_count <=0 || client_count > max_client_count)
	{
		printf("client number is out of limit.it should between 1 and %d\n",max_client_count);
	}

	int index = 0 ;
	pthread_t thread_id[max_client_count];

	for(;index<client_count;index++)
	{
		pthread_create(thread_id+index,NULL,client_do,(void *)saddr);
	}

	index = 0;
	for(;index < client_count;index++)
	{	
		pthread_join(thread_id[index],NULL);
	}

	free(saddr);
	printf("done\n");
}
