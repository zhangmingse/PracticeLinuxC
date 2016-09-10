#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAXARGS 20
#define ARGBUFLEN 100

void execute(char * arglist[])
{
	pid_t pid;
	int status;
	pid = fork();
	switch(pid)
	{
		case -1:
			perror("fork error \n");
			exit(-1);
		case 0:
			execvp(arglist[0],arglist);
			perror("execvp error");
			exit(-2);
		default:
			while(wait(&status)!=pid);
			printf("child exit with status = %d\n",status);
	}
}

char * makestring(char * arg)
{
	char * argptr;
	arg[strlen(arg)-1] = '\0';
	argptr = (char*)malloc(strlen(arg));
	strcpy(argptr,arg);
	return argptr;
}

void main()
{
	char * arglist[MAXARGS];
	char  argbuf[ARGBUFLEN];
	int argnum = 0;

	while(argnum < MAXARGS)
	{
		printf("ARG[%d]:",argnum);
		if(fgets(argbuf,ARGBUFLEN,stdin) && argbuf[0]!='\n')
		{
			arglist[argnum++] = makestring(argbuf);
		}
		else if(argbuf[0] == '\n')
		{
			if(argnum == 0)
			{
				printf("no argument!restart!\n");
				continue;
			}
			arglist[argnum] = NULL;
			execute(arglist);
			argnum = 0;
			continue;
		}
		else
		{
			printf("fgets error .restart!\n");
			argnum = 0;
		}
	}
}
