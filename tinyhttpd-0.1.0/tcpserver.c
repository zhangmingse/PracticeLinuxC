#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<string.h>
#include<netdb.h>
#include<sys/types.h>
#include<unistd.h>

void main()
{
	struct sockaddr_in addr;
	struct sockaddr_in clientaddr;
	int socketid;
	int port = 7777;
	socketid = socket(AF_INET,SOCK_STREAM,0);
	memset(&addr,0,sizeof(addr));
	memset(&clientaddr,0,sizeof(clientaddr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(socketid,(struct sockaddr *)&addr,sizeof(addr))<0)
	{
		perror("bind error ");
		exit(-1);
	}
	listen(socketid,1);
	int clientid = accept(socketid,NULL,NULL);
	if(clientid <=0)
	{
		printf("accept error clientid = %d",clientid);
	}
	char response[] = "response !";
	char buf[1024];
	printf("accepted!\n");
	while(1)
	{

		int n = recv(clientid,buf,1024,0);
		if(n<=0)
		{
			perror("read error");
			break;
		}
		if(n<1023)
			buf[n] = '\0';
		else
			buf[1023] = '\0';
		printf("receive dada:%s\n",buf);
		send(clientid,buf,strlen(buf),0);
	}
	close(clientid);

	
}
