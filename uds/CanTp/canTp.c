/*
******************************************************************
* 
* 
*
******************************************************************
*/
#include "CanTp/CanTp.h"
#include "Interface/uds_cfg.h"
#include "Det/det.h"
#include "Pdu/pdu.h"
#include "Interface/uds_interface.h"

CANTP tp;

#define FUNC_CANTP_INIT           1
#define FUNC_CANTP_TRANSMIT       2
#define FUNC_CanTp_MainTask   		3
#define FUNC_CANTP_PROCESS        4
#define FUNC_CANTP_TXCONFIRM      5    



#define ERROR_FC_TIMEOUT          1
#define ERROR_CF_TIMEOUT          2    
#define ERROR_FRAME_LEN           3
#define ERROR_TP_LEN              4  
#define ERROR_BUSY                5 
#define ERROR_SEQUENCE            6 
#define ERROR_STATUS              7 
#define ERROR_ARGUMENT            8 
#define ERROR_CAN_ID              9
#define ERROR_STMIN_TIMEOUT       10
/*********************************************************
* void CANTP_Init()
*********************************************************/
void CanTp_Init()
{
    tp.Status=TP_UDS_IDLE;
    tp.CF_TxBS_Num = 0;
    tp.TxTimeStamp = 0;

    tp.RxTimeStamp=0;
    tp.CF_RxSequence= 0;
    tp.CF_RxBS_Num=0;

    tp.txPdu=NULL;
    tp.rxPdu=NULL;  
}
/*********************************************************
* void CANTP_Transmit(uint8 pduid, PduInfoType* pduinfo)
*********************************************************/
void CanTp_Transmit(uint8 id, PduInfoType* pduinfo)
{
	id = id;
    id = 0;
    tp.txPdu = pduinfo;  
    tp.Status = TP_TRANSMIT_FF;  //???
    CanIf_TxIndication();
}


/*********************************************************
* void CanTp_MainTask()
*********************************************************/
void CanTp_MainTask(void)
{
    uint32 current= Uds_GetTime();
    if (tp.Status == TP_RECEIVING_FC)
    {
        if ((current > 0) && (current > tp.TxTimeStamp) && ((current- tp.TxTimeStamp) > FC_TIMEOUT))
        {
            tp.Status = TP_UDS_IDLE;
            tp.txPdu = NULL;
            Uds_CanTpTxConfirmation(0,NTFRSLT_OK); 
            Det_Report(MODULE_CANTP,FUNC_CanTp_MainTask,ERROR_FC_TIMEOUT,0);
        }
    }

    if (tp.Status == TP_RECEIVING_CF)
    {
        if ((current > 0) && (current > tp.RxTimeStamp) && ((current- tp.RxTimeStamp) > CF_TIMEOUT))
        {
            tp.Status = TP_UDS_IDLE;
            tp.txPdu = NULL;
            Uds_CanTpTxConfirmation(0,NTFRSLT_OK); 
            Det_Report(MODULE_CANTP,FUNC_CanTp_MainTask,ERROR_CF_TIMEOUT,0);
        }  
    }

    CanTp_TxConfirmation();
}


