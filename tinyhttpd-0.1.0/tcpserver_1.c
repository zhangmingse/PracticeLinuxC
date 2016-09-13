#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<string.h>
#include<netdb.h>
void main()
{
	struct sockaddr_in saddr;
	int addr_len;
	int port = 7777;
	int server_sock_id ;
	int client_sock_id;

	memset(&saddr,0,sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);

	addr_len = sizeof(saddr);

	server_sock_id = socket(saddr.sin_family,SOCK_STREAM,0);
	if(server_sock_id<0)
	{
		perror("socket error");
	}

	bind(server_sock_id,(struct sockaddr *)&saddr,addr_len);
	listen(server_sock_id,1);

	client_sock_id = accept(server_sock_id,NULL,NULL);

	int myBUF_SIZE = 1024;
	char buf[myBUF_SIZE];
	while(recv(client_sock_id,buf,myBUF_SIZE,0)>0)
	{
		printf("%s\n",buf);
	}
	close(client_sock_id);
	close(server_sock_id);
	return;

}
