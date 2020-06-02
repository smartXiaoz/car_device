/*
******************************************************************
*
* 功能：对CAN驱动的接口封装头文件
* 描述;
* 作者：monli
* 版本： V1.1
*
******************************************************************
*/
#ifndef _CAN_DRIVER_H_
#define _CAN_DRIVER_H_

#include "common/Platform_Types.h"

typedef struct
{
    uint32     ID;             // 报文ID。
    uint8      RemoteFlag;
    uint8      ExtendedFlag;   // 是否是扩展帧。
    uint8      DataLen;        // Transmit Data Length (<=8)
    uint8      Data[8];        // Recieve Data Buffer
}CanFrame;


void CanInit(void);

uint8 CanIf_Transmit(CanFrame* frame);

void CanIf_Received(Can_SF	can_temp);
void CanIf_TxIndication(void);

void CanIf_TxConfirm(void);


#endif
