#include "timer.h"
#include "usart.h"

//��ʱ��2�жϷ������	 
void TIM2_IRQHandler(void)
{ 		    		  			    
	if(TIM2->SR&0X0001)//����ж�
	{
//		printf("USART2-SR = 0x%x \r\n",USART2->SR);				   				     	    	
	}				   
	TIM2->SR&=~(1<<0);//����жϱ�־λ 	    
}
//��ʱ��3�жϷ������	 
void TIM3_IRQHandler(void)
{ 		    		  			    
	if(TIM3->SR&0X0001)//����ж�
	{
		    				   				     	    	
	}				   
	TIM3->SR&=~(1<<0);//����жϱ�־λ 	    
}
//��ʱ��4�жϷ������	 
void TIM4_IRQHandler(void) 
{ 		    		  			    
	if(TIM4->SR&0X0001)//����ж�
	{
		    				   				     	    	
	}				   
	TIM4->SR&=~(1<<0);//����жϱ�־λ 	    
}
//TIM1_PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM1_PWM_Init(u16 arr,u16 psc)
{		 					 
	//�˲������ֶ��޸�IO������
	RCC->APB2ENR|=1<<11; 	//TIM1ʱ��ʹ��    
	GPIOA->CRH&=0XFFFF0FF0;	//PA8���֮ǰ������
	GPIOA->CRH|=0X0000B00B;	//���ù������ 
	
	TIM1->ARR=arr;			//�趨�������Զ���װֵ 
	TIM1->PSC=psc;			//Ԥ��Ƶ������
  
	TIM1->CCMR1|=7<<4;  	//CH1 PWM2ģʽ		 
	TIM1->CCMR1|=1<<3; 		//CH1Ԥװ��ʹ��	 
 	TIM1->CCER|=1<<0;   	//OC1 ���ʹ��	   
   
	TIM1->CCMR2|=7<<12;  	//CH4 PWM2ģʽ		 
	TIM1->CCMR2|=1<<11; 	//CH4Ԥװ��ʹ��	   
	TIM1->CCER|=1<<12;   	//OC4 ���ʹ��
	
	TIM1->BDTR|=1<<15;   	//MOE �����ʹ��	
	
	TIM1->CR1=0x0080;   	//ARPEʹ�� 
	TIM1->CR1|=0x01;    	//ʹ�ܶ�ʱ��1 										  
} 
//TIM3 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM3_PWM_Init(u16 arr,u16 psc)
{		 					 
	//�˲������ֶ��޸�IO������
	RCC->APB1ENR|=1<<1; 	//TIM3ʱ��ʹ��    
	RCC->APB2ENR|=1<<2;    	//ʹ��PORTAʱ��
	RCC->APB2ENR|=1<<3;    	//ʹ��PORTBʱ��	
	GPIOA->CRL&=0X00FFFFFF;	//PA6\7���
	GPIOA->CRL|=0XBB000000;	//���ù������ 
	GPIOB->CRL&=0XFFFFFF00;	//PB0\1���
	GPIOB->CRL|=0X000000BB;	//���ù������ 	  	 
	   
//	RCC->APB2ENR|=1<<0;     //��������ʱ��	   
//	AFIO->MAPR&=0XFFFFF3FF; //���MAPR��[11:10]
//	AFIO->MAPR|=1<<11;      //������ӳ��,TIM3_CH2->PB5

	TIM3->ARR=arr;			//�趨�������Զ���װֵ 
	TIM3->PSC=psc;			//Ԥ��Ƶ������Ƶ

	TIM3->CCMR1|=7<<4;  	//CH1 PWM2ģʽ		 
	TIM3->CCMR1|=1<<3; 		//CH1Ԥװ��ʹ��	   
	TIM3->CCER|=1<<0;   	//OC1 ���ʹ��
	
	TIM3->CCMR1|=7<<12;  	//CH2 PWM2ģʽ		 
	TIM3->CCMR1|=1<<11; 	//CH2Ԥװ��ʹ��	   
	TIM3->CCER|=1<<4;   	//OC2 ���ʹ��

	TIM3->CCMR2|=7<<4;  	//CH3 PWM2ģʽ		 
	TIM3->CCMR2|=1<<3; 		//CH3Ԥװ��ʹ��	   
	TIM3->CCER|=1<<8;   	//OC3 ���ʹ��
	
	TIM3->CCMR2|=7<<12;  	//CH4 PWM2ģʽ		 
	TIM3->CCMR2|=1<<11; 	//CH4Ԥװ��ʹ��	   
	TIM3->CCER|=1<<12;   	//OC4 ���ʹ��
	
	TIM3->BDTR|=1<<15;   	//MOE �����ʹ��	
	
	TIM3->CR1=0x0080;   	//ARPEʹ�� 
	TIM3->CR1|=0x01;    	//ʹ�ܶ�ʱ��3 											  
}
//ͨ�ö�ʱ��2�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��2!
void TIM2_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<0;	//TIM3ʱ��ʹ��    
 	TIM2->ARR=arr;  	//�趨�������Զ���װֵ//�պ�1ms    
	TIM2->PSC=psc;  	//Ԥ��Ƶ��7200,�õ�10Khz�ļ���ʱ��		  
	TIM2->DIER|=1<<0;   //��������ж�	  
	TIM2->CR1|=0x01;    //ʹ�ܶ�ʱ��2
  	MY_NVIC_Init(8,0,TIM2_IRQn,4);//��ռ1�������ȼ�3����2									 
}
//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<1;	//TIM3ʱ��ʹ��    
 	TIM3->ARR=arr;  	//�趨�������Զ���װֵ//�պ�1ms    
	TIM3->PSC=psc;  	//Ԥ��Ƶ��7200,�õ�10Khz�ļ���ʱ��		  
	TIM3->DIER|=1<<0;   //��������ж�	  
	TIM3->CR1|=0x01;    //ʹ�ܶ�ʱ��3
  	MY_NVIC_Init(8,0,TIM3_IRQn,4);//��ռ1�������ȼ�3����2									 
}
//ͨ�ö�ʱ��4�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��4!
void TIM4_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<2;	//TIM3ʱ��ʹ��    
 	TIM4->ARR=arr;  	//�趨�������Զ���װֵ//�պ�1ms    
	TIM4->PSC=psc;  	//Ԥ��Ƶ��7200,�õ�10Khz�ļ���ʱ��		  
	TIM4->DIER|=1<<0;   //��������ж�	  
	TIM4->CR1|=0x01;    //ʹ�ܶ�ʱ��4
  	MY_NVIC_Init(8,0,TIM4_IRQn,4);//��ռ1�������ȼ�3����2									 
}











