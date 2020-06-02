#include "OutPut.h"
#include "information.h"
#include "usart.h"
//LED IO��ʼ��
void OutPut_Init(void)
{
//	RCC->APB2ENR|=1<<2;		//ʹ��PORTAʱ��
	RCC->APB2ENR|=1<<3;		//ʹ��PORTBʱ��
	RCC->APB2ENR|=1<<4;		//ʹ��PORTCʱ��
	
//	GPIOA->CRL&=0XFFFFFF0F;	//B3 B4 B12 �������
//	GPIOA->CRL|=0X00000030; 
	
	GPIOB->CRL&=0XFFF0FFFF;	//B3 B8 B12 �������
	GPIOB->CRL|=0X00030000; 
	GPIOB->CRH&=0XFFF0FFF0; 
	GPIOB->CRH|=0X00080003; 
	
	GPIOC->CRL&=0XFFF00FFF; //C3 C10 C4 �������
	GPIOC->CRL|=0X00033000;  	 
//	GPIOC->CRH&=0XFFFFFFF0; 
//	GPIOC->CRH|=0X00000003; 
	GPIOC->CRH&=0XFFFFF0FF; 
	GPIOC->CRH|=0X00000300; 
	
	
	
	/*
	
	
	
GPIOA->CRL&=0XFF000FFF;	//B3 B8 B12 �������
GPIOA->CRL|=0X00333000; 

GPIOA->ODR&= ~(1<<3);//PA3 Ϊ 0
GPIOA->ODR|= 1<<3;//PA3 Ϊ 1

GPIOA->ODR&=~(3<<0);
	
	
	
	*/
	
	
	
	
//	CAN_ST = 0;
	
	CAN_ENpin = 0;			//CANʹ������
//	BLACK_ENpin = 0;		//��Ļ����ʹ������	PB12 ��AC8225���ƣ�MCU��Ϊ���룬��������
	POWER_ENpin = 0;		//��Դʹ������
	MUTE_ENpin = 0;			//���ž�������
	PAMP_ENpin = 0;			//����ʹ������
	Astern_OPutPin = 1;		//������� 
	
}


void OUT_DISABLE(void)
{
	GPIOB->CRL&=0X00FFFF00;
	GPIOB->ODR&=~(3<<0);     	//PB0,1 �����
	GPIOB->ODR&=~(3<<6);     	//PB6,7 �����
	
#ifdef TW2836
	GPIOA->CRL&=0XFFFF00FF;
	GPIOA->ODR&=~(3<<2);     	//PA2,3 �����
#endif
	
	
    DMA1_Channel1->CCR &= (uint16_t)(~DMA_CCR1_EN);
	ADC1->CR2&=~(1<<0);	   //�ر�ADת����
}

#include "adc.h"
void OUT_ENABLE(void)
{
	
//	GPIOC->CRH&=0XFFFFF0FF; 
//	GPIOC->ODR&=~(1<<10);     	//
	
//	GPIOA->CRL&=0XFF0FFF0F;		//PA5���ó����룬����
//	GPIOA->CRL|=0X00300300;		//PA5���ó����룬����
//	GPIOA->ODR&=~(1<<2);     	//
//	GPIOA->ODR&=~(1<<5);     	//
#ifdef TW2836
	GPIOA->CRL|=0X00003300;
	GPIOA->ODR|=(3<<2);			//PB0,1 �����
#endif
//	GPIOB->CRL&=0X00FFFF00;
	GPIOB->CRL|=0X33000033;
	GPIOB->ODR|=(3<<0);			//PB0,1 �����
	GPIOB->ODR|=(3<<6);			//PB6,7 �����
	
	Adc_Init();
//	DMA1_Channel1->CCR |= (uint16_t)(DMA_CCR1_EN);
//	ADC1->CR2 |=(1<<0);	   //�ر�ADת����
}


//��������źţ��ߵ�ƽ�������͵�ƽ����
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










