#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<netdb.h>
#include<unistd.h>
#include<string.h>


void main(int ac,char * av[])
{

	struct sockaddr_in servadd;
	struct hostent *hp;
	int sock_id ,sock_fd;
	char message[BUFSIZ];
	char request[BUFSIZ];
	int messlen;



	if(ac!=3)
	{
		printf("wrong arg number!\n exit\n");
		return;
	}

	bzero(&servadd,sizeof(servadd));
	hp = gethostbyname(av[1]);

	if(hp == NULL)
	{
		printf("host name err\n");
		return;
	}
	bcopy(hp->h_addr,(struct sockadr *)&servadd.sin_addr,hp->h_length);
	servadd.sin_port = htons(atoi(av[2]));
	servadd.sin_family = AF_INET;


	while(1)
	{
		printf("please input your request:\n");
		fgets(request,BUFSIZ,stdin);
		printf("your request is : %s\n",request);
		
		sock_id = socket(AF_INET,SOCK_STREAM,0);
		if(sock_id == -1)
		{
			perror("socket error");
			continue;
		}

		if(connect(sock_id,(struct sockaddr *)&servadd,sizeof(servadd))!=0)
		{
			perror("connect error");
			exit(-2);
		}

		FILE * fp = fdopen(sock_id,"w");
		if(fp == NULL)
		{
			perror("fdopen");
			continue;
		}
		fprintf(fp,"%s\r\n",request);
		fflush(fp);
		while((messlen = read(sock_id,message,BUFSIZ))>0)
		{
			
			if(write(1,message,messlen)!=messlen)
			{
				perror("write error");
				break;
			}
		}
		close(sock_id);

	}
}


