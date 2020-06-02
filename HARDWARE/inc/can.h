#ifndef __CAN_H
#define __CAN_H	 
#include "sys.h"	    								    


#define BUSSOFFTEST		1






#define	CAN_LEN		32			//CAN缓存长度

typedef struct{
 u32 id;
 u8  data[8];
} Can_SF;


typedef struct{
 u8 writpointer;
 u8 readpointer;
 Can_SF frame[CAN_LEN];
} CAN_CACHE;

u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode);//CAN初始化

u8 CAN_Send_Msg(Can_SF TX_Msg);		//发送数据
u8 Can_Get_RXMsg(Can_SF *RX_Msg);	//从缓存区取数据
void Can_Write_TXMsg(Can_SF TX_Msg,u8 Priority);//向发送缓冲区写入一帧报文
u8 Can_Get_TXMsg(Can_SF *TX_Msg);//从发送缓冲区取出一帧报文

void CAN_RXclean(void);//清空接收缓存
void CAN_SENGSTOP(void);

#endif

















