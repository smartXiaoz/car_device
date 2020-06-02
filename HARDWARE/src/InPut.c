#include "InPut.h"
#include "information.h"

//按键初始化函数
void InPut_Init(void)
{
	RCC->APB2ENR|=1<<2;     //使能PORTA时钟
	RCC->APB2ENR|=1<<4;     //使能PORTC时钟
	
	GPIOA->CRL&=0XFF0FFF0F;		//PA5设置成输入，默认下拉	  
	GPIOA->CRL|=0X00800080; 
	GPIOA->ODR&=~(1<<5);		//PA5 下拉
	GPIOA->ODR&=~(1<<1);		//PA2 下拉
	
	
#ifdef	SKY121
	GPIOC->CRL&=0XFFFFF0FF;		//PC2设置成输入	  
	GPIOC->CRL|=0X00000800; 			
	GPIOC->ODR&=~(1<<2);			//PC2 下拉
//	GPIOC->ODR|=1<<2;			//PC2 下拉
#endif
	
//	GPIOC->ODR&=~(1<<2);		//PC2 下拉
} 


u8 InPut_Scan(void)
{	 
	static u8 cnt0 = 0;
	static u8 cnt1 = 0;
	static u8 cnt2 = 0;
	
#ifdef	SKY121
	
	static u8 cnt3 = 0;	
	
//	if(BACK_IN == 0)
	if(BACK_IN == 1)
	{
		if(cnt3<5)cnt3++;
		else
		{
			CAR_Inf.ReverseSta = 1;
		}
	}
	else
	{
		cnt3 = 0;
		CAR_Inf.ReverseSta = 0;
	}
#endif
	
	if(ACC_IN == 1)	//ACC信号，延时上电
	{
		if(cnt0<5)cnt0++;
		else
		{
			CAR_Inf.ACC = 1;
			cnt1 = 0;
		}
	}
	else //ACC信号消失，延时下电
	{
		if(cnt1<5)cnt1++;
		else
		{
			CAR_Inf.ACC = 0;
#ifdef EMMCDEBUG
//			CAR_Inf.ACC = 1;//不注释，上电不检测ACC信号，直接启动，
#endif
			cnt0 = 0;
		}
	}
	
	if(SYS_Inf.MPU_STA == 1)
	{
		if(Blight_IN == 1)	//背光信号
		{
			CAR_Inf.NIGHTLIGHT = 1;
#ifdef EMMCDEBUG
//			CAR_Inf.NIGHTLIGHT = 0;
#endif
			cnt2 = 0;
		}
		else //背光信号消失，延时下电
		{
			if(cnt2<50)cnt2++;
			else
			{
				CAR_Inf.NIGHTLIGHT = 0;
			}
		}
	}
	else
	{
		CAR_Inf.NIGHTLIGHT = 0;
	}
	return 0;
}




















