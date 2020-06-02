#include "OutPut.h"
#include "information.h"
#include "usart.h"
//LED IO初始化
void OutPut_Init(void)
{
//	RCC->APB2ENR|=1<<2;		//使能PORTA时钟
	RCC->APB2ENR|=1<<3;		//使能PORTB时钟
	RCC->APB2ENR|=1<<4;		//使能PORTC时钟
	
//	GPIOA->CRL&=0XFFFFFF0F;	//B3 B4 B12 推挽输出
//	GPIOA->CRL|=0X00000030; 
	
	GPIOB->CRL&=0XFFF0FFFF;	//B3 B8 B12 推挽输出
	GPIOB->CRL|=0X00030000; 
	GPIOB->CRH&=0XFFF0FFF0; 
	GPIOB->CRH|=0X00080003; 
	
	GPIOC->CRL&=0XFFF00FFF; //C3 C10 C4 推挽输出
	GPIOC->CRL|=0X00033000;  	 
//	GPIOC->CRH&=0XFFFFFFF0; 
//	GPIOC->CRH|=0X00000003; 
	GPIOC->CRH&=0XFFFFF0FF; 
	GPIOC->CRH|=0X00000300; 
	
	
	
	/*
	
	
	
GPIOA->CRL&=0XFF000FFF;	//B3 B8 B12 推挽输出
GPIOA->CRL|=0X00333000; 

GPIOA->ODR&= ~(1<<3);//PA3 为 0
GPIOA->ODR|= 1<<3;//PA3 为 1

GPIOA->ODR&=~(3<<0);
	
	
	
	*/
	
	
	
	
//	CAN_ST = 0;
	
	CAN_ENpin = 0;			//CAN使能引脚
//	BLACK_ENpin = 0;		//屏幕背光使能引脚	PB12 由AC8225控制，MCU设为输入，不做控制
	POWER_ENpin = 0;		//电源使能引脚
	MUTE_ENpin = 0;			//功放静音引脚
	PAMP_ENpin = 0;			//功放使能引脚
	Astern_OPutPin = 1;		//倒车输出 
	
}


void OUT_DISABLE(void)
{
	GPIOB->CRL&=0X00FFFF00;
	GPIOB->ODR&=~(3<<0);     	//PB0,1 输出低
	GPIOB->ODR&=~(3<<6);     	//PB6,7 输出低
	
#ifdef TW2836
	GPIOA->CRL&=0XFFFF00FF;
	GPIOA->ODR&=~(3<<2);     	//PA2,3 输出低
#endif
	
	
    DMA1_Channel1->CCR &= (uint16_t)(~DMA_CCR1_EN);
	ADC1->CR2&=~(1<<0);	   //关闭AD转换器
}

#include "adc.h"
void OUT_ENABLE(void)
{
	
//	GPIOC->CRH&=0XFFFFF0FF; 
//	GPIOC->ODR&=~(1<<10);     	//
	
//	GPIOA->CRL&=0XFF0FFF0F;		//PA5设置成输入，下拉
//	GPIOA->CRL|=0X00300300;		//PA5设置成输入，下拉
//	GPIOA->ODR&=~(1<<2);     	//
//	GPIOA->ODR&=~(1<<5);     	//
#ifdef TW2836
	GPIOA->CRL|=0X00003300;
	GPIOA->ODR|=(3<<2);			//PB0,1 输出高
#endif
//	GPIOB->CRL&=0X00FFFF00;
	GPIOB->CRL|=0X33000033;
	GPIOB->ODR|=(3<<0);			//PB0,1 输出高
	GPIOB->ODR|=(3<<6);			//PB6,7 输出高
	
	Adc_Init();
//	DMA1_Channel1->CCR |= (uint16_t)(DMA_CCR1_EN);
//	ADC1->CR2 |=(1<<0);	   //关闭AD转换器
}


//倒车输出信号，高电平正常，低电平倒车
void Astern_Ctrl(u8 Sta)	
{
//	printf("to bank\r\n");
//	printf("sta = %d,LOA = %d \r\n",Sta,SYS_Config.LOA);
	
	
	if(SYS_Config.LOA == 0)return;
	if(Sta)	{SYS_Inf.KEY_ENABLE = 1;Astern_OPutPin=0;}
	else	
	{
		SYS_Inf.KEY_ENABLE = 0;
		Astern_OPutPin=1;
		
		SYS_Inf.MUTE_OK |= 0xF0;
		if(SYS_Inf.MUTE_OK == 0xFF)
		{
			Mute_Ctrl(SYS_Inf.MUTE_STA);
		}
	}
}










