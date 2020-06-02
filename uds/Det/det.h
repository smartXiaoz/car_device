/*
******************************************************************
*
*
*
******************************************************************
*/
#ifndef _DET_H_
#define _DET_H_


#include "common/Platform_Types.h"


#define MODULE_CAN          1
#define MODULE_FLASH        2
#define MODULE_CANTP        3
#define MODULE_UDS          4


void Det_Report(uint8 ModuleID,uint8 FuncId,uint8 Code,uint8 Option);


#endif
