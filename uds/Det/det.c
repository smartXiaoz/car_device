#include "Det/det.h"
#include "CanIf/uds_can_driver.h"
#include "Interface/uds_cfg.h"

uint8 det_report_status=1;

void Det_Report(uint8 ModuleID,uint8 FuncId,uint8 Code,uint8 Option) 
{
    CanFrame txFrame;
    if (det_report_status)
    {
        txFrame.ID = CANTP_TX_ID;
        if ((CANTP_TX_ID & 0x80000000) != 0)
        {    
            txFrame.ExtendedFlag = 1;
        }
        else
        {      
            txFrame.ExtendedFlag = 0;  
        }
        txFrame.RemoteFlag = 0;
        txFrame.Data[0]=ModuleID;
        txFrame.Data[1]=FuncId;
        txFrame.Data[2]=Code;    
		    txFrame.Data[3]=Option; 
        txFrame.DataLen=4;          
        CanIf_Transmit(&txFrame); 
    }
}
