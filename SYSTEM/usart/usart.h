#ifndef __USART_H
#define __USART_H
#include "sys.h"
#include "stdio.h"	 

//#define U3DATALOG		//MPU
//#define U2DATALOG		//环视
#define PRINTEABLE		//printf启用


//#define USART_REC_LEN  		256		//定义最大接收字节数 200
#define EN_USART1_RX 		1			//使能（1）/禁止（0）串口1接收
#define EN_USART2_RX 		1			//使能（1）/禁止（0）串口2接收
#define EN_USART3_RX 		1			//使能（1）/禁止（0）串口3接收

//extern u8  USART1_RX_BUF[USART_REC_LEN];	//接收缓冲,最大USART_REC_LEN个字节
// 
//extern u16 USART1_RX_STA;					//接收状态标记	

//如果想串口中断接收，请不要注释以下宏定义
void uart1_init(u32 pclk2,u32 bound);
void uart2_init(u32 pclk2,u32 bound);
void uart3_init(u32 pclk2,u32 bound);

u8 Uart1_TxBuff(u8 *Dat);
u8 Uart2_TxBuff(u8 *Dat);
u8 Uart3_TxBuff(u8 *Dat);

u8 uart1GetByte(u8 *Dat);
u8 uart2GetByte(u8 *Dat);
u8 uart3GetByte(u8 *Dat);

#endif	   
















