/*
******************************************************************
*
* monli
*
******************************************************************
*/
#include "Pdu/pdu.h"
#include "Interface/uds_interface.h"
#include "CanIf/uds_can_driver.h"
#include "Det/det.h"



extern const UdsConfig UDS;
#define SID   BufferSpaceRx[0]


#define UDS_GET_SERVICE_TABLE                     1
#define UDS_CHECKTP_TIMEOUT                       2



#define ERROR_UDS_SERVICETABLE_NULL               1
#define ERROR_UDS_TPTIMEOUT                       2



Uds uds;

/*****************************
*本地全局变量
******************************/
static uint8 BufferSpaceRx[UDS_RX_BUFFER_SIZE]; //缓存空间
static uint8 BufferSpaceTx[UDS_TX_BUFFER_SIZE]; //缓存空间
static PduInfoType  RpduInfo;
static PduInfoType  TpduInfo;



/*****************************
*本地函数定义
*****************************/  
static Std_ReturnType DslInternal_Forward2Dsd(PduIdType RxBufferID);
static Std_ReturnType UdsInternal_Forward2Dsp(PduIdType RxBufferID);
static Std_ReturnType UdsInternal_Process(PduIdType RxBufferID);
static void UdsInternal_ProcessingDone(PduLengthType Length);
static uint8 UdsInternal_IsSidValid(PduIdType RxBufferID, uint8 sid);//huanglei
static uint8 UdsInternal_IsSidSecurityValid(PduIdType RxBufferID,uint8 sid);
static void Uds_ErrorHandle(uint8 sid,uint8 NRC_,PduIdType RxBufferID);


static PduInfoType* GetRxBuffer()
{
    return &RpduInfo;
}

static PduInfoType* GetTxBuffer()
{
    return &TpduInfo;
}



static void InitRxBuffer()
{
    RpduInfo.SduDataPtr =  BufferSpaceRx;
    RpduInfo.SduLength  = 0;
    RpduInfo.Counter = 0;
}

static void InitTxBuffer()
{
    TpduInfo.SduDataPtr =  BufferSpaceTx;
    TpduInfo.SduLength  = 0;
    TpduInfo.Counter = 0;
}

static void Buffer_Init()
{
    uint32 j;  
    for (j = 0; j < UDS_RX_BUFFER_SIZE; j++)
    {
        BufferSpaceRx[j] = 0; 
    }   
    for (j = 0; j < UDS_TX_BUFFER_SIZE; j++)
    {
        BufferSpaceTx[j] = 0; 
    }  
    InitRxBuffer();
    InitTxBuffer();
}





const  UdsServiceTable* Uds_GetServiceTable(PduIdType RxBufferID)
{
    uint8 i;
    const UdsServiceTable * table;
    for(i=0;i < UDS._Dsd.ServiceTableNum;i++)
    {
        if (UDS._Dsd.ServiceTables[i].Id == RxBufferID)
        {
            table =  &(UDS._Dsd.ServiceTables[i]);  
            return table;
        }
    }
    Det_Report(MODULE_UDS,UDS_GET_SERVICE_TABLE,ERROR_UDS_SERVICETABLE_NULL,0);    
    return NULL;
}

Std_ReturnType UdsInternal_CheckTpTimeout(PduIdType RxBufferID)
{
    uint32 current = Uds_GetTime();
    if (uds.TpTimeStamp >  0 && current >= uds.TpTimeStamp &&  current - uds.TpTimeStamp > TP_TIMEOUT)
    {
        uds.SecurityLevel=0;
        uds.SessionLevel=1;
        uds.State = UDS_IDLE;
        Det_Report(MODULE_UDS,UDS_CHECKTP_TIMEOUT,ERROR_UDS_TPTIMEOUT,0);   
    }
    uds.TpTimeStamp = current;
    return E_OK;    
}


BufReq_ReturnType Uds_CanTpProvideRxBuffer(PduIdType CanTpRxPduId, PduLengthType Length,PduInfoType** RxBufPtr)
{
	CanTpRxPduId = CanTpRxPduId;
    CanTpRxPduId = 0;
	
    //UdsInternal_CheckTpTimeout(CanTpRxPduId);
    if (uds.State == UDS_IDLE)
    { 
        (*RxBufPtr) = GetRxBuffer();
        (*RxBufPtr)->SduLength = Length;
        return BUFREQ_OK;
    }
    else if (uds.State == UDS_BUSY)
    {
        return BUFREQ_E_BUSY;
    }
    else
    {
        return BUFREQ_E_NOT_OK;
    }
}




