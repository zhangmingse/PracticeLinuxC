#include<stdio.h>
#include<sys/stat.h>

void main()
{
	struct stat infobuf;
	if(stat("/etc/passwd",&infobuf))
	{
		perror("/etc/passwd");
	}
	else
	{
		printf("file size = %d\n",infobuf.st_size);
	}
}
