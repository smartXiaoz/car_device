#ifndef __CAN_H
#define __CAN_H	 
#include "sys.h"	    								    


#define BUSSOFFTEST		1






#define	CAN_LEN		32			//CAN���泤��

typedef struct{
 u32 id;
 u8  data[8];
} Can_SF;


typedef struct{
 u8 writpointer;
 u8 readpointer;
 Can_SF frame[CAN_LEN];
} CAN_CACHE;

u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);//CAN��ʼ��

u8 CAN_Send_Msg(Can_SF TX_Msg);		//��������
u8 Can_Get_RXMsg(Can_SF *RX_Msg);	//�ӻ�����ȡ����
void Can_Write_TXMsg(Can_SF TX_Msg,u8 Priority);//���ͻ�����д��һ֡����
u8 Can_Get_TXMsg(Can_SF *TX_Msg);//�ӷ��ͻ�����ȡ��һ֡����

void CAN_RXclean(void);//��ս��ջ���
void CAN_SENGSTOP(void);

#endif

















