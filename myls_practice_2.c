#include<stdio.h>
#include<sys/types.h>
#include<dirent.h>

void do_ls(char []);
void main(int ac,char * av[])
{
	if(ac == 1)
		do_ls(".");
	else
	{
		ac--;
		while(ac--)
		{
			printf("---------%s:\n",*(++av));
			do_ls(*(av));
		}
	}
}

void do_ls(char dirname[])
{
	DIR* dirptr;
	struct dirent * dirent_p;
	if((dirptr=opendir(dirname))==NULL)
		fprintf(stderr,"canno't open dir %s \n",dirname);
	else
	{
		while((dirent_p=readdir(dirptr))!=NULL)
		{
			printf("%s \n",dirent_p->d_name);
		}
		close(dirptr);
	}
}
