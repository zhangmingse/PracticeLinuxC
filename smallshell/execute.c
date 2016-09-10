#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<sys/wait.h>

int execute(char **arglist)
{
	int pid;
	int status;

	if(arglist[0] == NULL)
	{
		return 0;
	}
	if((pid = fork())==-1)
	{	
		perror("fork error\n");
	}
	else if(pid == 0)
	{
		signal(SIGINT,SIG_DFL);
		signal(SIGQUIT,SIG_DFL);
		execvp(arglist[0],arglist);
		perror("execvp error \n");
		exit(-1);
	}
	else
	{
		while(wait(&status)!=pid);
		return status;
	}
}
