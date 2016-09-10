#include<stdio.h>

void main()
{
	FILE * file;
	char c;
	int space = 0;
	int temp_space;
	file = fopen("/etc/passwd","r");
	if(file != NULL)
	{
		while((c=fgetc(file))!=EOF)
		{
			if(c == ':')
			{
				space += 2;
				printf("\n");
				temp_space = space;
				while(temp_space--)
				{
					printf(" ");
				}
			}
			else if(c == '\n')
			{
				space = 0;
				printf("\n");
			}
			else
			{
				printf("%c",c);
			}
		}
	}
}
