#include<stdio.h>
#include<sys/types.h>
#include<dirent.h>


void do_ls(char[]);

void main(int ac, char * av[] )
{
	if(ac == 1)
	{
		do_ls(".");
	}
	else
	{
		while(ac--)
		{
			printf("%s:\n",*av);
			do_ls(*(av++));
		}
	}

}

void do_ls(char dirname[])
{
	DIR * dirptr;
	struct dirent * direntry_p;
	if((dirptr=opendir(dirname))==NULL)
	{
		fprintf(stderr,"myls canno't open %s\n",dirname);
	}
	else
	{
		while((direntry_p=readdir(dirptr))!=NULL)
		{
			printf("%s\n",direntry_p->d_name);
		}
		close(dirptr);
	}

}
