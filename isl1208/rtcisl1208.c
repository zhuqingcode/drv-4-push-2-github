/* 
 * 
 *
 * Copyright (c) 2013 talent Co., Ltd. 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; 
 *
 * History: 
 *      10-April-2006 create this file
 */
 
 

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <linux/poll.h>
#include <linux/interrupt.h>
#include <linux/timer.h>
#include <linux/miscdevice.h>
#include <linux/errno.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/ioctl.h>


#include "rtcisl1208.h"
#include "isl1208.h"

extern void isl1208_getrtctime(stk_rtc_time *rtc_tm);
extern int isl1208_setrtctime(stk_rtc_time *tm);
extern void isl1208_setsystime(stk_rtc_time *prtc_tm);
extern int isl1208_init(void);
#ifdef RTC_TIMER
struct timer_list rtcisl1208_timer;
void rtcisl1208_setup(unsigned char ntime);
#endif
static void isl1208_rtctosystime(void)
{
	stk_rtc_time rtc_tm;
	isl1208_getrtctime(&rtc_tm);

#if RTC_DEBUG

	printk("driver set sys timer: %d-%d-%d %d:%d:%d\n\n",rtc_tm.tm_year,
		rtc_tm.tm_mon,rtc_tm.tm_mday,
		rtc_tm.tm_hour,rtc_tm.tm_min,rtc_tm.tm_sec);
#endif
	isl1208_setsystime(&rtc_tm);
	return;
}

#ifdef USE_TIMER
static void rtcisl1208_handler(unsigned long time)
{
	//process
	unsigned char ntime = 10; // 30
	isl1208_rtctosystime();
	// add time
	rtcisl1208_setup(ntime);
	add_timer(&rtcisl1208_timer) ;
	return;
}	

static void rtcisl1208_setup(unsigned char ntime)
{
	/*** Initialize the timer structure***/
	init_timer(&rtcisl1208_timer) ;
	rtcisl1208_timer.function = rtcisl1208_handler ;
	rtcisl1208_timer.expires = jiffies + HZ*ntime; // HZ = 100 
	/***Initialisation ends***/
	return;
}
#endif

/* 
 *	open dev, init gpio
 */
static int rtcisl1208_open(struct inode *inode ,struct file *file)
{
    	return 0;
}

/*
 *	close device, do nothing
 */
static int rtcisl1208_release(struct inode *inode ,struct file *file)
{
    	return 0;
}


/*
 *	ioctl method
 */
static long rtcisl1208_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {	
		case TYPE_RTC_GET_TIME:	{
			stk_rtc_time rtc_tm;
					
			memset(&rtc_tm, 0, sizeof (stk_rtc_time));
			isl1208_getrtctime(&rtc_tm);						
			if (copy_to_user((stk_rtc_time*)arg, &rtc_tm, sizeof(stk_rtc_time)))
			return -EFAULT;	
			return 0;
		}

		case TYPE_RTC_SET_TIME:	{
			stk_rtc_time rtc_tm;
			if (!capable(CAP_SYS_TIME))
			return -EPERM;
			if (copy_from_user(&rtc_tm, (stk_rtc_time*)arg, sizeof(stk_rtc_time)))
			return -EFAULT;
			isl1208_setrtctime(&rtc_tm);
			return 0;
		}

		case TYPE_RTC_RTC2SYS_SET: {
			isl1208_rtctosystime();
			//printk("set systime ok!\n");
			return 0;
			}
			default:
			return -ENOIOCTLCMD;
		} 
	return 0;
} 
  
 
static struct file_operations rtcisl1208_fops =
{ 
  	.owner = THIS_MODULE, 
  	.open = rtcisl1208_open,
  	.release = rtcisl1208_release,
  	.unlocked_ioctl = rtcisl1208_ioctl,     
}; 

static struct miscdevice rtcisl1208_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "rtcisl1208",
	.fops = &rtcisl1208_fops,
};


static int __init  rtcisl1208_init(void)
{ 
	int ret = 0;
	
	//unsigned char ntime = 10;//30
	ret = misc_register(&rtcisl1208_dev);
	if (ret) 
	{ 
		printk("rtcisl1208 can't register major number\n"); 
    	return ret; 
	}
	isl1208_init();
	#ifdef USE_TIMER
	stk_rtcds1307_setup(ntime);//comment it or not
	add_timer(&stk_rtcds1307_timer);// comment it or not
	#endif
	isl1208_rtctosystime();
	printk("rtc chip(isl1208)'s driver @hi3520d\n");
    return 0; 
} 


static void __exit rtcisl1208_exit(void) 
{ 
	//del_timer_sync(&stk_rtcds1307_timer);//comment it or not
    misc_deregister(&rtcisl1208_dev);
}


module_init(rtcisl1208_init);
module_exit(rtcisl1208_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zhuqing@njtalent.cn");
MODULE_DESCRIPTION("rtc chip(isl1208)'s driver @hi3520d");