BufReq_ReturnType Uds_CanTpProvideTxBuffer(PduIdType CanTpRxPduId, PduInfoType** TxBufPtr, PduLengthType Length)
{
	CanTpRxPduId = CanTpRxPduId;
    CanTpRxPduId = 0;
    if (uds.State == UDS_IDLE)
    { 
        (*TxBufPtr) = GetTxBuffer();
        (*TxBufPtr)->SduLength = Length;
        return BUFREQ_OK;
    }
    else if (uds.State == UDS_BUSY)
    {
        return BUFREQ_E_BUSY;
    }
    else
    {
        return BUFREQ_E_NOT_OK;
    }
}


void Uds_CanTpRxIndication(PduIdType DcmRxPduID, NotifResultType Result)
{
    if (uds.State == UDS_IDLE)
    {
        uds.ID =  DcmRxPduID;
        uds.State = UDS_BUSY;   
#if UDS_USE_MAINFUNCTION == STD_OFF
        DslInternal_Forward2Dsd(DcmRxPduID);
#endif
    }
    return;  
}


void Uds_MainTask(void)
{
    Std_ReturnType ret;
	ret = ret;
    if (uds.State == UDS_BUSY)
    {
        uds.State = UDS_WAITING;
        ret = DslInternal_Forward2Dsd(uds.ID);      
    }
}



void Uds_CanTpTxConfirmation(PduIdType DcmTxPduId, NotifResultType Result)
{
	DcmTxPduId = DcmTxPduId;
    DcmTxPduId = 0;
	
    if (Result == NTFRSLT_OK)
    {
        uds.State = UDS_IDLE;
    }
    return;
}



void Uds_Init(void)
{
    Buffer_Init();
    memset((void*)&uds,0,sizeof(Uds));
    uds.SecurityLevel=0;
    uds.SessionLevel=1;
    uds.State = UDS_IDLE;
    uds.TransferState=NONE;
    uds.SeedValid= FALSE;
    uds.Active= FALSE;
    uds.Reboot =FALSE;
    uds.TimeStamp = 0;
    uds.Tick=0;
}


uint8 UdsInternal_IsSidValid(PduIdType RxBufferID,uint8 sid)
{
    uint8 i=0;
    const UdsServiceTable *ptrServiceTable = Uds_GetServiceTable(RxBufferID);    
    if (ptrServiceTable == NULL)
    {
        return FALSE;
    }  
    
    for (i = 0; i < ptrServiceTable->ServiceNum; i++)
    {
        if (ptrServiceTable->Services[i].ServiceId == sid)
        {
            return TRUE;
        }
    }
    return FALSE;
}


uint8 UdsInternal_IsSidSecurityValid(PduIdType RxBufferID,uint8 sid)
{
    uint8 i=0,j=0;
    const SecurityRow *securityRow;
    const UdsServiceTable *ptrServiceTable = Uds_GetServiceTable(RxBufferID);    
    const UdsService      *service;
    PduInfoType *pRxBufPtr =  GetRxBuffer();
    uint16 subFunc; 
    if (ptrServiceTable == NULL)
    {
        return FALSE;
    } 
    for (i = 0; i < ptrServiceTable->ServiceNum; i++)
    {
        service = &ptrServiceTable->Services[i];
        if (service->ServiceId == sid)
        {    
            if (service->Subfunc != NULL)
            {
                if (sid == 0x2E)
                {
                    if(pRxBufPtr->SduLength < 3)
                    {
                  //      return TRUE;
                    }
                    subFunc = (((uint16)pRxBufPtr->SduDataPtr[1] << 8) & 0xFF00) | (((uint16)pRxBufPtr->SduDataPtr[2])  & 0xFF);
                    if((uds.SecurityLevel == 3) &&(subFunc != 0xf10a))
					{
						 return TRUE;
					}
                }
                if (sid == 0x31)
                {
                    if(pRxBufPtr->SduLength < 4)
                    {
                  //      return TRUE;
                    }
                    if(uds.SecurityLevel == 1)
					   {
						 return TRUE;
					   }
                    subFunc = (((uint16)pRxBufPtr->SduDataPtr[2] << 8) & 0xFF00) | (((uint16)pRxBufPtr->SduDataPtr[3])  & 0xFF);
                }
            }
            
            if (service->Subfunc == NULL || service->Subfunc == subFunc)
            {
                securityRow = service->SecurityLevelRef;    
                if(securityRow==NULL)
                { 
                    return TRUE;
                }
                
                for(j=0;securityRow[j].SecurityLevel!=0;j++)
                {
                    //如果请求服务的安全等级与当前安全等级(初始值为0)相同，则可以请求该服务
                    if(securityRow[j].SecurityLevel==uds.SecurityLevel)
                    { 
                        return TRUE; 
                    }
                }
            }
        }
    }
    return FALSE;
}

