#include<stdio.h>
#include<stdlib.h>

void main()
{
	printf("start\n");
	pid_t fpid;
	fpid=fork();
	if(fpid == 0)
	{
		printf("child pid = %d \n",getpid());
		printf("child stat to sleep!\n");
		usleep(2000000);
		printf("child wake up \n");
	}
	else
	{
		printf("father pid = %d\n",getpid());
		printf("fpid = %d \n",fpid);
	}
	printf("process exit pid = %d \n",getpid());
	exit(0);
}
