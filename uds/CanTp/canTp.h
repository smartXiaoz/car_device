/*
******************************************************************
*ÃèÊö£ºCANTP²ã
*×÷Õß£ºmonli
*°æ±¾£ºV1.0
*
******************************************************************
*/
#ifndef _CANTP_H_
#define _CANTP_H_

#include "common/Platform_Types.h"
#include "CanIf/uds_can_driver.h"

#define SF_N_PDU 0x00 //SingleFrame
#define FF_N_PDU 0x01 //FirstFrame
#define CF_N_PDU 0x02 //ConsecutiveFrame
#define FC_N_PDU 0x03 //FlowControl

#define FC_N_CTS   0x04



typedef enum
{
    NTFRSLT_OK=  0x00,
    NTFRSLT_E_NOT_OK=0x01,
    NTFRSLT_E_TIMEOUT_A=0x02,
    NTFRSLT_E_TIMEOUT_Bs=0x03,
    NTFRSLT_E_TIMEOUT_Cr=0x04,
    NTFRSLT_E_WRONG_SN=0x05,
    NTFRSLT_E_INVALID_FS=0x06,
    NTFRSLT_E_UNEXP_PDU=0x07,
    NTFRSLT_E_WFT_OVRN=0x08,
    NTFRSLT_E_NO_BUFFER=0x09,
    NTFRSLT_E_CANCELLATION_OK=0x0A,
    NTFRSLT_E_CANCELATION_NOT_OK=0x0B
}NotifResultType;

typedef uint32 PduIdType;

typedef uint16 PduLengthType;


typedef struct
{
    uint8           *SduDataPtr;
    PduLengthType   SduLength;
    PduLengthType   Counter;  
}PduInfoType;

typedef enum
{
    BUFREQ_OK = 0,
    BUFREQ_E_NOT_OK,
    BUFREQ_E_BUSY,
    BUFREQ_E_OVFL
}BufReq_ReturnType;


typedef enum
{
    TP_UDS_IDLE,   
 
    TP_TRANSMIT_FF,   
    TP_TRANSMIT_CF,  
    TP_TRANSMIT_FC,   

    TP_RECEIVING_CF,
    TP_RECEIVING_FC  
}TPStatus;


typedef struct
{
    TPStatus Status;
    PduInfoType* txPdu;
    PduInfoType* rxPdu;
    
    uint32   CF_RxBS_Num;  
    uint8    CF_RxSequence;

    uint32   RxTimeStamp;
    uint32   TxTimeStamp;
    uint32   PduID;
    uint8    STmin;
    uint32   CF_TxBS_Num;
}CANTP;



extern CANTP tp;

void CanTp_Init(void);

void CanTp_Transmit(uint8 PduId, PduInfoType* pduinfo);

void CanTp_TxConfirmation(void);

void CanTp_Process(CanFrame *rxFrame);

void CanTp_MainTask(void);


void Uds_CanTpRxIndication(PduIdType CanTpRxPduId,NotifResultType Result);

BufReq_ReturnType Uds_CanTpProvideRxBuffer(PduIdType CanTpRxPduId, PduLengthType Length,PduInfoType** RxBufPtr);

void Uds_CanTpTxConfirmation(PduIdType CanTpRxPduId, NotifResultType Result);



#endif
