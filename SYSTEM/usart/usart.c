#include "usart.h"	  
#include "FreeRTOS.h"  
#include "timers.h"
  
//��ʼ��IO ����1
//pclk2:PCLK2ʱ��Ƶ��(Mhz)
//bound:������ 
void uart1_init(u32 pclk2,u32 bound)
{
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������	 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<2;   //ʹ��PORTA��ʱ��  
	RCC->APB2ENR|=1<<14;  //ʹ�ܴ���ʱ�� 
	GPIOA->CRH&=0XFFFFF00F;//IO״̬���� 
	GPIOA->CRH|=0X000008B0;//IO״̬����
//	GPIOA->ODR|=1<<10;			//PA2 ����
		
	RCC->APB2RSTR|=1<<14;   //��λ����1
	RCC->APB2RSTR&=~(1<<14);//ֹͣ��λ	   	   
	//����������
 	USART1->BRR=mantissa; // ����������	 
	USART1->CR1|=0X200C;  //1λֹͣ,��У��λ.
#if EN_USART1_RX		  //���ʹ���˽���
	//ʹ�ܽ����ж� 
	USART1->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��	    	
	MY_NVIC_Init(8,0,USART1_IRQn,4);//��2��������ȼ� 
//	USART1_RX_STA=0;
#endif
}
//��ʼ��IO ����2
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������ 
void uart2_init(u32 pclk1,u32 bound)
{
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk1*1000000)/(bound*16);//�õ�USARTDIV
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������	 
    mantissa<<=4;
	mantissa+=fraction; 
	
	RCC->APB2ENR|=1<<2;   //ʹ��PORTA��ʱ��  
	
	RCC->APB1ENR|=1<<17;  //ʹ�ܴ���ʱ�� 
	GPIOA->CRL&=0XFFFF00FF;//IO״̬����
	GPIOA->CRL|=0X00008B00;//IO״̬���� 
	GPIOA->ODR|=1<<3;			//PA2 ����
	
	
	RCC->APB1RSTR|=1<<17;   //��λ����1
	RCC->APB1RSTR&=~(1<<17);//ֹͣ��λ	   	   
	//����������
 	USART2->BRR=mantissa; // ����������	 
	USART2->CR1|=0X200C;  //1λֹͣ,��У��λ.
	
#if EN_USART2_RX		  //���ʹ���˽���
	//ʹ�ܽ����ж� 
	USART2->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��
//	USART2->CR3|=0X0001;  //�����ж�
	MY_NVIC_Init(8,0,USART2_IRQn,4);//��2��������ȼ�
//	USART2_RX_STA=0;
#endif
}

//��ʼ��IO ����3
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������ 
void uart3_init(u32 pclk1,u32 bound)
{
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk1*1000000)/(bound*16);//�õ�USARTDIV
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������	 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<3;   //ʹ��PORTB��ʱ��  
	RCC->APB1ENR|=1<<18;  //ʹ�ܴ���ʱ�� 
	GPIOB->CRH&=0XFFFF00FF;//IO״̬����
	GPIOB->CRH|=0X00008B00;//IO״̬���� 
//	GPIOB->ODR|=1<<11;			//PA2 ����
	RCC->APB1RSTR|=1<<18;   //��λ����1
	RCC->APB1RSTR&=~(1<<18);//ֹͣ��λ	   	   
	//����������
 	USART3->BRR=mantissa; // ����������	 
	USART3->CR1|=0X200C;  //1λֹͣ,��У��λ.
#if EN_USART3_RX		  //���ʹ���˽���
	//ʹ�ܽ����ж� 
	USART3->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��	    	
	MY_NVIC_Init(8,0,USART3_IRQn,4);//��2��������ȼ� 
//	USART3_RX_STA=0;
#endif
}



//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc����
//printf�������ָ��fputc����fputc���������
//����ʹ�ô���1(USART1)���printf��Ϣ
int fputc(int ch, FILE *f)
{
#ifdef	PRINTEABLE
	while((USART1->SR&0X40)==0);//�ȴ���һ�δ������ݷ������  
	USART1->DR = (u8) ch;      	//дDR,����1����������
#endif
	return ch;
}
#endif 
//end
//////////////////////////////////////////////////////////////////



#if EN_USART1_RX  //���ʹ���˽���


