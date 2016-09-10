#include<stdio.h>
#include<fcntl.h>

main()
{
	int fd;
	char line[100];

	fgets(line,100,stdin);printf("%s\n",line);	
	fgets(line,100,stdin);printf("%s\n",line);
	fgets(line,100,stdin);printf("%s\n",line);

	close(0);
	fd = open("/etc/passwd",O_RDONLY);
	if(fd!=0)
	{
		fprintf(stderr,"Could not open data sa fd 0\n");
	}

	
	fgets(line,100,stdin);printf("%s\n",line);
	fgets(line,100,stdin);printf("%s\n",line);
	fgets(line,100,stdin);printf("%s\n",line);
}
