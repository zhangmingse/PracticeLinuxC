#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>

char buf1[] = "abcdefghijkl";
char buf2[] = "ABCDEFGHIJKL";
int main()
{
	int fd;
	if((fd=open("file.hole",O_WRONLY|O_CREAT|O_TRUNC)) < 0)
	{
		printf("create error\n");
	}
	if(write(fd,buf1,10) != 10)
		printf("buf1 write error\n");
	
	if(lseek(fd,200,SEEK_SET) == -1)
		printf("lseek error");
	
	if(write(fd,buf2,10) != 10)
		printf("buf1 write error");

	exit(0);
}
