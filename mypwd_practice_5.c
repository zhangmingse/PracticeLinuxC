#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>
#include<dirent.h>

ino_t get_inode(char * fname)
{
	struct stat info;
	stat(fname,&info);
	return info.st_ino;
}

void inum_to_name(ino_t inode_to_find,char * namebuf,int buflen)
{
	DIR * dirptr;
	struct dirent * entryPtr;
	dirptr = opendir(".");
	while((entryPtr=readdir(dirptr))!=NULL)
	{
		if(entryPtr->d_ino==inode_to_find)
		{
			strncpy(namebuf,entryPtr->d_name,buflen);
			return;

		}
	}
	printf("error looking for inum = %d\n",(int)inode_to_find);
	exit(1);
}

void printpath(ino_t this_node)
{
	char itsname[BUFSIZ];

	if(this_node != get_inode(".."))
	{
		chdir("..");
		inum_to_name(this_node,itsname,BUFSIZ);
		printpath(get_inode("."));
		printf("/%s",itsname);
	}
}

void main()
{
	printpath(get_inode("."));
	putchar('\n');
	return;
}
