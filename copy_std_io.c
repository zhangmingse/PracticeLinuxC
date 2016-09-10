#include<stdlib.h>
#include<stdio.h>

int main()
{
	int c;
	FILE *in,*out;
	in = fopen("mydata","r");
	out= fopen("mydataout","w");

	while((c=fgetc(in))!=EOF)
	{
		fputc(c,out);
	}
	exit(0);
}
