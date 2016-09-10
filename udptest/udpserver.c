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
#define	BUFFER_SIZE 1024
#define FILE_NAME_MAX_SIZE 512

int main()
{
	struct sockaddr_in server_addr;
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(SERVER_PORT);

	int server_socket_fd = socket(AF_INET,SOCK_DGRAM,0);
	if(server_socket_fd == -1)
	{
		perror("Create Socket Failed:");
		exit(1);
	}

	if(-1== (bind(server_socket_fd,(struct sockaddr*)&server_addr,sizeof(server_addr))))
	{
		perror("Server Bind Failed:");
		exit(1);
	}

	while(1)
	{
		struct sockaddr_in client_addr;
		socklen_t client_addr_length = sizeof(client_addr);

		char buf[BUFFER_SIZE];
		bzero(buf,BUFFER_SIZE);
		if(recvfrom(server_socket_fd,buf,BUFFER_SIZE,0,(struct sockaddr *)&client_addr,&client_addr_length) == -1)
		{
			perror("Receive Data Failed:");
			exit(1);
		}

		char file_name[FILE_NAME_MAX_SIZE+1];
		bzero(file_name,FILE_NAME_MAX_SIZE+1);
		strncpy(file_name,buf,strlen(buf)>FILE_NAME_MAX_SIZE?FILE_NAME_MAX_SIZE:strlen(buf));
		printf("%s\n",file_name);
	}
	close(server_socket_fd);
	return 0;

}
