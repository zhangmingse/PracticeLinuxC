#include<stdio.h>
#include<sys/types.h>
#include<dirent.h>


void do_ls(char * dirname)
{
	DIR * dirptr;
	struct dirent * entry;
	dirptr = opendir(dirname);
	if(dirptr == NULL)
	{
		printf("open dir %s error\n",dirname);
	}
	while((entry = readdir(dirptr))!=NULL)
	{
		printf("%s \n",entry->d_name);
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
