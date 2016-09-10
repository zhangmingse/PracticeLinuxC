#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<string.h>
#include<unistd.h>
#define oops(msg)  {perror(msg);exit(1);}



void main(int ac,char * av[])
{
	struct sockaddr_in servadd;
	struct hostent *hp;
	int sock_id,sock_fd;
	char message[BUFSIZ];
	int messagelen;

	sock_id = socket(AF_INET,SOCK_STREAM,0);

	if(sock_id == -1)
		oops("socket");

	bzero(&servadd,sizeof(servadd));

	hp = gethostbyname(av[1]);
	if(hp == NULL)
		oops(av[1]);

	bcopy(hp->h_addr,(struct sockaddr *)&servadd.sin_addr,hp->h_length);

	servadd.sin_port = htons(atoi(av[2]));
	servadd.sin_family = AF_INET;

	if(connect(sock_id,(struct sockaddr *)&servadd,sizeof(servadd))!=0)
		oops("connect");

	messagelen = read(sock_id,message,BUFSIZ);

	if(messagelen == -1)
		oops("read");
	if(write(1,message,messagelen)!=messagelen)
		oops("write");

	close(sock_id);
}
