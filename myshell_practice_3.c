#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAXARGS 20
#define ARGBUFLEN 100

char * makestring(char* arg)
{
	char * argptr;
	arg[strlen(arg)-1] = '\0';
	argptr = (char * )malloc(strlen(arg));
	strcpy(argptr,arg);
	return argptr;
}


void execute(char * arglist[])
{
	pid_t pid;
	int status;
	pid = fork();
	switch(pid)
	{
		case -1:
			perror("fork error\n");
			exit(-1);
		case 0:
			printf("command %s \n",arglist[0]);
			execvp(arglist[0],arglist);
			perror("exevcp error\n");
			exit(-1);
		default:
			while(wait(&status)!=pid);
			printf("child process exit with status = %d\n",status);
	}
}

void main()
{
	int argnum = 0;
	char * arglist[MAXARGS+1];
	char argbuf[ARGBUFLEN];
	while(argnum<MAXARGS)
	{
		printf("ARG[%d]:",argnum);
		if(fgets(argbuf,ARGBUFLEN-1,stdin) && argbuf[0] != '\n')
		{
			arglist[argnum++] = makestring(argbuf);
		}
		else if(argbuf[0] == '\n')
		{
			if(argnum == 0)
			{
				printf("no argument! restart!\n");
				continue;
			}
			arglist[argnum] = NULL;
			execute(arglist);
			argnum = 0;
			continue;
		}
		else
		{
			
		}
	}
}