uint8 UdsInternal_IsSidSessionValid(PduIdType RxBufferID,uint8 sid)
{
    uint8 i=0,j=0;
    const SessionRow *sessionRow;
    const UdsServiceTable *ptrServiceTable = Uds_GetServiceTable(RxBufferID);
    
    if (ptrServiceTable == NULL)
    {
        return FALSE;
    }
  
    for (i = 0; i < ptrServiceTable->ServiceNum; i++)
    {
        if (ptrServiceTable->Services[i].ServiceId == sid)
        {
            sessionRow=ptrServiceTable->Services[i].SessionLevelRef;
       
            if(sessionRow==NULL)//如果没有对请求的服务进行会话设置，则可直接请求该服务
            {   
                return TRUE;
            }
       
            for(j=0;sessionRow[j].SessionLevel!=0;j++) 
            {              
                //如果请求服务的会话等级与当前会话等级(初始值为1)相同，则可以请求该服务
                if(sessionRow[j].SessionLevel==uds.SessionLevel)
                {
                    return TRUE;
                } 
            }
            return FALSE;
        }
    }
    return FALSE;
}

Std_ReturnType DslInternal_Forward2Dsd(PduIdType RxBufferID)
{
    if (UdsInternal_IsSidValid(RxBufferID,SID) == TRUE)
    {
        return UdsInternal_Forward2Dsp(RxBufferID);
    }
    else  //不支持请求的服务
    {
        Uds_ErrorHandle(SID,Uds_E_RequestServiceNotSupported,RxBufferID);
        return E_NOT_OK;
    }
}

Std_ReturnType UdsInternal_CheckSessionTimeout(PduIdType RxBufferID)
{
    uint32 current = Uds_GetTime();
    if (uds.TimeStamp >  0 && current >= uds.TimeStamp &&  current - uds.TimeStamp > SESSION_TIMEOUT)
    {
        uds.SecurityLevel=0;
        uds.SessionLevel=1;
    }
    uds.TimeStamp = current;
    return E_OK;    
}

Std_ReturnType UdsInternal_Forward2Dsp(PduIdType RxBufferID)
{
    UdsInternal_CheckSessionTimeout(RxBufferID);
    
    /*检测服务会话设置*/
    if(UdsInternal_IsSidSessionValid(RxBufferID,SID)==FALSE)
    {
        Uds_ErrorHandle(SID,Uds_E_serviceNotSupportedInActiveSession,RxBufferID);
        return E_NOT_OK;
    }  
  
    /*检测服务安全性设置*/
    if(UdsInternal_IsSidSecurityValid(RxBufferID,SID)==FALSE)
    {
        Uds_ErrorHandle(SID,Uds_E_SECURITYACCESSDENIED,RxBufferID);
        return E_NOT_OK;
    }
    
    uds.Active=TRUE;
    UdsInternal_Process(RxBufferID);
    return E_OK;
}


void UdsInternal_TesterPresent(PduIdType RxBufferID)
{
    uint8 SubFunction;
    PduInfoType *pTxBufPtr =  GetTxBuffer();
    PduInfoType *pRxBufPtr =  GetRxBuffer();

    //发送请求的数据长度错误
    if(pRxBufPtr->SduLength != 2)
    {
        Uds_ErrorHandle(0x3E,Uds_E_incorrectMessageLengthOrInvalidFormat,RxBufferID);
        return;
    }
    
    SubFunction = pRxBufPtr->SduDataPtr[1];

    //不支持所请求的子服务
    if (SubFunction!=0x00 && SubFunction!=0x80)
    {
        Uds_ErrorHandle(0x3E,Uds_E_SubFunctionNotSupported,RxBufferID);
        return;
    }
  
    Uds_TesterPresent();    

    if ((SubFunction & 0x80) == 0x00)
    {
        pTxBufPtr->SduLength = 0;
        pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++]=0x7E;
        pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++]=0x0;
        UdsInternal_ProcessingDone(pTxBufPtr->SduLength);
    }
    else
    {
        Uds_CanTpTxConfirmation(0,NTFRSLT_OK);
    }
    return;
}

void UdsInternal_DiagnosticSessionControl(PduIdType RxBufferID) 
{
    uint16 i;
	uint8 ret;
    uint8 SubFunction,Session;
    
    PduInfoType *pTxBufPtr = GetTxBuffer();
    PduInfoType *pRxBufPtr = GetRxBuffer();
  
    const SessionRow * DspSessionTable=UDS._Dsp.Session;

    SubFunction = pRxBufPtr->SduDataPtr[1];
       
    //发送请求的数据长度错误
    if(pRxBufPtr->SduLength!=2)
    {
        Uds_ErrorHandle(0x10,Uds_E_incorrectMessageLengthOrInvalidFormat,RxBufferID);
        return;
    }

    //不支持所请求的子服务
    Session = SubFunction & 0x7F;
    for(i=0;DspSessionTable[i].SessionLevel!=0;i++) 
    {
        if(Session==DspSessionTable[i].SessionLevel)
        {
            break;
        }
    }   
        
    if(DspSessionTable[i].SessionLevel==0)
    {
        Uds_ErrorHandle(0x10,Uds_E_SubFunctionNotSupported,RxBufferID);
        return;
    }

	ret = Uds_DiagnosticSessionControlPreCheck(uds.SessionLevel,Session);
	if (ret != E_OK)
	{
        Uds_ErrorHandle(0x10,ret,RxBufferID);
        return;
	}

    uds.SeedValid = FALSE; 
    uds.TransferState =NONE;
    uds.SessionLevel=Session;   
     
    if ((SubFunction & 0x80) == 0)
    {
        pTxBufPtr->SduLength = 0;      
        pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++]=0x50;
        pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++]=SubFunction;
        pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++]=P2_CAN_SERVER_MAX_HIGH;
        pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++]=P2_CAN_SERVER_MAX_LOW;
        pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++]=P2_STAR_CAN_SERVER_MAX_HIGH;
        pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++]=P2_STAR_CAN_SERVER_MAX_LOW;         
        UdsInternal_ProcessingDone(pTxBufPtr->SduLength);
    }
    else
    {
        Uds_CanTpTxConfirmation(0,NTFRSLT_OK);
    }  

    Uds_DiagnosticSessionControl(Session);
    return;  
}


