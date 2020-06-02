#include "Switch.h"
#include "reportTASK.h"

Switch_STA Switch={
2,//ָ��
4,//����ת̬
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
		case 1:	Switch_S0_Pin = 1,Switch_S1_Pin = 0;	break;	//	FM_Audio,������
		case 2:	Switch_S0_Pin = 0,Switch_S1_Pin = 1;	break;	//	BL_Audio,����
		case 3:	Switch_S0_Pin = 1,Switch_S1_Pin = 1;	break;	//	MCU_Audio,��ý��
		case 4:	Switch_S0_Pin = 0,Switch_S1_Pin = 0;	break;	//	PC_Audio,����
		default:re = 1;									break;
		}
	Report_STA |= 0x02;
	return re;
}










