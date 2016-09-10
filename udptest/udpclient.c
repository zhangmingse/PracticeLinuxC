#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<netdb.h>
#include<stdarg.h>
#include<string.h>



#define SERVER_PORT 4444
#define	BUFF_SIZE 1024
#define FILE_NAME_MAX_SIZE 512

int main(int ac ,char * av[])
{
	if(ac != 2)
	{
		perror("argument number error. usage: udpclient hostIP");
		return -1;
	}
	struct sockaddr_in server_addr;
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(av[1]);
	server_addr.sin_port = htons(SERVER_PORT);
	
	int client_socket_fd = socket(AF_INET,SOCK_DGRAM,0);
	if(client_socket_fd < 0)
	{
		perror("create socket error");
		exit(-1);
	}

	char file_name[FILE_NAME_MAX_SIZE+1];
	bzero(file_name,FILE_NAME_MAX_SIZE+1);
	printf("please input file name on server:\t");
	scanf("%s",file_name);

	char buffer[BUFF_SIZE];
	bzero(buffer,BUFF_SIZE);
	strncpy(buffer,file_name,strlen(file_name)>BUFF_SIZE?BUFF_SIZE:strlen(file_name));

	printf("filename %s\n",buffer);
	if(sendto(client_socket_fd,buffer,BUFF_SIZE,0,(struct sockaddr *)&server_addr,sizeof(server_addr))<0)
	{
		perror("send file name failed");
		exit(-1);
	}
	printf("done!\n");
	close(client_socket_fd);
	return 0;

}
