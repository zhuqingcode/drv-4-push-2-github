/*
 * filename:	isl1208.c
 * designer:	zhuqing
 * date:		2014-1-15
 */
#include <linux/kernel.h>

#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/mm.h>
#include <linux/miscdevice.h>
#include <linux/proc_fs.h>

#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/system.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>

#include <linux/string.h>
#include <linux/list.h>
#include <asm/delay.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/poll.h>
#include <asm/bitops.h>
#include <asm/uaccess.h>
#include <asm/irq.h>

#include <linux/moduleparam.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/time.h>

#include "gpio_i2c.h"
#include "isl1208.h"
#include "rtcisl1208.h"

static unsigned char days_in_mo[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/* 
 *	 isl1208 base function
 */
unsigned char isl1208_readreg(unsigned char addr)
{
	return gpio_i2c_read(RTC_I2C_W_ADDR, addr);
	//return hi_i2c_read(RTC_I2C_R_ADDR,addr);
}

int isl1208_writereg(unsigned char addr, unsigned char data)
{
	int ret = 0;
	gpio_i2c_write(RTC_I2C_W_ADDR, addr, data);
	//hi_i2c_write(RTC_I2C_W_ADDR, addr, data);
	return ret;
}
/* 
 *	 isl1208 mktime
 */
long isl1208_mktime(stk_rtc_time * ptm)
{
	
	{
            if (0 >= (int) (ptm->tm_mon -= 2)) {    /* 1..12 -> 11,12,1..10 */
                ptm->tm_mon += 12;      /* Puts Feb last since it has leap day */
                ptm->tm_year -= 1;
            }

            return (((
                (unsigned long) (ptm->tm_year/4 - ptm->tm_year/100 + ptm->tm_year/400 + 367*ptm->tm_mon/12 + ptm->tm_mday) +
                    ptm->tm_year*365 - 719499
                )*24 + ptm->tm_hour /* now have hours */
              )*60 + ptm->tm_min /* now have minutes */
          )*60 + ptm->tm_sec; /* finally seconds */
        }

 }
/* 
 *	 isl1208 set sys time
 */
void isl1208_setsystime(stk_rtc_time *prtc_tm) 
{
	struct timespec tv;
	long time = 0;
	time = isl1208_mktime(prtc_tm);
	tv.tv_nsec = 0;
	//tv.tv_sec = time;
	tv.tv_sec = time;//just adjust it
	//printk("time:0x%x\n",time);
	do_settimeofday(&tv);
	//printk("set sys time ok!\n");
	return ;
}

int isl1208_setrtctime(stk_rtc_time *tm) 
{
	unsigned char sr;
	unsigned char mon, day, hrs, min, sec, leap_yr,weekday;
	unsigned int yrs;
#if  RTC_DEBUG
	printk("[%s: %d] set Rtc time: %d-%d-%d %d:%d:%d \n",__func__,__LINE__,tm->tm_year,
		tm->tm_mon,tm->tm_mday,
		tm->tm_hour,tm->tm_min,tm->tm_sec);
#endif

	yrs = tm->tm_year;
	mon = tm->tm_mon;   //   + 1;   /* tm_mon starts at zero */
	day = tm->tm_mday;
	hrs = tm->tm_hour;
	min = tm->tm_min;
	sec = tm->tm_sec;
	weekday = tm->tm_wday;

	
	
	if ((yrs < 2000) || (yrs > 2099))
		return 1;

	leap_yr = ((!(yrs % 4) && (yrs % 100)) || !(yrs % 400));

	if ((mon > 12) || (day == 0))
		return 1;

	if (day > (days_in_mo[mon] + ((mon == 2) && leap_yr)))
		return 1;
	
	if ((hrs >= 24) || (min >= 60) || (sec >= 60))
		return 1;

	if (yrs >= 2000)
		yrs -= 2000;	/* RTC (0, 1, ... 99) */
#if RTC_DEBUG
	printk("[%s: %d] driver set Rtc time: %d-%d-%d %d:%d:%d \n",__func__,__LINE__,yrs,mon,day,hrs,min,sec);
#endif

	sec = BIN_TO_BCD(sec);
	min = BIN_TO_BCD(min);
	hrs = BIN_TO_BCD(hrs) | ISL1208_REG_HR_MIL;
	day = BIN_TO_BCD(day);
	mon = BIN_TO_BCD(mon);
	yrs = BIN_TO_BCD(yrs);
	weekday = BIN_TO_BCD(weekday & 7);
	//local_irq_save(flags);
	//local_irq_disable();

	sr = isl1208_readreg(ISL1208_REG_SR);
	//printk("sr:0x%02x\n", sr);
	/* set WRTC */
	isl1208_writereg(ISL1208_REG_SR,
				       sr | ISL1208_REG_SR_WRTC);
	
	isl1208_writereg(ISL1208_REG_YR,yrs);
	isl1208_writereg(ISL1208_REG_MO,mon);			
	isl1208_writereg(ISL1208_REG_DT,day);
	isl1208_writereg(ISL1208_REG_HR,hrs);
	isl1208_writereg(ISL1208_REG_MN,min);
	isl1208_writereg(ISL1208_REG_SC,sec);
	isl1208_writereg(ISL1208_REG_DW,weekday);
	//local_irq_restore(flags);

	/* clear WRTC again */
	sr = isl1208_writereg(ISL1208_REG_SR,
				       sr & ~ISL1208_REG_SR_WRTC);

	//rtc_tm->tm_year += 2000;
	//rtc_tm->tm_mon--;//comment by Zhuqing in 2011.9.5
	isl1208_setsystime(tm);
	
	//ds1307_rtctosystime();
	return 0;
}
/* 
 *	 isl1208 get rtc time
 */
void isl1208_getrtctime(stk_rtc_time *rtc_tm) 
{
	unsigned char sr;
	rtc_tm->tm_sec = isl1208_readreg((unsigned char)ISL1208_REG_SC);
	rtc_tm->tm_min = isl1208_readreg((unsigned char)ISL1208_REG_MN);
	rtc_tm->tm_hour = isl1208_readreg((unsigned char)ISL1208_REG_HR) & 0x3f;
	rtc_tm->tm_mday = isl1208_readreg((unsigned char)ISL1208_REG_DT);
	rtc_tm->tm_mon = isl1208_readreg((unsigned char)ISL1208_REG_MO);
	rtc_tm->tm_year = isl1208_readreg((unsigned char)ISL1208_REG_YR);
	rtc_tm->tm_wday= isl1208_readreg((unsigned char)ISL1208_REG_DW);

	sr = isl1208_readreg(ISL1208_REG_SR);


#if 0
	printk("driver Get time before convert:%d-%d-%d    %d:%d:%d\n",	rtc_tm->tm_year,
			rtc_tm->tm_mon,rtc_tm->tm_mday ,
			rtc_tm->tm_hour,rtc_tm->tm_min ,rtc_tm->tm_sec);
	printk("sr:0x%02x\n", sr);
#endif
	BCD_TO_BIN(rtc_tm->tm_sec);
	BCD_TO_BIN(rtc_tm->tm_min);
	BCD_TO_BIN(rtc_tm->tm_hour);
	BCD_TO_BIN(rtc_tm->tm_mday);
	BCD_TO_BIN(rtc_tm->tm_mon);
	BCD_TO_BIN(rtc_tm->tm_year);
	BCD_TO_BIN(rtc_tm->tm_wday);
	/*
	 * Account for differences between how the RTC uses the values
	 * and how they are defined in a struct rtc_time;
	 */
	
	//rtc_tm->tm_mon--;//comment by Zhuqing in 2011.9.5
#if RTC_DEBUG
	printk("driver Get time:%d-%d-%d    %d:%d:%d\n",	rtc_tm->tm_year,
			rtc_tm->tm_mon,rtc_tm->tm_mday ,
			rtc_tm->tm_hour,rtc_tm->tm_min ,rtc_tm->tm_sec);
#endif
	rtc_tm->tm_year += 2000;
	return;
}
/* 
 *	initalize isl1208 config
 */

int isl1208_init(void)
{
	int ret = 0;
#if 0	
	stk_rtc_time rtc_tm;
	isl1208_writereg(0x00, 0x00);//set CH = 0 , enable the oscillator!!!	
	//ds1307_writereg(0x0F, 0x08);

	rtc_tm.tm_year = 2011;
	rtc_tm.tm_mon = 12;
	rtc_tm.tm_mday = 01;
	rtc_tm.tm_hour= 14;
	rtc_tm.tm_min = 40;
	rtc_tm.tm_sec = 30;

	isl1208_setrtctime(&rtc_tm);
#endif
	isl1208_writereg(ISL1208_REG_SR, 0x10);//set CH = 0 , enable the oscillator!!!	
	return ret;
}
