#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<assert.h>
#include<string.h>


int main()
{
	int keyboard;
	int ret,i;
	char c;
	fd_set readfd;
	struct timeval timeout;
	keyboard = open("/dev/tty",O_RDONLY | O_NONBLOCK);
//	printf("1\n");
	assert(keyboard > 0);
	
	while(1)
	{
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		FD_ZERO(&readfd);
		//printf("2\n");
		FD_SET(keyboard,&readfd);
		//printf("start invoke select");
		ret = select(keyboard+1,&readfd,NULL,NULL,&timeout);

		//printf("end invoke select");
		if(ret == -1)
		{
			perror("select error");
		}
		else if(ret)
		{
			if(FD_ISSET(keyboard,&readfd))
			{
				i = read(keyboard,&c,1);
				if(c == '\n')
				{
					continue;
				}
				printf("hehe the input is %c\n",c);
				
				fflush(stdout);
				if(c == 'q')
				{
					break;
				}
			}
		}
		else if(ret == 0)
		{
			printf("time out\n");
			fflush(stdout);
		}
	}
	return 0;
}
