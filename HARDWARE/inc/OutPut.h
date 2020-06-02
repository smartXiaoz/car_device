#ifndef __OUTPUT_H
#define __OUTPUT_H	 
#include "sys.h"

#define	ON	1
#define	OFF	0


//#define CAN_ENpin	PCout(9)		//CAN使能引脚
#define CAN_ENpin	PCout(4)		//CAN使能引脚
#define BLACK_ENpin PBout(12)		//屏幕背光使能引脚
#define POWER_ENpin PCout(3)		//电源使能引脚
//#define MUTE_ENpin	PBout(3)		//功放静音引脚
#define MUTE_ENpin	PBout(8)		//功放静音引脚
#define PAMP_ENpin	PBout(4)		//功放使能引脚
//#define Astern_OPutPin PCout(8)		//倒车输出
#define Astern_OPutPin PCout(10)		//倒车输出
#define MPU_ResetPin PCout(11)		//MPU复位

void OutPut_Init(void);	//初始化		
void OUT_DISABLE(void);
void OUT_ENABLE(void);

//高电平CAN低功耗，低电平正常工作
#define CAN_Ctrl(Sta)		if(Sta)	CAN_ENpin=0;\
							else	CAN_ENpin=1;

//屏幕背光控制，不使用
#define Black_Ctrl(Sta)		if(Sta)	BLACK_ENpin=1;\
							else	BLACK_ENpin=0;

//电源控制，高电平电源开，低电平电源关
#define Power_Ctrl(Sta)		if(Sta)	POWER_ENpin=1;\
							else	POWER_ENpin=0;

//功放静音控制，高电平正常，低电平MUTE
#define Mute_Ctrl(Sta)		if(Sta)	MUTE_ENpin=0;\
							else	MUTE_ENpin=1;

//功放休眠控制，高电平工作，低电平休眠
#define PAMP_Ctrl(Sta)		if(Sta)	PAMP_ENpin=1;\
							else	PAMP_ENpin=0;

////倒车输出信号，高电平正常，低电平倒车
//#define Astern_Ctrl(Sta)	if(Sta)	Astern_OPutPin=0;\
//							else	Astern_OPutPin=1;


//AC8255复位信号，高电平复位，低电平工作
#define MPUReset_Ctrl(Sta)	if(Sta)	MPU_ResetPin=1;\
							else	MPU_ResetPin=0;



void Astern_Ctrl(u8 Sta);
















#endif

















