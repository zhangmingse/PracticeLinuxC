#include<stdio.h>
#include<stdlib.h>

void main()
{
	int  n;
	for(n=0;n<10;n++)
	{
		printf("my pid = %d , n = %d \n",getpid(),n);
		sleep(1);
		if(fork()!=0)
			exit(0);
	}
}
