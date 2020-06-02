/*
******************************************************************
*
*
*
******************************************************************
*/
#ifndef _UDS_CFG_H_
#define _UDS_CFG_H_

#include "common/Platform_Types.h"
 

#define UDS_BOOTLOADER                  STD_OFF

#define UDS_MAX_SEGMENTS                20

#define UDS_RX_BUFFER_SIZE              256
#define UDS_TX_BUFFER_SIZE              256


#define UDS_TX_BLOCK_SIZE               256
#define UDS_TX_DATA_SIZE                256
//#define UDS_FLASH_PAGE_MASK             0xFFFFFFFF
#define UDS_FLASH_PAGE_SIZE             2
#define UDS_FLASH_BYTE_SIZE             2


#define P2_CAN_SERVER_MAX_HIGH          (0x14)
#define P2_CAN_SERVER_MAX_LOW           (0x00)
#define P2_STAR_CAN_SERVER_MAX_HIGH     (0xC8)
#define P2_STAR_CAN_SERVER_MAX_LOW      (0x00)
 


#define FUNCTIONAL_ADDRESS              0x18DBFFFA
#define PHYSICAL_ADDRESS                0x18DA76FA

#define CANTP_TX_ID                     0x18DAFA76


#define UDS_USE_MAINFUNCTION            STD_ON

#define CANTP_FC_PEFRAME                STD_ON
#define CANTP_RX_FC_PEFRAME             0
#define CANTP_RX_FC_STMIN               0

#define UDS_KEY_RETRIES_MAX             3
 
#define DTCStatusAvailabilityMask       0xBD

#define SESSION_TIMEOUT                 5000
#define TP_TIMEOUT                      200
#define FC_TIMEOUT                      500
#define CF_TIMEOUT                      500


#define UDS_DEBUG  
#define UDS_DEBUG_1   


//#define FLASH_REGIONS                   1


#define UDS_RC_CHECKSUM                 0x0202
#define UDS_RC_CHECKPRECONDITION        0x0203
#define UDS_RC_ERASE                    0xff00
#define UDS_RC_VERIFY                   0xff01

#define DTC_NUM                         12//20

 

#endif
