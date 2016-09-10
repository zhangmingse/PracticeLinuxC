#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<string.h>

void main()
{
	DIR * dirptr;
	struct dirent * entry;
	int file_name_len;
	char * charptr;
	char filename[BUFSIZ];
	dirptr = opendir(".");
	if(dirptr == NULL)
	{
		printf("cannot open current direntory\n");
		return;
	}
	file_name_len = 0;
	charptr = NULL;
	while((entry = readdir(dirptr))!=NULL)
	{
		file_name_len = strlen(entry->d_name);
		if(file_name_len <3)
			continue;
		charptr = entry->d_name+file_name_len;
		if(*(charptr-1) == 'c'  && *(charptr - 2) == '.')
		{

			printf("%s\n",entry->d_name);
			strncpy(filename,entry->d_name,BUFSIZ);
			filename[file_name_len-2] = '\0';
			remove(filename);
		}
	}
}
