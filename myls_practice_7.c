#include<stdio.h>
#include<sys/types.h>
#include<dirent.h>

void do_ls(char dir_name[])
{
	DIR * dir;
	struct dirent * entry;
	if((dir=opendir(dir_name))!=NULL)
	{
		while((entry=readdir(dir))!=NULL)
		{
			printf("%s\n",entry->d_name);
		}
	}

}

void main(int ac,char * av[])
{
	if(ac == 1)
	{
		do_ls(".");
	}
	else
	{
		while(--ac)
		{
			printf("=========%s\n",*(++av));
			do_ls(*(av));
		}
	}
}
