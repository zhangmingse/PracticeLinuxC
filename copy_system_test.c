#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdlib.h>

int main()
{
	char c;
	char block[1024];
	int in,out;
	int nread;
	in = open("mydata",O_RDONLY);
	out = open("mydataout",O_WRONLY | O_CREAT, S_IRUSR|S_IWUSR);
	while((nread=read(in,block,sizeof(block))) > 0)
	{
		write(out,&c,nread);
	}
	exit(0);
}
