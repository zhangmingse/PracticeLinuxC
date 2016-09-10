#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "smsh.h"


#define is_delim(x) ((x)== ' '|| (x)== '\t')

char *next_cmd(char * prompt,FILE *fp)
{
	char * buf;
	int bufspace = 0;
	int pos = 0;
	int c;

	printf("%s",prompt);

	while((c=getc(fp))!=EOF)
	{
		if(pos+1>bufspace)
		{
			if(bufspace == 0)
			{
				buf = emalloc(BUFSIZ);
			}
			else
			{
				buf = (char*)erealloc(buf,bufspace+BUFSIZ);
			}
			bufspace+=BUFSIZ;

		}

		if(c == '\n')
			break;

		buf[pos++] = c;
	}

	if(c==EOF && pos == 0)
		return NULL;
	buf[pos] ='\0';
	return buf;

}
char * newstr(char * start,int len)
{
	char * rv = emalloc(len +1);
	rv[len] = '\0';
	strncpy(rv,start,len);
	return rv;
}

char ** splitline(char * line)
{
	char ** args;
	int spots=0;
	int bufspace = 0;
	int argnum = 0;
	char * cp = line;
	char *start;
	int len;

	if(line ==NULL)
	{
		return NULL;
	}
	
	args = emalloc(BUFSIZ);
	bufspace = BUFSIZ;
	spots = BUFSIZ/sizeof(char*);

	while(*cp != '\0')
	{
		while(is_delim(*cp))
			cp++;

		if(*cp == '\0')
			break;
		
		if(argnum+1>spots)
		{
			args = erealloc(args,bufspace+BUFSIZ);
			bufspace+=BUFSIZ;
			spots+=(BUFSIZ/sizeof(char*));
		}

		start = cp;
		len =1;
		while(*++cp != '\0' && !(is_delim(*cp)))
			len++;

		args[argnum++] = newstr(start,len);
	}

	args[argnum] = NULL;
	return args;

}




void freelist(char ** arglist)
{
	char ** cp = arglist;
	while(*cp)
	{
		free(*cp++);
	}
	free(arglist);
}

void * emalloc(size_t n)
{
	void * rv;
	if((rv = malloc(n))==NULL)
		fatal("out of memory","",1);
	return rv;
}

void * erealloc(void * p,size_t n)
{
	void * rv;
	if((rv = (void *)realloc(p,n))==NULL)
	{
		fatal("relloc() failed","",1);
	}
	return rv;
}



