void UdsInternal_ControlDTCSetting(PduIdType RxBufferID)
{
    uint8 SubFunction,ControlType;
    PduInfoType *pTxBufPtr = GetTxBuffer();
    PduInfoType *pRxBufPtr = GetRxBuffer();

    //发送请求的数据长度错误
    if(pRxBufPtr->SduLength != 2)
    {
        Uds_ErrorHandle(0x85,Uds_E_incorrectMessageLengthOrInvalidFormat,RxBufferID);
        return;
    }

    SubFunction = pRxBufPtr->SduDataPtr[1];
    ControlType = SubFunction & 0x7F;

    //不支持所请求的子服务
    if ((ControlType!=0x01) && (ControlType != 0x02) && (ControlType!=0x81) && (ControlType != 0x82))
    {
        Uds_ErrorHandle(0x85,Uds_E_SubFunctionNotSupported,RxBufferID);
        return;
    }
    
    Uds_ControlDTCSetting(ControlType);
  
    if ((SubFunction & 0x80) == 0)
    {      
        pTxBufPtr->SduLength = 0;
        pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++]=0xC5;
        pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++]=ControlType;
        UdsInternal_ProcessingDone(pTxBufPtr->SduLength);
    }
    else
    {
        Uds_CanTpTxConfirmation(0,NTFRSLT_OK);
    }  
    return;
}


void UdsInternal_CommunicationControl(PduIdType RxBufferID)
{
    uint8 SubFunction,communicationType,controlType;
    PduInfoType *pTxBufPtr = GetTxBuffer();
    PduInfoType *pRxBufPtr = GetRxBuffer();
    
    //发送请求的数据长度错误
    if(pRxBufPtr->SduLength!=3)
    {
        Uds_ErrorHandle(0x28,Uds_E_incorrectMessageLengthOrInvalidFormat,RxBufferID);
        return;
    }

    SubFunction = pRxBufPtr->SduDataPtr[1];
    controlType= SubFunction;
    //不支持所请求的子服务
    if ((controlType!=0x00) && (controlType != 0x03) && (controlType != 0x01))
    {
        Uds_ErrorHandle(0x28,Uds_E_SubFunctionNotSupported,RxBufferID);
        return;
    }
    
    communicationType =pRxBufPtr->SduDataPtr[2];
    if ((communicationType!=0x01) && (communicationType != 0x02) && (communicationType != 0x03))
    {
        Uds_ErrorHandle(0x28,Uds_E_REQUESTOUTOFRANGE,RxBufferID);
        return;
    }

    Uds_CommunicationControl(controlType,pRxBufPtr->SduDataPtr[2]); 

  //if ((SubFunction & 0x80) == 0)
    {
        pTxBufPtr->SduLength = 0;
        pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++]=0x68;
        pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++]=SubFunction;
        UdsInternal_ProcessingDone(pTxBufPtr->SduLength);
    }
/*
    else
    {
        Uds_CanTpTxConfirmation(0,NTFRSLT_OK);
    } 
*/
    return;
}


void UdsInternal_ECUReset(PduIdType RxBufferID) 
{
    uint8 SubFunction,ret;
    PduInfoType *pTxBufPtr = GetTxBuffer();
    PduInfoType *pRxBufPtr = GetRxBuffer();
    SubFunction = pRxBufPtr->SduDataPtr[1];
  
    //发送请求的数据长度错误
    if(pRxBufPtr->SduLength != 2)
    {
        Uds_ErrorHandle(0x11,Uds_E_incorrectMessageLengthOrInvalidFormat,RxBufferID);
        return;
    }
   
    
    //不支持所请求的子服务resetType
    if ((SubFunction!=0x01)  && (SubFunction!=0x81))
    {
        Uds_ErrorHandle(0x11,Uds_E_SubFunctionNotSupported,RxBufferID);
        return;
    }


    ret = Uds_EcuResetPreCheck(SubFunction);
	if (ret != E_OK)
	{
        Uds_ErrorHandle(0x11,ret,RxBufferID);
        return;
	}
	if ((SubFunction & 0x80) == 0)
	{
    pTxBufPtr->SduLength = 0;        
    pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++]=0x51;
    pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++]=SubFunction;
            
    UdsInternal_ProcessingDone(pTxBufPtr->SduLength);
	}
	else
		{
		    Uds_CanTpTxConfirmation(0,NTFRSLT_OK);
		}
    Uds_EcuReset(SubFunction);
 
    return;       
}


