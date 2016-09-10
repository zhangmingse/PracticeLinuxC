#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<sys/types.h>


void do_ls(char dirname[])
{
	DIR * dirptr;
	struct dirent * entry;

	dirptr = opendir(dirname);
	if(dirptr == NULL)
	{
		printf("cannot opendir %s\n",dirname);
		return;
	}
	while((entry = readdir(dirptr))!=NULL)
	{
		printf("%s\n",entry->d_name);
	}
}


void main(int ac,char * args[])
{
	if(ac == 1)
	{
		do_ls(".");
	}
	else
	{
		while(--ac)
		{
			do_ls(*++args);
		}
	}
}
