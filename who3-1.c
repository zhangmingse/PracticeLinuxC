#include<stdio.h>
#include<sys/types.h>
#include<utmp.h>
#include<fcntl.h>
#include<time.h>
#include<stdlib.h>

#define SHOWHOST
#define MAX_SUPPORTED_SIZE 100 //for now

void show_info(int,struct utmp *);
void showtime(time_t);
static struct utmp utmparr[MAX_SUPPORTED_SIZE];

int main()
{
	struct utmp * utbufp, * utmp_next();//what's the meanning of * utmp_next() ?
	if(utmp_open("/run/utmp") == -1)
	{
		perror(UTMP_FILE);
		exit(1);
	}
	int i = 0;
	int count = 0;
	while(count < MAX_SUPPORTED_SIZE && (utbufp = utmp_next()) != NULL)
	{
		utmparr[count] = *utbufp;
		count++;
	}
	
	while(i++<count )
	{
		utbufp = utmparr+(i-1);
		if(utbufp->ut_type != USER_PROCESS)
			continue;
		int j = 0;
		int flag = 0;
		for(j = 0 ; j < count; j++)
		{
			struct utmp * uttemp = utmparr + j;
//			printf("my:%d   ck=>id :%s time: %d\n",utbufp->ut_tv.tv_sec,uttemp->ut_id,uttemp->ut_tv.tv_sec);
			if(!strcmp(utbufp->ut_id, uttemp->ut_id) && uttemp->ut_type != USER_PROCESS && uttemp->ut_tv.tv_sec > utbufp->ut_tv.tv_sec)
			{
				flag =1;
			}
			else
				continue;
		}
		if(flag == 0)
			show_info(i,utbufp);
	}
	utmp_close();
	return 0;
}

void show_info(int i,struct utmp * utbufp)
{
	printf("%4d  ",i);
	printf("ut->type:%d   ",utbufp->ut_type);
	printf("ut->ut_id:%s   ",utbufp->ut_id);
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
