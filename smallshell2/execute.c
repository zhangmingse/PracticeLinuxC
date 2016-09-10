#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<signal.h>

int execute(char** arglist)
{
	int pid;
	int status;

	if(arglist[0] == NULL)
		return 0;
	
	pid = fork();
	if(pid == -1)
		perror("fork error");
	else if(pid == 0)
	{
		signal(SIGINT,SIG_DFL);
		signal(SIGQUIT,SIG_DFL);
		execvp(arglist[0],arglist);
		perror("cannot execute command\n");
		exit(1);
	}
	else
	{
		while(wait(&status)!=pid);
		return status;
	}
}