/*********************************************************
*********************************************************/
void CanTp_Process(CanFrame *rxFrame)
{
    uint32   len=0,RxSequence=0,count;
    uint8   framekind=0,i=0,FS,remain;
    PduInfoType *pBuffer = NULL;
   
    len=rxFrame->DataLen;
    if(len<=1)
    {
	    Det_Report(MODULE_CANTP,FUNC_CANTP_PROCESS,ERROR_FRAME_LEN,SF_N_PDU);
        return;
    }
            
    framekind=rxFrame->Data[0]>> 4;//
    if(rxFrame->Data[0] == 0x21)
    {
        rxFrame->Data[0]  = 0x21;
    }

    switch(framekind)
    {
        case SF_N_PDU: //SingleFrame

            if(rxFrame->Data[1] == 0x3E && rxFrame->Data[2] == 0x80)
            {
            	uds.TimeStamp = Uds_GetTime(); //
            	return;
            }

            if (rxFrame->DataLen < 2)  
            {
                Det_Report(MODULE_CANTP,FUNC_CANTP_PROCESS,ERROR_FRAME_LEN,SF_N_PDU);
                return;
            }

            len=rxFrame->Data[0]&0x0f;
            if (len == 0 || len > 7 || len > rxFrame->DataLen - 1)      
            {
                Det_Report(MODULE_CANTP,FUNC_CANTP_PROCESS,ERROR_TP_LEN,SF_N_PDU);
                return; 
            }

            if(Uds_CanTpProvideRxBuffer(rxFrame->ID,(PduLengthType)len,&pBuffer)==BUFREQ_OK)
            {

                for(i=1;i<=len;i++)
                {
                    pBuffer->SduDataPtr[i-1]=rxFrame->Data[i];  
                }

                tp.PduID = rxFrame->ID;
                tp.Status = TP_UDS_IDLE;
                tp.txPdu = NULL;
								tp.RxTimeStamp=Uds_GetTime(); 
                Uds_CanTpRxIndication(rxFrame->ID, NTFRSLT_OK);                
            }
            else
            {
               Det_Report(MODULE_CANTP,FUNC_CANTP_PROCESS,ERROR_BUSY,SF_N_PDU);
            }
            break;

        case FF_N_PDU: //FirstFrame
            if (rxFrame->DataLen != 8)      //??????
            { 
                Det_Report(MODULE_CANTP,FUNC_CANTP_PROCESS,ERROR_FRAME_LEN,FF_N_PDU);
                return;
            }

            len=((uint32)(rxFrame->Data[0]&0x0f)<<8)+(uint32)rxFrame->Data[1];  //12???????   //TestCase ID-TP33
            if (len <= 7)  //??????
            {
                Det_Report(MODULE_CANTP,FUNC_CANTP_PROCESS,ERROR_TP_LEN,FF_N_PDU);
                return;
            }
                      
            if(Uds_CanTpProvideRxBuffer(rxFrame->ID,(PduLengthType)len,&pBuffer)==BUFREQ_OK)
            {
                tp.rxPdu = pBuffer;
                for(i=2;i<8;i++) 
                {
                    pBuffer->SduDataPtr[i-2]=rxFrame->Data[i];  
                }
                pBuffer->Counter=6;          
                tp.CF_RxBS_Num = 0; 
                tp.PduID = rxFrame->ID;
                tp.CF_RxSequence = 0;
                tp.RxTimeStamp=Uds_GetTime(); 
                tp.txPdu = NULL;                              
                tp.Status = TP_TRANSMIT_FC;      
                CanIf_TxIndication();
            }
            else 
            {
                Det_Report(MODULE_CANTP,FUNC_CANTP_PROCESS,ERROR_BUSY,FF_N_PDU);
            }
            break;

        case CF_N_PDU: //ConsecutiveFrame???????

            if (rxFrame->DataLen < 2) // ??????
            {
                Det_Report(MODULE_CANTP,FUNC_CANTP_PROCESS,ERROR_FRAME_LEN,CF_N_PDU);
                return;
            }

            if (rxFrame->ID != tp.PduID)  //?????ID
            {
                Det_Report(MODULE_CANTP,FUNC_CANTP_PROCESS,ERROR_CAN_ID,CF_N_PDU);
                return;
            }
            
            
            if (tp.Status != TP_RECEIVING_CF)
            {
                Det_Report(MODULE_CANTP,FUNC_CANTP_PROCESS,ERROR_STATUS,CF_N_PDU);
                return;  // û????????
            }

            RxSequence=rxFrame->Data[0]&0x0F;//????? ,???????
            if (((tp.CF_RxSequence + 1) & 0xF) != RxSequence)  //????????
            {
                Det_Report(MODULE_CANTP,FUNC_CANTP_PROCESS,ERROR_SEQUENCE,CF_N_PDU);
                tp.Status = TP_UDS_IDLE;
                return;
            }  
            tp.CF_RxSequence = RxSequence;
            tp.RxTimeStamp= Uds_GetTime();

            pBuffer = tp.rxPdu;
            count =pBuffer->Counter;
            if(pBuffer != NULL && RxSequence ==(((count)/7  + 1)& 0x0f))
            {
                if (pBuffer->SduLength > pBuffer->Counter)
                {
                    remain = pBuffer->SduLength - pBuffer->Counter;
                    if (remain >=7  && rxFrame->DataLen != 8)           //TestCase ID-TP34
                    {
                        Det_Report(MODULE_CANTP,FUNC_CANTP_PROCESS,ERROR_TP_LEN,CF_N_PDU);
                        tp.Status = TP_UDS_IDLE;
                        return;
                    }
                    else if (remain < 7 && rxFrame->DataLen < (remain + 1))          //TestCase ID-TP34
                    {
                        Det_Report(MODULE_CANTP,FUNC_CANTP_PROCESS,ERROR_TP_LEN,CF_N_PDU);
                        tp.Status = TP_UDS_IDLE;
                        return;
                    }
                }
                tp.CF_RxBS_Num++;
                for(i=1;(i<len) && (pBuffer->Counter<pBuffer->SduLength);i++)
                {
                    pBuffer->SduDataPtr[pBuffer->Counter]=rxFrame->Data[i];
                    pBuffer->Counter++;  
                }

                if(pBuffer->Counter>=pBuffer->SduLength) //???????
                {
                    tp.Status = TP_UDS_IDLE;
                    Uds_CanTpRxIndication(rxFrame->ID, NTFRSLT_OK);
                }
#if CANTP_FC_PEFRAME == STD_ON                
                else
                {
                    if (tp.CF_RxBS_Num == CANTP_RX_FC_PEFRAME)
                    {
                        tp.Status = TP_TRANSMIT_FC;
                        CanIf_TxIndication();
                    }  
                }
#endif
            }         
            break;   
        
        case FC_N_PDU://?????
            if (rxFrame->DataLen < 3)           // ??????
            {
                Det_Report(MODULE_CANTP,FUNC_CANTP_PROCESS,ERROR_FRAME_LEN,FC_N_PDU);
                return;
            }
                     
            FS = rxFrame->Data[0] & 0xF;  
            if (FS > 2)    //??????
            {
                Det_Report(MODULE_CANTP,FUNC_CANTP_PROCESS,ERROR_ARGUMENT,FC_N_PDU);
                return;
            }

            if (tp.PduID != rxFrame->ID)
            {
                tp.Status = TP_UDS_IDLE; 
                tp.txPdu = NULL;
                Uds_CanTpTxConfirmation(0,NTFRSLT_OK);
                Det_Report(MODULE_CANTP,FUNC_CANTP_PROCESS,ERROR_CAN_ID,FC_N_PDU);
                return;
            }
            
            if (FS == 2)   //???,??????
            {
                tp.Status = TP_UDS_IDLE;                                    
                tp.txPdu = NULL;
                Uds_CanTpTxConfirmation(0,NTFRSLT_OK);
                Det_Report(MODULE_CANTP,FUNC_CANTP_PROCESS,ERROR_ARGUMENT,FC_N_PDU);
                return;
            } 
            
            if (tp.txPdu == NULL)              // û?????????????
            {
                Det_Report(MODULE_CANTP,FUNC_CANTP_PROCESS,ERROR_STATUS,FC_N_PDU);
                return;
            }            
             
            tp.RxTimeStamp = Uds_GetTime();   
            
            if (FS == 1)   //???
            {
                Det_Report(MODULE_CANTP,FUNC_CANTP_PROCESS,ERROR_ARGUMENT,FC_N_PDU);
                return;
            }
                                         
            tp.CF_TxBS_Num = rxFrame->Data[1];  
            if (tp.CF_TxBS_Num == 0)
            {
                tp.CF_TxBS_Num = 0xFF;
            }
            tp.STmin = rxFrame->Data[2];
            tp.Status = TP_TRANSMIT_CF;   
            CanIf_TxIndication();                                           
            break;
    }  
}


