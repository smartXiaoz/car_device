#include "timer.h"
#include "usart.h"

//定时器2中断服务程序	 
void TIM2_IRQHandler(void)
{ 		    		  			    
	if(TIM2->SR&0X0001)//溢出中断
	{
//		printf("USART2-SR = 0x%x \r\n",USART2->SR);				   				     	    	
	}				   
	TIM2->SR&=~(1<<0);//清除中断标志位 	    
}
//定时器3中断服务程序	 
void TIM3_IRQHandler(void)
{ 		    		  			    
	if(TIM3->SR&0X0001)//溢出中断
	{
		    				   				     	    	
	}				   
	TIM3->SR&=~(1<<0);//清除中断标志位 	    
}
//定时器4中断服务程序	 
void TIM4_IRQHandler(void) 
{ 		    		  			    
	if(TIM4->SR&0X0001)//溢出中断
	{
		    				   				     	    	
	}				   
	TIM4->SR&=~(1<<0);//清除中断标志位 	    
}
//TIM1_PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM1_PWM_Init(u16 arr,u16 psc)
{		 					 
	//此部分需手动修改IO口设置
	RCC->APB2ENR|=1<<11; 	//TIM1时钟使能    
	GPIOA->CRH&=0XFFFF0FF0;	//PA8清除之前的设置
	GPIOA->CRH|=0X0000B00B;	//复用功能输出 
	
	TIM1->ARR=arr;			//设定计数器自动重装值 
	TIM1->PSC=psc;			//预分频器设置
  
	TIM1->CCMR1|=7<<4;  	//CH1 PWM2模式		 
	TIM1->CCMR1|=1<<3; 		//CH1预装载使能	 
 	TIM1->CCER|=1<<0;   	//OC1 输出使能	   
   
	TIM1->CCMR2|=7<<12;  	//CH4 PWM2模式		 
	TIM1->CCMR2|=1<<11; 	//CH4预装载使能	   
	TIM1->CCER|=1<<12;   	//OC4 输出使能
	
	TIM1->BDTR|=1<<15;   	//MOE 主输出使能	
	
	TIM1->CR1=0x0080;   	//ARPE使能 
	TIM1->CR1|=0x01;    	//使能定时器1 										  
} 
//TIM3 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM3_PWM_Init(u16 arr,u16 psc)
{		 					 
	//此部分需手动修改IO口设置
	RCC->APB1ENR|=1<<1; 	//TIM3时钟使能    
	RCC->APB2ENR|=1<<2;    	//使能PORTA时钟
	RCC->APB2ENR|=1<<3;    	//使能PORTB时钟	
	GPIOA->CRL&=0X00FFFFFF;	//PA6\7输出
	GPIOA->CRL|=0XBB000000;	//复用功能输出 
	GPIOB->CRL&=0XFFFFFF00;	//PB0\1输出
	GPIOB->CRL|=0X000000BB;	//复用功能输出 	  	 
	   
//	RCC->APB2ENR|=1<<0;     //开启辅助时钟	   
//	AFIO->MAPR&=0XFFFFF3FF; //清除MAPR的[11:10]
//	AFIO->MAPR|=1<<11;      //部分重映像,TIM3_CH2->PB5

	TIM3->ARR=arr;			//设定计数器自动重装值 
	TIM3->PSC=psc;			//预分频器不分频

	TIM3->CCMR1|=7<<4;  	//CH1 PWM2模式		 
	TIM3->CCMR1|=1<<3; 		//CH1预装载使能	   
	TIM3->CCER|=1<<0;   	//OC1 输出使能
	
	TIM3->CCMR1|=7<<12;  	//CH2 PWM2模式		 
	TIM3->CCMR1|=1<<11; 	//CH2预装载使能	   
	TIM3->CCER|=1<<4;   	//OC2 输出使能

	TIM3->CCMR2|=7<<4;  	//CH3 PWM2模式		 
	TIM3->CCMR2|=1<<3; 		//CH3预装载使能	   
	TIM3->CCER|=1<<8;   	//OC3 输出使能
	
	TIM3->CCMR2|=7<<12;  	//CH4 PWM2模式		 
	TIM3->CCMR2|=1<<11; 	//CH4预装载使能	   
	TIM3->CCER|=1<<12;   	//OC4 输出使能
	
	TIM3->BDTR|=1<<15;   	//MOE 主输出使能	
	
	TIM3->CR1=0x0080;   	//ARPE使能 
	TIM3->CR1|=0x01;    	//使能定时器3 											  
}
//通用定时器2中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器2!
void TIM2_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<0;	//TIM3时钟使能    
 	TIM2->ARR=arr;  	//设定计数器自动重装值//刚好1ms    
	TIM2->PSC=psc;  	//预分频器7200,得到10Khz的计数时钟		  
	TIM2->DIER|=1<<0;   //允许更新中断	  
	TIM2->CR1|=0x01;    //使能定时器2
  	MY_NVIC_Init(8,0,TIM2_IRQn,4);//抢占1，子优先级3，组2									 
}
//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<1;	//TIM3时钟使能    
 	TIM3->ARR=arr;  	//设定计数器自动重装值//刚好1ms    
	TIM3->PSC=psc;  	//预分频器7200,得到10Khz的计数时钟		  
	TIM3->DIER|=1<<0;   //允许更新中断	  
	TIM3->CR1|=0x01;    //使能定时器3
  	MY_NVIC_Init(8,0,TIM3_IRQn,4);//抢占1，子优先级3，组2									 
}
//通用定时器4中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器4!
void TIM4_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<2;	//TIM3时钟使能    
 	TIM4->ARR=arr;  	//设定计数器自动重装值//刚好1ms    
	TIM4->PSC=psc;  	//预分频器7200,得到10Khz的计数时钟		  
	TIM4->DIER|=1<<0;   //允许更新中断	  
	TIM4->CR1|=0x01;    //使能定时器4
  	MY_NVIC_Init(8,0,TIM4_IRQn,4);//抢占1，子优先级3，组2									 
}











