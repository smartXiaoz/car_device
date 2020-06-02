#include "usart.h"	  
#include "FreeRTOS.h"  
#include "timers.h"
  
//初始化IO 串口1
//pclk2:PCLK2时钟频率(Mhz)
//bound:波特率 
void uart1_init(u32 pclk2,u32 bound)
{
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//得到USARTDIV
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分	 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<2;   //使能PORTA口时钟  
	RCC->APB2ENR|=1<<14;  //使能串口时钟 
	GPIOA->CRH&=0XFFFFF00F;//IO状态设置 
	GPIOA->CRH|=0X000008B0;//IO状态设置
//	GPIOA->ODR|=1<<10;			//PA2 上拉
		
	RCC->APB2RSTR|=1<<14;   //复位串口1
	RCC->APB2RSTR&=~(1<<14);//停止复位	   	   
	//波特率设置
 	USART1->BRR=mantissa; // 波特率设置	 
	USART1->CR1|=0X200C;  //1位停止,无校验位.
#if EN_USART1_RX		  //如果使能了接收
	//使能接收中断 
	USART1->CR1|=1<<5;    //接收缓冲区非空中断使能	    	
	MY_NVIC_Init(8,0,USART1_IRQn,4);//组2，最低优先级 
//	USART1_RX_STA=0;
#endif
}
//初始化IO 串口2
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率 
void uart2_init(u32 pclk1,u32 bound)
{
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk1*1000000)/(bound*16);//得到USARTDIV
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分	 
    mantissa<<=4;
	mantissa+=fraction; 
	
	RCC->APB2ENR|=1<<2;   //使能PORTA口时钟  
	
	RCC->APB1ENR|=1<<17;  //使能串口时钟 
	GPIOA->CRL&=0XFFFF00FF;//IO状态设置
	GPIOA->CRL|=0X00008B00;//IO状态设置 
	GPIOA->ODR|=1<<3;			//PA2 上拉
	
	
	RCC->APB1RSTR|=1<<17;   //复位串口1
	RCC->APB1RSTR&=~(1<<17);//停止复位	   	   
	//波特率设置
 	USART2->BRR=mantissa; // 波特率设置	 
	USART2->CR1|=0X200C;  //1位停止,无校验位.
	
#if EN_USART2_RX		  //如果使能了接收
	//使能接收中断 
	USART2->CR1|=1<<5;    //接收缓冲区非空中断使能
//	USART2->CR3|=0X0001;  //错误中断
	MY_NVIC_Init(8,0,USART2_IRQn,4);//组2，最低优先级
//	USART2_RX_STA=0;
#endif
}

//初始化IO 串口3
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率 
void uart3_init(u32 pclk1,u32 bound)
{
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk1*1000000)/(bound*16);//得到USARTDIV
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分	 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=1<<3;   //使能PORTB口时钟  
	RCC->APB1ENR|=1<<18;  //使能串口时钟 
	GPIOB->CRH&=0XFFFF00FF;//IO状态设置
	GPIOB->CRH|=0X00008B00;//IO状态设置 
//	GPIOB->ODR|=1<<11;			//PA2 上拉
	RCC->APB1RSTR|=1<<18;   //复位串口1
	RCC->APB1RSTR&=~(1<<18);//停止复位	   	   
	//波特率设置
 	USART3->BRR=mantissa; // 波特率设置	 
	USART3->CR1|=0X200C;  //1位停止,无校验位.
#if EN_USART3_RX		  //如果使能了接收
	//使能接收中断 
	USART3->CR1|=1<<5;    //接收缓冲区非空中断使能	    	
	MY_NVIC_Init(8,0,USART3_IRQn,4);//组2，最低优先级 
//	USART3_RX_STA=0;
#endif
}



//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定向fputc函数
//printf的输出，指向fputc，由fputc输出到串口
//这里使用串口1(USART1)输出printf信息
int fputc(int ch, FILE *f)
{
#ifdef	PRINTEABLE
	while((USART1->SR&0X40)==0);//等待上一次串口数据发送完成  
	USART1->DR = (u8) ch;      	//写DR,串口1将发送数据
#endif
	return ch;
}
#endif 
//end
//////////////////////////////////////////////////////////////////



#if EN_USART1_RX  //如果使能了接收


//串口1中断服务程序 	
//u8 USART1_RX_BUF[USART_REC_LEN];	//接收缓冲,最大USART_REC_LEN个字节.
//u16 USART1_RX_STA=0;				//接收状态标记,bit15:接收完成标志,bit14~0:接收到的有效字节数	  
//extern xTimerHandle	Usart1RX_Timer_Handle;
//void Usart1RX_vTimerCallback(xTimerHandle xTimer){USART1_RX_STA |= 0x8000;} 
extern void Test_Receivebyte(uint8_t byte);
void USART1_IRQHandler(void)
{
	u8 ucTemp;	
	if(USART1->SR&(1<<5))//接收到数据
	{	 
		ucTemp = USART1->DR; 
		Test_Receivebyte(ucTemp);
	}
//	u8 ucTemp;	
//	if(USART1->SR&(1<<5))//接收到数据
//	{	 
//		ucTemp=USART1->DR; 
//		if((USART1_RX_STA&0x8000)==0)//接收未完成
//		{		
//			USART1_RX_BUF[USART1_RX_STA&0X7FFF]=ucTemp;
//			USART1_RX_STA++;
//			xTimerResetFromISR(Usart1RX_Timer_Handle,0);
//		}	
//	}
}
#endif	



#if EN_USART3_RX	//如果使能了接收
//串口2中断服务程序 	
u8 USART3_RX_BUF[1030];	//接收缓冲,最大USART_REC_LEN个字节.
u16 uart3write = 0;
u16 uart3read = 0;
void USART3_IRQHandler(void)
{
	u8 ucTemp;	
	if(USART3->SR&(1<<5))//接收到数据
	{	 
		ucTemp=USART3->DR; 
		USART3_RX_BUF[uart3write] = ucTemp;
		if(uart3write<1024)uart3write++;
		else uart3write = 0;
	}
}
//从缓冲区取出一字节数据
u8 uart3GetByte(u8 *Dat)
{
	if(uart3write == uart3read) return 0;//缓存空，没得东西
	else 
	{
		*Dat = USART3_RX_BUF[uart3read];
		if(uart3read<1024)uart3read++;
		else uart3read = 0;
		return 1;
	}
}

#endif	




#if EN_USART2_RX   //如果使能了接收
//串口3中断服务程序 
u16 uart2write = 0;
u16 uart2read = 0;
u8 USART2_RX_BUF[260];	//接收缓冲,最大USART_REC_LEN个字节.
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
	if(USART2->SR&(1<<5))//接收到数据
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
//从缓冲区取出一字节数据
u8 uart2GetByte(u8 *Dat)
{
	if(uart2write == uart2read) return 0;//缓存空，没得东西
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
		while((USART1->SR&0X40)==0);//等待上一次串口数据发送完成  
		USART1->DR = (u8) Dat[i+1];	//写DR,串口1将发送数据
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
		while((USART2->SR&0X40)==0);//等待上一次串口数据发送完成  
		USART2->DR = (u8) Dat[i+1];	//写DR,串口1将发送数据
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
		while((USART3->SR&0X40)==0);//等待上一次串口数据发送完成  
		USART3->DR = (u8) Dat[i+1];	//写DR,串口1将发送数据
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






