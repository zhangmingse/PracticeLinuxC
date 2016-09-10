#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>

ino_t get_inode(char * fname)
{
	struct stat info;
	if(stat(fname,&info)==-1)
	{
		fprintf(stderr,"Cannot stat");
		exit(1);
	}
	return info.st_ino;
}

void inum_to_name(ino_t inode_to_find,char * namebuf,int buflen)
{
	DIR * dir_ptr;
	struct dirent * dirent_ptr;
	dir_ptr = opendir(".");
	if(dir_ptr ==NULL)
	{
		perror(".");
		exit(1);
	}
	while((dirent_ptr = readdir(dir_ptr))!=NULL)
	{
		if(dirent_ptr->d_ino == inode_to_find)
		{
			strncpy(namebuf,dirent_ptr->d_name,buflen);
			namebuf[buflen-1]=='\0';
			return ;
		}
	}
	printf("error looking for inum %d\n",(int)inode_to_find);
	exit(1);
}

void printpath(ino_t this_node)
{
	ino_t my_node;
	char its_name[BUFSIZ];
	if(get_inode("..")!=this_node)
	{
		chdir("..");
		inum_to_name(this_node,its_name,BUFSIZ);
		my_node=get_inode(".");
		printpath(my_node);
		printf("/%s",its_name);
	}
}

void main()
{
	printpath(get_inode("."));
	putchar('\n');
	return;
}