//����1�жϷ������ 	
//u8 USART1_RX_BUF[USART_REC_LEN];	//���ջ���,���USART_REC_LEN���ֽ�.
//u16 USART1_RX_STA=0;				//����״̬���,bit15:������ɱ�־,bit14~0:���յ�����Ч�ֽ���	  
//extern xTimerHandle	Usart1RX_Timer_Handle;
//void Usart1RX_vTimerCallback(xTimerHandle xTimer){USART1_RX_STA |= 0x8000;} 
extern void Test_Receivebyte(uint8_t byte);
void USART1_IRQHandler(void)
{
	u8 ucTemp;	
	if(USART1->SR&(1<<5))//���յ�����
	{	 
		ucTemp = USART1->DR; 
		Test_Receivebyte(ucTemp);
	}
//	u8 ucTemp;	
//	if(USART1->SR&(1<<5))//���յ�����
//	{	 
//		ucTemp=USART1->DR; 
//		if((USART1_RX_STA&0x8000)==0)//����δ���
//		{		
//			USART1_RX_BUF[USART1_RX_STA&0X7FFF]=ucTemp;
//			USART1_RX_STA++;
//			xTimerResetFromISR(Usart1RX_Timer_Handle,0);
//		}	
//	}
}
#endif	



#if EN_USART3_RX	//���ʹ���˽���
//����2�жϷ������ 	
u8 USART3_RX_BUF[1030];	//���ջ���,���USART_REC_LEN���ֽ�.
u16 uart3write = 0;
u16 uart3read = 0;
void USART3_IRQHandler(void)
{
	u8 ucTemp;	
	if(USART3->SR&(1<<5))//���յ�����
	{	 
		ucTemp=USART3->DR; 
		USART3_RX_BUF[uart3write] = ucTemp;
		if(uart3write<1024)uart3write++;
		else uart3write = 0;
	}
}
//�ӻ�����ȡ��һ�ֽ�����
u8 uart3GetByte(u8 *Dat)
{
	if(uart3write == uart3read) return 0;//����գ�û�ö���
	else 
	{
		*Dat = USART3_RX_BUF[uart3read];
		if(uart3read<1024)uart3read++;
		else uart3read = 0;
		return 1;
	}
}

#endif	




#if EN_USART2_RX   //���ʹ���˽���
//����3�жϷ������ 
u16 uart2write = 0;
u16 uart2read = 0;
u8 USART2_RX_BUF[260];	//���ջ���,���USART_REC_LEN���ֽ�.
void USART2_IRQHandler(void)
{
	volatile u8 ucTemp;
	__IO u32 SR_STA;
	
	SR_STA = USART2->SR;
	if(SR_STA&(1<<3))//ORE
	{
		ucTemp = USART2->DR;
		USART2->SR &= ~(1<<3);
	}
	if(USART2->SR&(1<<5))//���յ�����
	{	 
		ucTemp=USART2->DR;
		USART2_RX_BUF[uart2write]=ucTemp;
		uart2write++;
		if(uart2write>=256)uart2write = 0;
	}
	if(SR_STA&(1<<0))//PE
	{
		ucTemp = USART2->DR;
		USART2->SR &= ~(1<<0);
	}	

	if(SR_STA&(1<<1))//FE
	{
		ucTemp = USART2->DR;
		USART2->SR &= ~(1<<1);
	}
}
//�ӻ�����ȡ��һ�ֽ�����
u8 uart2GetByte(u8 *Dat)
{
	if(uart2write == uart2read) return 0;//����գ�û�ö���
	else 
	{
		*Dat = USART2_RX_BUF[uart2read];
		uart2read++;
		if(uart2read>=256)uart2read = 0;
		return 1;
	}
}

#endif	


u8 Uart1_TxBuff(u8 *Dat)
{
	u8 i = 0;
	for(i=0;i<Dat[0];i++)
	{
		while((USART1->SR&0X40)==0);//�ȴ���һ�δ������ݷ������  
		USART1->DR = (u8) Dat[i+1];	//дDR,����1����������
	}
	return 0;
}
u8 Uart2_TxBuff(u8 *Dat)
{
	u8 i = 0;
#ifdef U2DATALOG
	printf("send data:   ");
#endif
	for(i=0;i<Dat[0];i++)
	{
		while((USART2->SR&0X40)==0);//�ȴ���һ�δ������ݷ������  
		USART2->DR = (u8) Dat[i+1];	//дDR,����1����������
#ifdef U2DATALOG
		if(Dat[i+1]>0x0f)	printf("%x ",Dat[i+1]);
		else				printf("0%x ",Dat[i+1]);
#endif	
	}
#ifdef U2DATALOG
	printf("\r\n");
#endif
	return 0;
}
u8 Uart3_TxBuff(u8 *Dat)
{
	u8 i = 0;
#ifdef U3DATALOG
	printf("send data:   ");
#endif
	for(i=0;i<Dat[0];i++)
	{
		while((USART3->SR&0X40)==0);//�ȴ���һ�δ������ݷ������  
		USART3->DR = (u8) Dat[i+1];	//дDR,����1����������
#ifdef U3DATALOG
		if(Dat[i+1]>0x0f)	printf("%x ",Dat[i+1]);
		else				printf("0%x ",Dat[i+1]);
#endif
	}
#ifdef U3DATALOG
	printf("\r\n");
#endif
	return 0;
}






