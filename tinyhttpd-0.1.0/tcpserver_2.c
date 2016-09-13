#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<netdb.h>


void main()
{
	struct sockaddr_in serveraddr;
	int server_socket_fd;
	int client_socket_fd;
	int port = 7777;

	memset(&serveraddr,0,sizeof(serveraddr));
	serveraddr.sin_port = htons(port);
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

	server_socket_fd = socket(serveraddr.sin_family,SOCK_STREAM,0);
	bind(server_socket_fd,(struct sockaddr *)&serveraddr,sizeof(serveraddr));

	listen(server_socket_fd,1);

	client_socket_fd = accept(server_socket_fd,NULL,NULL);
	int myBUF_SIZE = 1024;
	char buf[myBUF_SIZE];
	int readlen = 0;
	while((readlen=recv(client_socket_fd,buf,myBUF_SIZE,0))>0)
	{
		if(readlen >myBUF_SIZE-1)
			readlen = myBUF_SIZE -1;
		buf[readlen] = '\0';
		printf("%s\n",buf);
	}
	close(client_socket_fd);
	close(server_socket_fd);
	return;
}
