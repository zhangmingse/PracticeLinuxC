#include<stdio.h>
#include<stdlib.h>
#include<string.h>


#define MAXARGS 20
#define ARGBUFLEN 100

void execute(char * arg,char * arglist[])
{
	pid_t pid ;
	int status;
	pid = fork();
	switch(pid)
	{
		case -1:
			perror("fork error !\n");
			exit(-1);
		case 0:
			execvp(arg,arglist);
			perror("execp error \n");
			exit(-2);
		default:
			while(wait(&status)!= pid);
			printf("child exit with status %d \n",status);
			break;
	}
}

char * makearg(char * argbuf)
{
	argbuf[strlen(argbuf)-1] = '\0';
	char * arg =(char *)malloc(strlen(argbuf));
	strcpy(arg,argbuf);
	return arg;
}

void main()
{
	char * arglist[MAXARGS+1];
	int argnum;
	char argbuf[ARGBUFLEN];

	int pid = 0;
	while(1)
	{
		argnum = 0;
		for(;argnum<MAXARGS;argnum++)
		{
			printf("ARG[%d]:\n",argnum);
			fgets(argbuf,ARGBUFLEN,stdin);
			if(*argbuf == '\n')
				break;
			arglist[argnum] = makearg(argbuf);
		}
		if(argnum == 0)
		{
			printf("no argument\n");
			continue;
		}
		arglist[argnum] = NULL;
		execute(arglist[0],arglist);
	}
}
