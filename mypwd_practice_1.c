#include<stdlib.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
#include<string.h>
ino_t get_inode(char * name)
{
	struct stat info;
	if(stat(name,&info)==-1)
	{
		fprintf(stderr,"Cannot stat");
		perror(name);
		exit(1);
	}
	return info.st_ino;
}

void inum_to_name(ino_t inode_to_find,char * namebuf, int buflen)
{
	DIR * dir_ptr;
	struct dirent * dirent_ptr;

	dir_ptr = opendir(".");
	if(dir_ptr==NULL)
	{
		perror(".");
		exit(1);
	}
	while((dirent_ptr=readdir(dir_ptr))!=NULL)
	{
//		printf("##%d\n",(int)dirent_ptr->d_ino);
		if(dirent_ptr->d_ino == inode_to_find)
		{
			strncpy(namebuf,dirent_ptr->d_name,buflen);
			namebuf[buflen-1]='\0';
			closedir(dir_ptr);
			return;
		}
	}
	
	printf("error looking for inum %d\n",(int)inode_to_find);
	exit(1);
}

void printpathto(ino_t this_node)
{
	ino_t my_node;
	char its_name[BUFSIZ];
	if(get_inode("..")!=this_node)
	{
		chdir("..");
//		printf("-----");
		inum_to_name(this_node,its_name,BUFSIZ);
		my_node=get_inode(".");
		printpathto(my_node);
		printf("/%s",its_name);
	}
}

void main()
{
	printpathto(get_inode("."));
	putchar('\n');
	return;
}
