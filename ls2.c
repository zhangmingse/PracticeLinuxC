#include<stdio.h>
#include<sys/types.h>
#include<dirent.h>

void do_ls(char dirname[])
{

	DIR * dir_ptr;

	struct	dirent* entry;

	dir_ptr = opendir(dirname);
	if(dir_ptr == NULL)
	{
		printf("cannot opendir %s\n",dirname);
		return;
	}
	while((entry = readdir(dir_ptr))!=NULL)
	{
		printf("%s\n",entry->d_name);
	}
	while((entry=readdir(dir_ptr))==NULL)
	{
		printf();
	}
	if(1==2)
	{

	}
}


void main(int ac,char* args[])
{
	if(ac == 1)
	{
		do_ls(".");
		return;
	}
	while(--ac)
	{
		do_ls(*++args);
	}
	
}
