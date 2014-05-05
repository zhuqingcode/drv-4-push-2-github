
#ifndef _RTC_ISL1208_H__
#define _RTC_ISL1208_H__
#define RTC_DEBUG 0
typedef struct _stk_rtc_time_s
{
	unsigned int tm_sec;
	unsigned int tm_min;
	unsigned int tm_hour;
	unsigned int tm_mday;
	unsigned int tm_mon;
	unsigned int tm_year;
	unsigned int tm_wday;
	unsigned int tm_yday;
	unsigned int tm_isdst;
}stk_rtc_time;

#define ISL1208            'D'
#define TYPE_RTC_GET_TIME                    _IOWR(ISL1208, 1, stk_rtc_time)
#define TYPE_RTC_SET_TIME                    _IOWR(ISL1208, 2, stk_rtc_time)
#define TYPE_RTC_RTC2SYS_SET                    _IOWR(ISL1208, 3, stk_rtc_time)
#endif
