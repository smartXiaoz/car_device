#ifndef __RTC_H
#define __RTC_H	  
#include "sys.h"

#define DATE	__DATE__
#define TIME	__TIME__
#define YEAR	((((DATE[7]-'0')*10+(DATE[8]-'0'))*10+(DATE[9]-'0'))*10+(DATE[10]-'0'))
#define MONTH	(DATE[2]=='n'?(DATE[1]=='a'?1:6)\
				:DATE[2]=='b'?2\
				:DATE[2]=='r'?(DATE[0]=='M'?3:4)\
				:DATE[2]=='y'?5\
				:DATE[2]=='l'?7\
				:DATE[2]=='g'?8\
				:DATE[2]=='p'?9\
				:DATE[2]=='t'?10\
				:DATE[2]=='v'?11:12)
#define DAY		((DATE[4]==' '?0:(DATE[4]-'0'))*10+(DATE[5]-'0'))
#define HOUR	((TIME[0]-'0')*10+(TIME[1]-'0'))
#define MINUTE	((TIME[3]-'0')*10+(TIME[4]-'0'))
#define SECOND	((TIME[6]-'0')*10+(TIME[7]-'0'))

//时间结构体
typedef struct 
{
	vu8 hour;
	vu8 min;
	vu8 sec;			
	//公历日月年周
	vu16 w_year;
	vu8  w_month;
	vu8  w_date;
	vu8  week;
	vu8  week2;
	u8 flag;
	u8 sta;
}_calendar_obj;		


extern _calendar_obj calendar;				//日历结构体
extern volatile u8	RTC_IRQflag;											

u8 RTCcn_Get(void);							//获取计数值
void Disp_Time(u8 x,u8 y,u8 size);			//在制定位置开始显示时间
void Disp_Week(u8 x,u8 y,u8 size,u8 lang);	//在指定位置显示星期
u8 RTC_Init(void);        					//初始化RTC,返回0,失败;1,成功;
u8 Is_Leap_Year(u16 year);					//平年,闰年判断
u8 RTC_Get(void);         					//获取时间   
u8 RTC_Get_Week(u16 year,u8 month,u8 day);
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);		//设置时间	
u8 RTC_Alarm_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);	//设置闹钟		
u8 RTC_Init2(void);
u8 RTC_Init3(void);
u8 RTC_Init4(void);
u32 Get_RTCcont(void);
#endif



















