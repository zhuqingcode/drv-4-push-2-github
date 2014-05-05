
#ifndef _ISL1208_H__
#define _ISL1208_H__

#define RTC_I2C_W_ADDR 0xDE
#define RTC_I2C_R_ADDR 0xDF

#define MINUTE 60
#define HOUR (60*MINUTE)
#define DAY (24*HOUR)
#define YEAR (365*DAY)

/* interestingly, we assume leap-years */
/*
 static int month[12] = {
         0,
         DAY*(31),
         DAY*(31+29),
         DAY*(31+29+31),
         DAY*(31+29+31+30),
         DAY*(31+29+31+30+31),
         DAY*(31+29+31+30+31+30),
         DAY*(31+29+31+30+31+30+31),
         DAY*(31+29+31+30+31+30+31+31),
         DAY*(31+29+31+30+31+30+31+31+30),
         DAY*(31+29+31+30+31+30+31+31+30+31),
         DAY*(31+29+31+30+31+30+31+31+30+31+30)
 };
*/
/* Register map */
/* rtc section */
#define ISL1208_REG_SC  0x00
#define ISL1208_REG_MN  0x01
#define ISL1208_REG_HR  0x02
#define ISL1208_REG_HR_MIL     (1<<7)	/* 24h/12h mode */
#define ISL1208_REG_HR_PM      (1<<5)	/* PM/AM bit in 12h mode */
#define ISL1208_REG_DT  0x03
#define ISL1208_REG_MO  0x04
#define ISL1208_REG_YR  0x05
#define ISL1208_REG_DW  0x06
#define ISL1208_RTC_SECTION_LEN 7

/* control/status section */
#define ISL1208_REG_SR  0x07
#define ISL1208_REG_SR_ARST    (1<<7)	/* auto reset */
#define ISL1208_REG_SR_XTOSCB  (1<<6)	/* crystal oscillator */
#define ISL1208_REG_SR_WRTC    (1<<4)	/* write rtc */
#define ISL1208_REG_SR_ALM     (1<<2)	/* alarm */
#define ISL1208_REG_SR_BAT     (1<<1)	/* battery */
#define ISL1208_REG_SR_RTCF    (1<<0)	/* rtc fail */
#define ISL1208_REG_INT 0x08
#define ISL1208_REG_INT_ALME   (1<<6)   /* alarm enable */
#define ISL1208_REG_INT_IM     (1<<7)   /* interrupt/alarm mode */
#define ISL1208_REG_09  0x09	/* reserved */
#define ISL1208_REG_ATR 0x0a
#define ISL1208_REG_DTR 0x0b

/* alarm section */
#define ISL1208_REG_SCA 0x0c
#define ISL1208_REG_MNA 0x0d
#define ISL1208_REG_HRA 0x0e
#define ISL1208_REG_DTA 0x0f
#define ISL1208_REG_MOA 0x10
#define ISL1208_REG_DWA 0x11
#define ISL1208_ALARM_SECTION_LEN 6

/* user section */
#define ISL1208_REG_USR1 0x12
#define ISL1208_REG_USR2 0x13
#define ISL1208_USR_SECTION_LEN 2



   /* Bits in CONTROL register. */
#define RTC_CONTROL_WRITEPROTECT 	0x80
#define RTC_TRICKLECHARGER 		8
  
  /* Bits in TRICKLECHARGER register TCS TCS TCS TCS DS DS RS RS. */
#define RTC_TCR_PATTERN 	0xA0	/* 1010xxxx */
#define RTC_TCR_1DIOD 	0x04	/* xxxx01xx */
#define RTC_TCR_2DIOD 	0x08	/* xxxx10xx */
#define RTC_TCR_DISABLED 	0x00	/* xxxxxx00 Disabled */
#define RTC_TCR_2KOHM 	0x01	/* xxxxxx01 2KOhm */
#define RTC_TCR_4KOHM 	0x02	/* xxxxxx10 4kOhm */
#define RTC_TCR_8KOHM 	0x03	/* xxxxxx11 8kOhm */

#ifndef BCD_TO_BIN 
#define BCD_TO_BIN(val) ((val) = ((val)&15) + ((val)>>4) * 10  )
#endif

#ifndef BIN_TO_BCD
#define BIN_TO_BCD(val) ((((val)/10)<<4) + (val)%10)
#endif

#endif
