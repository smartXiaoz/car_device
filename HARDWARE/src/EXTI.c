#include "EXTI.h"
#include "information.h"

//外部中断初始化程序
void ACCEXT_Init(void)
{
	RCC->APB2ENR|=1<<2;     			//使能PORTA时钟
	GPIOA->CRL&=0XFF0FFFFF;					  
	GPIOA->CRL|=0X00800000;				//PA5设置成输入，默认下拉
	PAout(5) = 0;						//下拉
	Ex_NVIC_Config(GPIO_A,5,RTIR); 	//上升沿触发
	MY_NVIC_Init(6,0,EXTI9_5_IRQn,4);	//抢占6，子优先级0，组4	  
	EXTI->PR=1<<5;						//开启外部中断
}


void EXTI9_5_IRQHandler(void)
{	 
	EXTI->PR=1<<5;  		//清除LINE5上的中断标志位 
	
#ifdef	SKY121
SYS_Inf.ACC_AWAKE = 1;
#endif
//	EXTI->IMR&=~(1<<5);	//关闭外部中断
}


//外部中断初始化程序
void EXTIX_Init(void)
{
	RCC->APB2ENR|=1<<2;     			//使能PORTA时钟
	GPIOA->CRH&=0XFFFF0FFF;					  
	GPIOA->CRH|=0X00008000;				//PA11设置成输入，默认下拉
	PAout(11) = 1;						//上拉
	Ex_NVIC_Config(GPIO_A,11,FTIR); 	//下降沿触发
	MY_NVIC_Init(6,0,EXTI15_10_IRQn,4);	//抢占6，子优先级0，组4	  
	EXTI->PR=1<<11;						//开启外部中断
	
	ACCEXT_Init();
}


void EXTI15_10_IRQHandler(void)
{	 
	EXTI->PR=1<<11;  		//清除LINE11上的中断标志位 
//	EXTI->IMR&=~(1<<11);	//关闭外部中断（中断来就唤醒了，该配置成CAN了）
}



void disable_exit(void)
{
	EXTI->IMR&=~(1<<5);
	EXTI->IMR&=~(1<<11);
}
















