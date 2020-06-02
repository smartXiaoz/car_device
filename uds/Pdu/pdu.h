/*
******************************************************************
* 描述：服务层头文件定义
* 作者：monli
* 版本：V1.4
* 
******************************************************************
*/
#ifndef _UDS_H_
#define _UDS_H_


#include "Interface/uds_cfg.h"
#include "common/uds_types.h"
#include "CanTp/CanTp.h"





/**----------Uds_SesCtrlType---------------**/
#define DEFAULT_SESSION                       0x01
#define PROGRAMMING_SESSION                   0x02
#define EXTENDED_DIAGNOSTIC_SESSION           0x03
#define SAFETY_SYSTEM_DIAGNOSTIC_SESSION      0x04
#define ALL_SESSION_LEVEL                     0xFF



/**----------Uds_NegativeResponseCodeType---------------**/
#define Uds_E_TRANSMISSIONRANGENOTINNEUTRAL   0x8C
#define Uds_E_CONDITIONSNOTCORRECT            0x22
#define Uds_E_RPMTOOHIGH                      0x81
#define Uds_E_VEHICLESPEEDTOOHIGH             0x88
#define Uds_E_GENERALREJECT                   0x10
#define Uds_E_SECURITYACCESSDENIED            0x33
#define Uds_E_BUSYREPEATREQUEST               0x21
#define Uds_E_ENGINERUNTIMETOOLOW             0x85
#define Uds_E_BRAKESWITCH_NOTCLOSED           0x8F
#define Uds_E_VEHICLESPEEDTOOLOW              0x89
#define Uds_E_TORQUECONVERTERCLUTCHLOCKED     0x91
#define Uds_E_VOLTAGETOOHIGH                  0x92
#define Uds_E_REQUESTSEQUENCEERROR            0x24
#define Uds_E_REQUESTOUTOFRANGE               0x31
#define Uds_E_TEMPERATURETOOHIGH              0x86
#define Uds_E_TRANSMISSIONRANGENOTINGEAR      0x8D
#define Uds_E_RequestCorrectlyReceivedResponsePending             0x78
#define Uds_E_RequestServiceNotSupported                          0x11
#define Uds_E_incorrectMessageLengthOrInvalidFormat               0x13
#define Uds_E_SubFunctionNotSupported                             0x12
#define Uds_E_serviceNotSupportedInActiveSession                  0x7f
#define Uds_E_SecurityAcces_InvalidKey                            0x35
#define Uds_E_SecurityAcces_exceededNumberOfAttempts              0x36


#define ISO15051_6DTCFORMAT   0x00
#define ISO14229_1DTCFORMAT   0x01
#define SAEJ1939_73DTCFORMAT  0x02
#define ISO11992_4DTCFORMAT   0x03


typedef enum 
{
    UDS_IDLE = 0,
		UDS_BUSY,
    UDS_WAITING, 
    UDS_ERROR
}Uds_State;



typedef enum
{
    NONE,
    UPLOAD,
    DOWNLOAD
}Uds_TransferState;




typedef struct
{
    uint32              Address;               
    uint32              MemorySize;             
    uint32              Index;
    uint8               Sequnce;                
    Uds_TransferState   TransferState;          
    Uds_State           State;
    uint8               SeedValid;
    uint8               KeyRetries;
    uint8               Ciphertext[8]; 
    uint8               SecurityLevel;
    uint8               SessionLevel;
    uint32              TimeStamp;
    uint32              ID;
    uint8               Active;
    uint8               Reboot; 
    uint8               ChecksumState;
    uint32              Tick;
    uint32              TpTimeStamp;
}Uds;






typedef struct
{
    uint32    Address;
    uint32    MemorySize;
    uint32    Checksum;
} SegInfoType;



void Uds_Init(void);

void Uds_MainTask(void);

void Uds_Tick(uint8 tick);

void Uds_ProcessingWait(uint8 sid,PduIdType RxBufferID);


extern Uds uds;

#endif
