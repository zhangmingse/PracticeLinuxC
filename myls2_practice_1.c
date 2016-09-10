#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
#include<pwd.h>
#include<grp.h>

char * uid_to_name(uid_t uid)
{
	static char numstr[10];
	struct passwd * pw_ptr;
	if( (pw_ptr = getpwuid(uid))==NULL )
	{
		sprintf(numstr,"%d",uid);
		return numstr;
	}
	else
	{
		return pw_ptr->pw_name;
	}
}

char * gid_to_name(gid_t gid)
{
	struct group *  grp_ptr;
	static char numstr[10];

	if((grp_ptr = getgrgid(gid))==NULL)
	{
		sprintf(numstr,"%d",gid);
		return numstr;
	}
	else
	{
		return grp_ptr->gr_name;
	}
}

void mode_to_letters(int mode,char str[])
{
	strcpy(str,"----------");

	if(S_ISDIR(mode)) str[0] = 'd';
	if(S_ISCHR(mode)) str[0] = 'c';
	if(S_ISBLK(mode)) str[0] = 'b';

	if(mode & S_IRUSR) str[1] = 'r';
	if(mode & S_IWUSR) str[2] = 'w';
	if(mode & S_IXUSR) str[3] = 'x';

	if(mode & S_IRGRP) str[4] = 'r';
	if(mode & S_IWGRP) str[5] = 'w';
	if(mode & S_IXGRP) str[6] = 'x';

	if(mode & S_IROTH) str[7] = 'r';
	if(mode & S_IWOTH) str[8] = 'w';
	if(mode & S_IXOTH) str[9] = 'x';
}

void show_file_info(char * filename,struct stat * info)
{
	char modestr[11];

	mode_to_letters(info->st_mode,modestr);

	printf("%s",modestr);
	printf(" %4d",(int)info->st_nlink);
	printf(" %-8s",uid_to_name(info->st_uid));
	printf(" %-8s",gid_to_name(info->st_gid));
	printf(" %8ld",(long)info->st_size);
	printf(" %.12s",4+ctime(&info->st_mtime));
	printf(" %s\n",filename);
}

void do_stat(char * filename)
{
	struct stat info;
	if(stat(filename,&info) == -1)
	{
		perror(filename);
	}
	else
	{
		show_file_info(filename,&info);
	}
}

void do_ls(char * dirname)
{
	DIR * dir;
	struct dirent * entry;
	if((dir=opendir(dirname))!=NULL)
	{
		while((entry=readdir(dir))!=NULL)
		{
			//printf("%s\n",entry->d_name);
			do_stat(entry->d_name);
		}
		closedir(dir);
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
			printf("====>>>>%s<<<<====",*(++av));
			do_ls(*av);
		}
	}
}
