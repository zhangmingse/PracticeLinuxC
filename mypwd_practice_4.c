#include<stdio.h>
#include<stdlib.h>
#include<string.h>
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
	struct dirent* dirent_ptr;
	dir_ptr = opendir(".");
	if(dir_ptr==NULL)
	{
		fprintf(stderr,"error open dir .");
		exit(1);
	}
	while((dirent_ptr = readdir(dir_ptr))!=NULL)
	{
		if(dirent_ptr->d_ino == inode_to_find)
		{
			strncpy(namebuf,dirent_ptr->d_name,buflen);
			namebuf[buflen-1]='\0';
			return;
		}
	}

	fprintf(stderr,"error looking for inum %d",(int)inode_to_find);
	exit(1);
}

void printpathto(ino_t this_node)
{
	ino_t mynode;
	char itsname[BUFSIZ];
	if(this_node != get_inode(".."))
	{
		chdir("..");
		inum_to_name(this_node,itsname,BUFSIZ);
		mynode=get_inode(".");
		printpathto(mynode);
		printf("/%s",itsname);
	}
}

void main()
{
	printpathto(get_inode("."));
	putchar('\n');
	return;
}
