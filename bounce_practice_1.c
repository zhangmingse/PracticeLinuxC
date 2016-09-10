#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<curses.h>
#include<signal.h>
#include<string.h>

#define MESSAGE "o"
#define BLANK 	" "
#define COLS 50

int row;
int col;
int dir;


void move_msg(int signum)
{
	signal(SIGALRM,move_msg);
	move(row,col);
	addstr(BLANK);
	col += dir;
	if( col < 0)
		col = 0;
	else if( (col+strlen(MESSAGE)) > COLS)
		col = COLS - strlen(MESSAGE);
	move(row,col);
	addstr(MESSAGE);
	refresh();

}

int set_ticker(int n_msec)
{
	struct itimerval new_timeset;
	long n_sec,n_usecs;

	n_sec = n_msec/1000;
	n_usecs = (n_msec%1000)*1000L;

	new_timeset.it_interval.tv_sec = n_sec;
	new_timeset.it_interval.tv_usec = n_usecs;

	new_timeset.it_value.tv_sec = n_sec;
	new_timeset.it_value.tv_usec = n_msec;

	return setitimer(ITIMER_REAL,&new_timeset,NULL);
}


void main()
{
	int delay;
	int ndelay;

	int c ;

	initscr();
	crmode();
	noecho();
	clear();

	row  = 10;
	col =0;
	dir = 1;
	delay = 200;

	move(row,col);
	addstr(MESSAGE);
	refresh();
	
	signal(SIGALRM,move_msg);
	set_ticker(delay);

	while(1)
	{
		ndelay = 0;
		c = getch();
		if(c == 'Q') break;
		if(c == ' ') dir = -dir;
		if(c == 'f'  && delay > 2) ndelay = delay/2;
		if(c == 's') ndelay = delay *2;
		if(ndelay>0) set_ticker(ndelay);
	}
	endwin();
	return;
}