void UdsInternal_SecurityAccess(PduIdType RxBufferID) 
{
    uint8 i,securitySeedSize=0,SubFunction=0;
    uint8 securityAccessType;
    uint8 result;
    PduInfoType *pTxBufPtr = GetTxBuffer();
    PduInfoType *pRxBufPtr = GetRxBuffer();

    //发送请求的数据长度错误
    if(pRxBufPtr->SduLength<2)
    {      
         Uds_ErrorHandle(0x27,Uds_E_incorrectMessageLengthOrInvalidFormat,RxBufferID);
         return;
    }
        
    SubFunction=pRxBufPtr->SduDataPtr[1];
   
    pTxBufPtr->SduLength = 0;
    pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++]=0x67; 
    pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++]=SubFunction; 
    
   // if((SubFunction == 0x01)|| SubFunction == 0x09)  //Request Seed
   // if ((SubFunction!=0x01) &&(SubFunction!=0x02))
	if ((SubFunction!=0x03) &&(SubFunction!=0x04))	
   {
	   Uds_ErrorHandle(0x27,Uds_E_SubFunctionNotSupported,RxBufferID);
	   return;
   }
    //if((SubFunction == 0x01))  //Request Seed 2019-4-22
    if(SubFunction == 0x03)  //Request Seed
    {
        if(pRxBufPtr->SduLength != 2)
        {      
          Uds_ErrorHandle(0x27,Uds_E_incorrectMessageLengthOrInvalidFormat,RxBufferID);
          return;
        }  
        securityAccessType =  SubFunction;
        result =Uds_RequestSeed(securityAccessType,&pRxBufPtr->SduDataPtr[2], &pTxBufPtr->SduDataPtr[2],&securitySeedSize);   
        if (result == E_OK)
        {
            for(i=0;i<securitySeedSize;i++)
            {             
                uds.Ciphertext[i] = pTxBufPtr->SduDataPtr[2+i];
            }
            pTxBufPtr->SduLength +=securitySeedSize;
            uds.SeedValid = TRUE;
            uds.KeyRetries = 0;
        }
        else
        {
            Uds_ErrorHandle(0x27,Uds_E_REQUESTOUTOFRANGE,RxBufferID);
            return; 
        }
    }
   // else if ((SubFunction == 0x02)|| SubFunction == 0x0A)
    //else if ((SubFunction == 0x02))
    else if(SubFunction == 0x04)
    {
        if(pRxBufPtr->SduLength != 4)
        {      
          Uds_ErrorHandle(0x27,Uds_E_incorrectMessageLengthOrInvalidFormat,RxBufferID);
          return;
        }  
        if (uds.SeedValid == TRUE)
        {
            securityAccessType = (SubFunction - 1);
            if (Uds_CompareKey(securityAccessType,uds.Ciphertext,&pRxBufPtr->SduDataPtr[2]) == E_OK)   
            {
                uds.SecurityLevel= securityAccessType;  
                uds.SeedValid = FALSE;     //通过验证后SEED失效
            }
            else
            {
                uds.KeyRetries++;
                if (uds.KeyRetries <=UDS_KEY_RETRIES_MAX)
                {  
                    Uds_ErrorHandle(0x27,Uds_E_SecurityAcces_InvalidKey,RxBufferID);
                    return;
                }
                else
                {
                    uds.SeedValid = FALSE;   //超过重试次数后SEED失效
                    Uds_ErrorHandle(0x27,Uds_E_SecurityAcces_exceededNumberOfAttempts,RxBufferID);
                    return;
                }
            }
        }
        else
        {
            Uds_ErrorHandle(0x27,Uds_E_REQUESTSEQUENCEERROR,RxBufferID);
            return;     
        }
    }
    else 
    {
   
    	Uds_ErrorHandle(0x27,0x7E,RxBufferID);
        return;     
    }
  
    UdsInternal_ProcessingDone(pTxBufPtr->SduLength);
    return;
}


