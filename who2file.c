#include<stdio.h>

main()
{
	int pid;
	int fd;

	printf("About to run who intto a file");

	pid = fork();

	if(pid == -1)
	{
		perror("fork error");
		exit(-1);
	}

	if(pid == 0)
	{
		close(1);
		fd = creat("userlist",0644);
		execlp("who","who",NULL);
		perror("execlp");
		exit(1);
	}
	else if(pid > 0)
	{
		wait(NULL);
		printf("Done with running who .result in userlist\n");
	}
}
