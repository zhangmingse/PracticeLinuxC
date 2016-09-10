#include<stdio.h>
#include<sys/stat.h>

void main()
{
	struct stat infobuf;
	if(stat("/etc/passwd",&infobuf))
		perror("/etc/passwd");
	else
	{
		printf("the file size is %d\n",infobuf.st_size);
	}
}
