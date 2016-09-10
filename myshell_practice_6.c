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
			perror("fork error\n");
			exit(-1);
		case 0:
			execvp(arglist[0],arglist);
			perror("execvp error\n");
			exit(-2);
		default:
			while(wait(&status)!=pid);
			int i = 0;
			for (i;i<MAXARGS;i++)
			{
				if(arglist[i]!=NULL)
				{
					printf("free %d\n",i);
					free(arglist[i]);
				}
			//	else
			//		break;
			}
			printf("child exit with status %d\n",status);
	}
}

char * makestring(char * arg)
{
	arg[strlen(arg)-1] = '\0';
	char * argptr = (char*)malloc(strlen(arg));
	strcpy(argptr,arg);
	return argptr;
}

void main()
{
	char * arglist[MAXARGS+1];
	char argbuf[ARGBUFLEN];

	int argnum = 0;

	for(argnum = 0;argnum<MAXARGS;argnum++)
	{
		arglist[argnum] = NULL;
	}
	argnum = 0;
	while(argnum < MAXARGS)
	{
		printf("ARG[%d]:",argnum);
		if(fgets(argbuf,ARGBUFLEN,stdin) && argbuf[0]!='\n')
		{
			arglist[argnum++] = makestring(argbuf);
		}
		else if(argbuf[0] == '\n')
		{
			if(argnum ==0)
			{
				printf("no argument! restart!\n");
				continue;
			}
			arglist[argnum] = NULL;
			execute(arglist);
			argnum = 0;
		}
		else
		{
			perror("fgets error \n");
			argnum = 0;
		}
	}
}
