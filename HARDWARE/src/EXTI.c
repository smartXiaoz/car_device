#include "EXTI.h"
#include "information.h"

//�ⲿ�жϳ�ʼ������
void ACCEXT_Init(void)
{
	RCC->APB2ENR|=1<<2;     			//ʹ��PORTAʱ��
	GPIOA->CRL&=0XFF0FFFFF;					  
	GPIOA->CRL|=0X00800000;				//PA5���ó����룬Ĭ������
	PAout(5) = 0;						//����
	Ex_NVIC_Config(GPIO_A,5,RTIR); 	//�����ش���
	MY_NVIC_Init(6,0,EXTI9_5_IRQn,4);	//��ռ6�������ȼ�0����4	  
	EXTI->PR=1<<5;						//�����ⲿ�ж�
}


void EXTI9_5_IRQHandler(void)
{	 
	EXTI->PR=1<<5;  		//���LINE5�ϵ��жϱ�־λ 
	
#ifdef	SKY121
SYS_Inf.ACC_AWAKE = 1;
#endif
//	EXTI->IMR&=~(1<<5);	//�ر��ⲿ�ж�
}


//�ⲿ�жϳ�ʼ������
void EXTIX_Init(void)
{
	RCC->APB2ENR|=1<<2;     			//ʹ��PORTAʱ��
	GPIOA->CRH&=0XFFFF0FFF;					  
	GPIOA->CRH|=0X00008000;				//PA11���ó����룬Ĭ������
	PAout(11) = 1;						//����
	Ex_NVIC_Config(GPIO_A,11,FTIR); 	//�½��ش���
	MY_NVIC_Init(6,0,EXTI15_10_IRQn,4);	//��ռ6�������ȼ�0����4	  
	EXTI->PR=1<<11;						//�����ⲿ�ж�
	
	ACCEXT_Init();
}


void EXTI15_10_IRQHandler(void)
{	 
	EXTI->PR=1<<11;  		//���LINE11�ϵ��жϱ�־λ 
//	EXTI->IMR&=~(1<<11);	//�ر��ⲿ�жϣ��ж����ͻ����ˣ������ó�CAN�ˣ�
}



void disable_exit(void)
{
	EXTI->IMR&=~(1<<5);
	EXTI->IMR&=~(1<<11);
}
















