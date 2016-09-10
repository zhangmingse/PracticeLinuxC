#include<stdio.h>
#include<sys/types.h>
#include<utmp.h>
#include<fcntl.h>
#include<time.h>
#include<stdlib.h>

#define SHOWHOST

void show_info(int,struct utmp *);
void showtime(time_t);

int main()
{
	struct utmp * utbufp, * utmp_next();//what's the meanning of * utmp_next() ?
	if(utmp_open("/run/utmp") == -1)
	{
		perror(UTMP_FILE);
		exit(1);
	}
	int i = 0;
	while((utbufp = utmp_next()) != ((struct utmp *)NULL))
		show_info(i++,utbufp);
	utmp_close();
	return 0;
}

void show_info(int i,struct utmp * utbufp)
{
	if(utbufp->ut_type!=USER_PROCESS)
		return;
	printf("%4d  ",i);
	printf("ut->type:%d   ",utbufp->ut_type);
	printf("ut->ut_exit->e_exit:%d   ",utbufp->ut_exit.e_exit);
	printf(" session:%8ld ",utbufp->ut_session);
	printf("%-8.8s",utbufp->ut_name);
	printf(" ");
	printf("%-8.8s",utbufp->ut_line);
	printf(" ");
	showtime(utbufp->ut_time);
	printf(" ");
	#ifdef SHOWHOST
	printf("(%s)",utbufp->ut_host);
	#endif
	printf("\n");
}


void showtime(long timeval)
{
	char * cp;
	cp = (char *)ctime(&timeval);
	
	printf("%12.12s",cp+4);
}
