#include<stdio.h>
#include<utmp.h>
#include<fcntl.h>
#include<unistd.h>

#define SHOWHOST
void showtime(long timeval)
{
	char * cp;
	cp = (char *)ctime(&timeval);
	
	printf("%12.12s",cp+4);
}

void show_info(int i,struct utmp * utbufp)
{
	if(utbufp->ut_type != DEAD_PROCESS)
		return;
	printf("%4d ",i);
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

int main()
{
	struct utmp current_record;
	int utmpfd;
	int reclen=sizeof(current_record);
	if((utmpfd=open("/run/utmp",O_RDONLY))==-1)
	{
		perror(UTMP_FILE);
		_exit(1);
	}
	int i =0;
	while( read(utmpfd,&current_record,reclen)==reclen )
		show_info(i++,&current_record);
	close(utmpfd);
	return 0;
}
