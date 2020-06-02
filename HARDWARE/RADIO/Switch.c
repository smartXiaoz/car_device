#include "Switch.h"
#include "reportTASK.h"

Switch_STA Switch={
2,//指令
4,//开关转态
};

void Switch_Init(void)
{
	RCC->APB2ENR|=1<<3; 
	GPIOB->CRH&=0X00FFFFFF; 
	GPIOB->CRH|=0X33000000; 
	Switch_S0_Pin = 1;
	Switch_S1_Pin = 0;
}

u8 Switch_ctrl(void)
{
	u8 re = 0;
	switch(Switch.sta)
		{
		case 1:	Switch_S0_Pin = 1,Switch_S1_Pin = 0;	break;	//	FM_Audio,收音机
		case 2:	Switch_S0_Pin = 0,Switch_S1_Pin = 1;	break;	//	BL_Audio,蓝牙
		case 3:	Switch_S0_Pin = 1,Switch_S1_Pin = 1;	break;	//	MCU_Audio,多媒体
		case 4:	Switch_S0_Pin = 0,Switch_S1_Pin = 0;	break;	//	PC_Audio,悬空
		default:re = 1;									break;
		}
	Report_STA |= 0x02;
	return re;
}










