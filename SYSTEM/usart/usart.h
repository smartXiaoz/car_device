#ifndef __USART_H
#define __USART_H
#include "sys.h"
#include "stdio.h"	 

//#define U3DATALOG		//MPU
//#define U2DATALOG		//����
#define PRINTEABLE		//printf����


//#define USART_REC_LEN  		256		//�����������ֽ��� 200
#define EN_USART1_RX 		1			//ʹ�ܣ�1��/��ֹ��0������1����
#define EN_USART2_RX 		1			//ʹ�ܣ�1��/��ֹ��0������2����
#define EN_USART3_RX 		1			//ʹ�ܣ�1��/��ֹ��0������3����

//extern u8  USART1_RX_BUF[USART_REC_LEN];	//���ջ���,���USART_REC_LEN���ֽ�
// 
//extern u16 USART1_RX_STA;					//����״̬���	

//����봮���жϽ��գ��벻Ҫע�����º궨��
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
















