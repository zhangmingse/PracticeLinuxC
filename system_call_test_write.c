#include <unistd.h>
#include <stdlib.h>

int main()
{
	if((write(1,"Here is some data\n",18))!=19 )
	{
		write(2,"A write error has occured on file descriptor 1\n",47);
	}
	exit(0);
}
