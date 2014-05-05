#include <stdio.h>
#include <unistd.h>     
#include <sys/types.h>  
#include <sys/stat.h>   
#include <sys/ioctl.h> 
#include <fcntl.h>      
#include <termios.h>    
#include <errno.h>
#include <time.h>

#include "rtcisl1208.h"


int main(int argc, char **argv)
{
	int fd = -1;
	stk_rtc_time rtc_tm;
	struct tm *t;
	time_t tt;
	fd = open("/dev/rtcisl1208", O_RDWR);
	if(fd < 0)
	{
		printf("can't open file:/dev/rtcisl1208\n");
		return -1;
	}

        if (argc >= 2 && !strcmp(argv[1], "settime")){
		printf("please input Year:\n");
		scanf("%d", &rtc_tm.tm_year);

		printf("please input Month:\n");
		scanf("%d", &rtc_tm.tm_mon);

		printf("please input Day:\n");
		scanf("%d", &rtc_tm.tm_mday);

		printf("please input Hour:\n");
		scanf("%d", &rtc_tm.tm_hour);

		printf("please input Minute:\n");
		scanf("%d", &rtc_tm.tm_min);

		printf("please input Second:\n");
		scanf("%d", &rtc_tm.tm_sec);
		
		printf("please input Weekday:\n");
		scanf("%d", &rtc_tm.tm_wday);
		
		printf("args you input:%d-%d-%d %d:%d:%d\n", rtc_tm.tm_year,rtc_tm.tm_mon,rtc_tm.tm_mday,rtc_tm.tm_hour,rtc_tm.tm_min,rtc_tm.tm_sec);
}
#if 0
	rtc_tm.tm_year = 2013;
	rtc_tm.tm_mon = 9;
	rtc_tm.tm_mday = 3;
	rtc_tm.tm_hour = 10;
	rtc_tm.tm_min = 0;
	rtc_tm.tm_sec = 0;
	rtc_tm.tm_wday = 2;
#endif
	ioctl(fd, TYPE_RTC_SET_TIME, &rtc_tm);
	//ioctl(fd, TYPE_RTC_GET_TIME, &rtc_tm);
	//printf("RTC time: %d-%d-%d %d:%d:%d\n",rtc_tm.tm_year,rtc_tm.tm_mon,rtc_tm.tm_mday,rtc_tm.tm_hour,rtc_tm.tm_min,rtc_tm.tm_sec);
	//ioctl(fd, TYPE_RTC_RTC2SYS_SET);
	
	while(1)
	{
		#if 1
		ioctl(fd, TYPE_RTC_GET_TIME, &rtc_tm);
		//printf("----------------------------------------------------------------\n");
		printf("RTC time: %d-%d-%d %d:%d:%d\n",rtc_tm.tm_year,rtc_tm.tm_mon,rtc_tm.tm_mday,rtc_tm.tm_hour,rtc_tm.tm_min,rtc_tm.tm_sec);
		//printf(" <----> ");
		//time(&tt);
		//t = localtime(&tt); 
		//printf("SYS time: %d-%d-%d %d:%d:%d\n",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
		sleep(10);
		#endif
		#if 0
		printf("----------------------------------------------------------------\n");
		time(&tt);
		t = localtime(&tt); 
		printf("SYS time: %d-%d-%d %d:%d:%d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
		printf(" <----> ");
		ioctl(fd, TYPE_RTC_GET_TIME, &rtc_tm);
		printf("RTC time: %d-%d-%d %d:%d:%d\n",rtc_tm.tm_year,rtc_tm.tm_mon,rtc_tm.tm_mday,rtc_tm.tm_hour,rtc_tm.tm_min,rtc_tm.tm_sec);
		usleep(100 * 1000);
		#endif
	}
	
}

