#include<stdio.h>
#include<stdlib.h>
void main()
{
	int pid;
	int fd;
	pid = fork();
	if(pid == 0)
	{
		char* dir = "./.";
		close(1);
		fd = creat("userlist",0644);
		execlp("ls","ls","-l","./.",NULL);
		perror("execlp error");
		exit(-1);
	}
	else if(pid > 0)
	{
		wait(NULL);
		printf("done\n");
	}
}