void UdsInternal_ReadDataByIdentifier(PduIdType RxBufferID)
{
    uint8 didSize;
    uint8 result;
    uint16 did;   
    PduInfoType *pTxBufPtr = GetTxBuffer();
    PduInfoType *pRxBufPtr = GetRxBuffer();

    //发送请求的数据长度错误
    if(pRxBufPtr->SduLength != 3)
    {
        Uds_ErrorHandle(0x22,Uds_E_incorrectMessageLengthOrInvalidFormat,RxBufferID);
        return;
    }  
    
    did=(((uint16)pRxBufPtr->SduDataPtr[1]) << 8 & 0xFF00) | (((uint16)pRxBufPtr->SduDataPtr[2]) & 0xFF) ;
    
    result = Uds_ReadDataByIdentifier(did,&pTxBufPtr->SduDataPtr[3],&didSize); 

    if (result == E_OK)
    {     
        pTxBufPtr->SduLength =0;
        pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++]=0x62;
        pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++]=pRxBufPtr->SduDataPtr[1];
        pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++]=pRxBufPtr->SduDataPtr[2];
        pTxBufPtr->SduLength += didSize;
        UdsInternal_ProcessingDone(pTxBufPtr->SduLength);
    }
    else
    {
        Uds_ErrorHandle(0x22,result,RxBufferID);
    }
    return;
}



void UdsInternal_WriteDataByIdentifier(PduIdType RxBufferID)
{
    uint16 did;
    uint8 result;
    PduInfoType *pTxBufPtr = GetTxBuffer();
    PduInfoType *pRxBufPtr = GetRxBuffer();

    //发送请求的数据长度错误
    if(pRxBufPtr->SduLength < 4)
    {
        Uds_ErrorHandle(0x2E,Uds_E_incorrectMessageLengthOrInvalidFormat,RxBufferID);
        return;
    }
    
    did=(((uint16)pRxBufPtr->SduDataPtr[1]) << 8 & 0xFF00) | (((uint16)pRxBufPtr->SduDataPtr[2]) & 0xFF) ;
    
    result=Uds_WriteDataByIdentifier(did,&pRxBufPtr->SduDataPtr[3],pRxBufPtr->SduLength-3);  // this function is doing nothing now(2014-10-23)

    if (result == E_OK)
    {     
        pTxBufPtr->SduLength = 0;
        pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++]=0x6E;
        pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++]=(uint8)((did&0xff00)>>8);   //回写高字节
        pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++]=(uint8)(did&0x00ff);        //回写低字节
        UdsInternal_ProcessingDone(pTxBufPtr->SduLength);
    }
    else
    {
        Uds_ErrorHandle(0x2E,result,RxBufferID);
    }
    return;
}

void UdsInternal_ClearDiagnosticInformation(PduIdType RxBufferID)
{
    uint32 groupOfDTC=0;
    uint8 result;
    PduInfoType *pTxBufPtr = GetTxBuffer();
    PduInfoType *pRxBufPtr = GetRxBuffer();

    //发送请求的数据长度错误
    if(pRxBufPtr->SduLength != 4)
    {
        Uds_ErrorHandle(0x14,Uds_E_incorrectMessageLengthOrInvalidFormat,RxBufferID);
        return;
    }
 
    groupOfDTC = (((uint32)pRxBufPtr->SduDataPtr[1]<<16))+((uint32)pRxBufPtr->SduDataPtr[2] << 8) + ((uint32)pRxBufPtr->SduDataPtr[3]);     
    if(groupOfDTC != 0xFFFFFF)
    {
    	Uds_ErrorHandle(0x14,Uds_E_REQUESTOUTOFRANGE,RxBufferID);
    	return ;
    }
    result = Uds_ClearDTC(groupOfDTC); 
    
    if (result == E_OK)
    {
        pTxBufPtr->SduLength = 0;
        pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++]=0x54;
        UdsInternal_ProcessingDone(pTxBufPtr->SduLength);
    }
    else
    {
        Uds_ErrorHandle(0x14,result,RxBufferID);
    }
    return;
}

