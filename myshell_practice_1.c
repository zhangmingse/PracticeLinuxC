#include<stdio.h>
#include<signal.h>
#include<string.h>
#include<stdlib.h>

#define MAXARGS 20
#define ARGLEN 100

void parentprocesshandler(int s)
{
//	printf("parent ctrl+c been called\n");
}

void execute(char * arglist[])
{
	int pid,exitstatus;
	struct sigaction newhandler,prehandler;
	newhandler.sa_handler = parentprocesshandler;
//	newhandler.sa_handler = NULL;
	newhandler.sa_flags = SA_RESETHAND | SA_RESTART;

	sigaction(SIGINT,&newhandler,&prehandler);
	pid = fork();
	switch(pid)
	{
		case -1:
			perror("fork failed");
			exit(1);
		case 0:
			execvp(arglist[0],arglist);
			perror("execvp failed");
			exit(1);
		default:
			while(wait(&exitstatus) != pid);
//			sigaction(SIGINT,&prehandler,NULL);
			printf("child exited with status %d, %d\n",exitstatus>>8,exitstatus&0377);
	}
}


char * makestring(char * buf)
{
	char * cp;
	buf[strlen(buf)-1]='\0';
	cp =(char *) malloc(strlen(buf)+1);
	if(cp == NULL)
	{
		fprintf(stderr,"no memory\n");
		exit(1);
	}
	strcpy(cp,buf);
	return cp;
}

void main()
{
	char * arglist[MAXARGS+1];
	int numargs;
	char argbuf[ARGLEN];
	
	numargs = 0;

	while(numargs < MAXARGS)
	{
		printf("ARG[%d]?",numargs);
		if(fgets(argbuf,ARGLEN,stdin) && *argbuf != '\n')
			arglist[numargs++] = makestring(argbuf);
		else
		{
			if(numargs > 0)
			{
				arglist[numargs] = NULL;
				execute(arglist);
				numargs = 0;
			}
		}
	}
}

