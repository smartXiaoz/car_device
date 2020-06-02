/*
 ******************************************************************
 *
 * 功能：对CAN驱动的接口封装
 * 描述;
 * 作者：monli
 * 版本： V1.3
 *
 ******************************************************************
 */
#include "uds_can_driver.h"
#include "CanTp/CanTp.h"
#include "Interface/uds_cfg.h"


void CanIf_Init(void)
{  

}

void CanIf_Received(Can_SF	can_temp)
{
    uint8 index;
    uint32 id;
    CanFrame Uptmp;

		id =can_temp.id;
		
		if ((id != FUNCTIONAL_ADDRESS) && (id != PHYSICAL_ADDRESS))
		{
			return;
		}
		
    Uptmp.ID = id;
    Uptmp.DataLen = 8;
    for (index = 0; index < Uptmp.DataLen; index++)
    {
        Uptmp.Data[index] = can_temp.data[index];
    }
    CanTp_Process(&Uptmp);
}

void CanIf_TxIndication()
{
    CanTp_TxConfirmation();  
}

void CanIf_TxConfirm()
{
}

uint8 CanIf_Transmit(CanFrame *frame)
{
		uint8 index;
	
		Can_SF	can_temp;

		if (frame == NULL)
				return 0;

		can_temp.id = frame->ID;
		
		for (index = 0; index < frame->DataLen; index++)
		{
			can_temp.data[index] = frame->Data[index];
		}

		for (index = frame->DataLen; index < 8; index++)
		{
			can_temp.data[index] = 0x55;
		}
		Can_Write_TXMsg(can_temp,0);
		return 0;
}
