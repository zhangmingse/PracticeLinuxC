#include "mytcpheader.h"

void main()
{
	struct sockaddr_in s_addr;
	int port = 7777;
	int s_fd;
	int c_fd;
	int buf_size = 1024;
	char buf[buf_size];

	memset(&s_addr,0,sizeof(s_addr));
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(port);
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	s_fd = socket(s_addr.sin_family,SOCK_STREAM,0);

	bind(s_fd,(struct sockaddr *)&s_addr,sizeof(s_addr));
	listen(s_fd,1);
	c_fd = accept(s_fd,NULL,NULL);
	int readlen = 0;
	while((readlen = recv(c_fd,buf,buf_size,0))>0)
	{
		if(readlen > buf_size - 3)
			readlen = buf_size - 1;

		buf[readlen] = '\0';
		printf("%s\n",buf);
	}

	close(s_fd);
	close(c_fd);
	return;
}
