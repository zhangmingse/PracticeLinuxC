#include<stdio.h>

void main()
{
	FILE * file;
	char c;
	file = fopen("/etc/passwd","r");
	if(file != NULL)
	{
		int space = 0;
		while((c=fgetc(file))!=EOF)
		{
			if(c==':')
			{
				space+=2;
				printf("\n");
				int space_temp = space;
				while(space_temp--)
				{
					printf(" ");
				}
			}
			else if(c=='\n')
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