void UdsInternal_ReadDTCInformation(PduIdType RxBufferID)
{
    uint8 subFunction;   
    PduInfoType *pTxBufPtr = GetTxBuffer();
    PduInfoType *pRxBufPtr =  GetRxBuffer();
    uint8  DTCStatusMask;
    uint8  ret=0;
    uint16  resLength=0;
    uint32 MaskRecord;
    uint8  RecordNumber;
    

    //发送请求的数据长度错误
    if(pRxBufPtr->SduLength < 2)
    {
        Uds_ErrorHandle(0x19,Uds_E_incorrectMessageLengthOrInvalidFormat,RxBufferID);
        return;
    }
    
    subFunction =pRxBufPtr->SduDataPtr[1];

    pTxBufPtr->SduLength = 0;
    pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++]=0x59;
    pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++]=subFunction;
    
    switch(subFunction)
    {
        //reportNumberOfDTCByStatusMask    
        case 0x01:
        if(pRxBufPtr->SduLength != 3)
        {
            Uds_ErrorHandle(0x19,Uds_E_incorrectMessageLengthOrInvalidFormat,RxBufferID);
            return;
        }
        DTCStatusMask = pRxBufPtr->SduDataPtr[2]; 
        pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++] = DTCStatusAvailabilityMask;
        pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++] = ISO14229_1DTCFORMAT;
        ret=Uds_ReportNumberOfDTCByStatusMask(DTCStatusMask,&pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength],&resLength);
        break;
        
        //reportDTCByStatusMask
        case 0x02:
        if(pRxBufPtr->SduLength != 3)
        {
            Uds_ErrorHandle(0x19,Uds_E_incorrectMessageLengthOrInvalidFormat,RxBufferID);
            return;
        }
        DTCStatusMask = pRxBufPtr->SduDataPtr[2];
        pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++] = DTCStatusAvailabilityMask;
        ret=Uds_ReportDTCByStatusMask(DTCStatusMask,&pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength],&resLength);
        break;
        
      /*  
        //reportDTCSnapshotRecordByDTCNumber
        case 0x04:
        if(pRxBufPtr->SduLength != 6)
        {
            Uds_ErrorHandle(0x19,Uds_E_incorrectMessageLengthOrInvalidFormat,RxBufferID);
            return;
        }
        MaskRecord = ((uint32)pRxBufPtr->SduDataPtr[2] << 16) | ((uint32)pRxBufPtr->SduDataPtr[3] << 8) | pRxBufPtr->SduDataPtr[4];
        RecordNumber = pRxBufPtr->SduDataPtr[5];
        if ((RecordNumber != 0x00)
            && (RecordNumber != 0x01)
            && (RecordNumber != 0xFF))
        {
            Uds_ErrorHandle(0x19,Uds_E_REQUESTOUTOFRANGE,RxBufferID);
            return;    
        }
        ret=Uds_ReportDTCSnapshotRecordByDTCNumber(MaskRecord,RecordNumber,&pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength],&resLength);
        break; 
        
        //reportDTCExtendedDataRecordByDTCNumber
    
        case 0x06:
        MaskRecord = ((uint32)pRxBufPtr->SduDataPtr[2] << 16) | ((uint32)pRxBufPtr->SduDataPtr[3] << 8) | pRxBufPtr->SduDataPtr[4];
        RecordNumber = pRxBufPtr->SduDataPtr[5];
        ret=Uds_ReportDTCExtendedDataRecordByDTCNumber(MaskRecord,RecordNumber,&pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength],&resLength);
        break;
     */

        //reportDTCExtendedDataRecordByDTCNumber
        case 0x0A:
        if(pRxBufPtr->SduLength != 2)
        {
            Uds_ErrorHandle(0x19,Uds_E_incorrectMessageLengthOrInvalidFormat,RxBufferID);
            return;
        }
        pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++] = DTCStatusAvailabilityMask;
        ret=Uds_ReportSupportedDTC(&pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength],&resLength);
        break;       
               
        default:
        Uds_ErrorHandle(0x19,Uds_E_SubFunctionNotSupported,RxBufferID);
        return;      
    }
    
    if (ret == E_OK)
    {
        pTxBufPtr->SduLength += resLength;
        UdsInternal_ProcessingDone(pTxBufPtr->SduLength);
    }
    else
    {
        Uds_ErrorHandle(0x19,ret,RxBufferID);
    }
    return;
}

void UdsInternal_InputOutputControlByIdentifier(PduIdType RxBufferID) 
{
    uint16  dataIdentifier;
    uint16  result;  
    uint16  controlStatusRecordSize;  
    PduInfoType *pTxBufPtr = GetTxBuffer();
    PduInfoType *pRxBufPtr = GetRxBuffer();
        
    //发送请求的数据长度错误
    if(pRxBufPtr->SduLength < 3)
    {
        Uds_ErrorHandle(0x2F,Uds_E_incorrectMessageLengthOrInvalidFormat,RxBufferID);
        return;
    }

    dataIdentifier = ((uint16)pRxBufPtr->SduDataPtr[1]<<8)+(uint16)pRxBufPtr->SduDataPtr[2];     
    result = Uds_InputOutputControlByIdentifier(dataIdentifier,&pRxBufPtr->SduDataPtr[3],&pTxBufPtr->SduDataPtr[3],&controlStatusRecordSize);

    if (result == E_OK)
    {
        pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++]=0x6F;
        pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++]=(uint8)((dataIdentifier&0xff00)>>8);
        pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++]=(uint8)(dataIdentifier&0x00ff);
        pTxBufPtr->SduLength +=controlStatusRecordSize;
        UdsInternal_ProcessingDone(pTxBufPtr->SduLength);
    }
    else
    {
        Uds_ErrorHandle(0x2F,result,RxBufferID);
    }

    return;       
}

