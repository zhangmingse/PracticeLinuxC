#include<stdio.h>
#include<string.h>
#include<ctype.h>

#include"smsh.h"
//#include"varlib.h"


int builtin_command(char ** args,int *resultptr)
{
	int rv = 0;
	if(strcmp(args[0],"set") == 0)
	{
		VLlist();
		*resultptr = 0;
		rv = 1;
	}
	else if(strchr(args[0],'=') !=NULL)
	{
		*resultptr = assign(args[0]);
		if(*resultptr != -1)
			rv = 1;
	}
	else if(strcmp(args[0],"export")==0)
	{
		if(args[1] != NULL && okname(args[1]))
		{
			*resultptr = VLexport(args[1]);
		}
		else
		{
			*resultptr = 1;
		}
		rv = 1;
	}

	return rv;
}


int assign(char * str)
{
	char *cp;
	int rv;
	cp = strchr(str,'=');
	*cp = '\0';
	rv = (okname(str)?VLstore(str,cp+1):-1);
	*cp = '=';
	return rv;
}


int okname(char * str)
{
	char * cp;
	for(cp = str;*cp;cp ++)
	{
		if((isdigit(*cp) && cp == str) || !(isalnum(*cp) || *cp == '_'))
			return 0;

	}
	return (cp != str);

}
