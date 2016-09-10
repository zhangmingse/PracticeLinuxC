#include<stdio.h>
#include  "smsh.h"

int is_control_command(char *);

int do_control_command(char **);
int ok_to_execute();

int process(char ** arglist)
{
	int rv = 0;
	
	if(arglist[0] == NULL)
		rv = 0;
	else if(is_control_command(arglist[0]))
		rv = do_control_command(arglist);
	else if(ok_to_execute())
	{
		if(!builtin_command(arglist,&rv))
		{
			rv = execute(arglist);
		}
	}
	
	return rv;
}