void CanTp_TxConfirmation(void)
{
    uint8 i,Dlc;
    PduInfoType *pBuffer = NULL;
    CanFrame txFrame;
    uint32 current;
	current = current;

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
    
    for(Dlc = 0; Dlc<8; Dlc++)
    {
    	txFrame.Data[Dlc]= 0x55;
    }

    if(tp.Status == TP_TRANSMIT_FC )
    {
        txFrame.Data[0]=0x30;
        txFrame.Data[1]=CANTP_RX_FC_PEFRAME;
        txFrame.Data[2]=CANTP_RX_FC_STMIN;
        txFrame.DataLen=3;
        CanIf_Transmit(&txFrame);
        CanIf_TxConfirm(); //???????
        tp.CF_RxBS_Num = 0;
        tp.Status = TP_RECEIVING_CF;
    }
     
    pBuffer= tp.txPdu;
    if (pBuffer == NULL)
    {
        return;
    }

    if(tp.Status == TP_TRANSMIT_FF )
    {
        if(pBuffer->SduLength<=7)//????7??????
        {
            //??????
            txFrame.Data[0]=(uint8)pBuffer->SduLength;
            for(i=0;i<pBuffer->SduLength;i++)
            {
                txFrame.Data[i+1]=pBuffer->SduDataPtr[i];
            }
            txFrame.DataLen=(uint8)(pBuffer->SduLength+1);             
            CanIf_Transmit(&txFrame);   
            CanIf_TxConfirm(); //???????
            Uds_CanTpTxConfirmation(0,NTFRSLT_OK);         
            tp.Status = TP_UDS_IDLE;          
        }  
        else
        {
            //??????
            pBuffer->Counter=6;
//			txFrame.Data[0]=0x10 | (pBuffer->SduLength >> 8) & 0xF;
            txFrame.Data[0]=0x10 | ((pBuffer->SduLength >> 8) & 0xF);
            txFrame.Data[1]=(uint8)pBuffer->SduLength;
            for(i=0;i<6;i++)
            {
                txFrame.Data[i+2]=pBuffer->SduDataPtr[i];
            }
            txFrame.DataLen=8;                           
            CanIf_Transmit(&txFrame);   
            CanIf_TxConfirm();    
            tp.Status = TP_RECEIVING_FC; 
            tp.CF_TxBS_Num = 0;                 
            tp.TxTimeStamp = Uds_GetTime();  
        }    
    }
    
    
    if(tp.Status == TP_TRANSMIT_CF )
    {
        current = Uds_GetTime();
//        if (current < tp.TxTimeStamp  ||  (current - tp.TxTimeStamp) < tp.STmin)     // ?????????
//        {
//		    Det_Report(MODULE_CANTP,FUNC_CANTP_TXCONFIRM,ERROR_STMIN_TIMEOUT,0);
//            return;
//        }

		if (tp.CF_TxBS_Num == 0)
		{
		    Det_Report(MODULE_CANTP,FUNC_CANTP_TXCONFIRM,ERROR_ARGUMENT,0);
            return;		
		}

        if((tp.CF_TxBS_Num > 0)&&(pBuffer->Counter < pBuffer->SduLength))
        {              
            txFrame.Data[0]=(uint8)((CF_N_PDU<<4)+ ((pBuffer->Counter/7 + 1)&0x0f));
            for(i=0;(i<7)&&(pBuffer->Counter<pBuffer->SduLength);i++,pBuffer->Counter++)
            {
                txFrame.Data[i + 1]=pBuffer->SduDataPtr[pBuffer->Counter];
            }
            txFrame.DataLen=i + 1;    
            CanIf_Transmit(&txFrame);     

            tp.CF_TxBS_Num--;            
            tp.TxTimeStamp = Uds_GetTime();    
            if(tp.CF_TxBS_Num == 0)
            {
                CanIf_TxConfirm();
                tp.Status = TP_RECEIVING_FC; 
            }                
        }        


        if(pBuffer->Counter==pBuffer->SduLength) //???????
        {  
            Uds_CanTpTxConfirmation(0,NTFRSLT_OK);                   
            CanIf_TxConfirm();  
            tp.txPdu = NULL;    
            tp.Status = TP_UDS_IDLE;
        }        
    }
}
