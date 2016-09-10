#include<stdio.h>
#include<sys/types.h>
#include<dirent.h>

void do_ls(char []);

void main(int ac ,char * av[])
{
	if(ac==1)
		do_ls(".");
	else
	{
		ac--;
		while(ac--)
		{
			printf("----%s\n",*(++av));
			do_ls(*av);
		}
	}
}

void do_ls(char dirname[])
{
	DIR* dirptr;
	struct dirent * direntptr;
	if((dirptr=opendir(dirname))==NULL)
	{
		fprintf(stderr,"canno't open %s\n",dirname);
	}
	else
	{
		while((direntptr=readdir(dirptr))!=NULL)
		{
			printf("%s\n",direntptr->d_name);
		}
	}
}