void UdsInternal_RoutineControl(PduIdType RxBufferID) 
{
    uint8   routineStatusRecordSize=0;
    uint8   SubFunction;
    uint16  RoutineId;
    uint8   result;        
    PduInfoType *pTxBufPtr = GetTxBuffer();
    PduInfoType *pRxBufPtr = GetRxBuffer();
        
    //发送请求的数据长度错误
    //if(pRxBufPtr->SduLength < 2)
    if(pRxBufPtr->SduLength != 4) //按照汉腾的格式进行修改 不一定符合标准
    {
        Uds_ErrorHandle(0x31,Uds_E_incorrectMessageLengthOrInvalidFormat,RxBufferID);
        return;
    }

    SubFunction =   pRxBufPtr->SduDataPtr[1];
    //不支持所请求的子服务
    if (SubFunction!=0x01 
        && SubFunction!=0x02
        && SubFunction!=0x03)
    {
        Uds_ErrorHandle(0x31,Uds_E_SubFunctionNotSupported,RxBufferID);
        return;
    }
                   
    RoutineId = ((uint16)pRxBufPtr->SduDataPtr[2]<<8)+(uint16)pRxBufPtr->SduDataPtr[3];        
    result = Uds_RoutineControl(SubFunction,RoutineId,&pRxBufPtr->SduDataPtr[4],&pTxBufPtr->SduDataPtr[4],&routineStatusRecordSize);

    if (result == E_OK)
    {
        pTxBufPtr->SduLength = 0;   
        pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++]= 0x71;
        pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++]= SubFunction;
        pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++]= (uint8)((RoutineId&0xff00)>>8);
        pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++]= (uint8)(RoutineId&0x00ff);
        pTxBufPtr->SduLength += routineStatusRecordSize;
        UdsInternal_ProcessingDone(pTxBufPtr->SduLength);
    }
    else
    {
        Uds_ErrorHandle(0x31,result,RxBufferID);
    }
    return;       
}

void UdsInternal_ProcessingDone(PduLengthType Length)
{
    PduInfoType *pTxBufPtr = NULL;
    pTxBufPtr = GetTxBuffer();
    pTxBufPtr->SduLength = Length;
    
    CanTp_Transmit((uint8)(0), pTxBufPtr);

    return;
}



void Uds_ProcessingWait(uint8 sid,PduIdType RxBufferID)
{
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
    txFrame.Data[0]=0x03;
    txFrame.Data[1]=0x7F;
    txFrame.Data[2]=sid;
    txFrame.Data[3]=0x78;
    txFrame.DataLen=8;                      
    CanIf_Transmit(&txFrame);  
}

void Uds_ErrorHandle(uint8 sid,uint8 NRC_,PduIdType RxBufferID)
{
    PduInfoType *pTxBufPtr =GetTxBuffer();
      
    uds.State = UDS_IDLE;
    
    if (RxBufferID  == FUNCTIONAL_ADDRESS)
    {
    	if((sid != 0x14))
        return;

    	if(NRC_ == Uds_E_SubFunctionNotSupported)
    	{
    		return ;
    	}
    }

    RxBufferID = 0;
    
    pTxBufPtr->SduLength = 0;
    pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++] = 0x7f;
    pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++] =  sid;
    pTxBufPtr->SduDataPtr[pTxBufPtr->SduLength++] =  NRC_;

    UdsInternal_ProcessingDone(pTxBufPtr->SduLength);
    
    return;
}


Std_ReturnType UdsInternal_Process(PduIdType RxBufferID)
{
    uds.Active = TRUE;
    switch (SID)
    {
        case 0x3E:
            UdsInternal_TesterPresent(RxBufferID);
            break;
        case 0x10:
            UdsInternal_DiagnosticSessionControl(RxBufferID);
            break;
        case 0x28:
            UdsInternal_CommunicationControl(RxBufferID);
            break;
        case 0x85:
            UdsInternal_ControlDTCSetting(RxBufferID);
            break;
        case 0x11:
            UdsInternal_ECUReset(RxBufferID);
            break;
        case 0x27:
            UdsInternal_SecurityAccess(RxBufferID);
            break;
        case 0x22:
            UdsInternal_ReadDataByIdentifier(RxBufferID);
            break;
        case 0x2E:
            UdsInternal_WriteDataByIdentifier(RxBufferID);
            break;
        case 0x14:
            UdsInternal_ClearDiagnosticInformation(RxBufferID);
            break;
        case 0x19:
            UdsInternal_ReadDTCInformation(RxBufferID);
            break; 
        case 0x2F:
            UdsInternal_InputOutputControlByIdentifier(RxBufferID);
            break;        
        case 0x31:
            UdsInternal_RoutineControl(RxBufferID);
            break;   
        default:
            return E_NOT_OK;       
    }
    uds.TimeStamp = Uds_GetTime(); //避免会话超时
    return E_OK;
}


uint8 Uds_GetStatus()
{
    return uds.Active;
}

void Uds_Tick(uint8 tick)
{
    uds.Tick+=tick;
}
