#include<stdio.h>
#include<curses.h>

void main()
{
	initscr();
	clear();
	int i = 0;
	for(;i<LINES;i++)
	{
		clear();
		move(i,i+i);
		if(i%2==1)
		{
			standout();
		}
		addstr("Hello,world");
		if(i%2==1)
		{
			standend();
		}
		refresh();
		sleep(1);
	}
	getchar();
	endwin();
}
