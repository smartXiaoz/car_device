#ifndef _APP_H
#define _APP_H



#include "SOFtimers.h"
#include "ledTASK.h"
#include "wakeupTASK.h"
#include "uartTASK.h"
#include "UartExpTASK.h"
#include "InPutScanTASK.h"
#include "CanTASK.h"

void APP_INT(void);



extern TaskHandle_t  udsTask_Handler;

#endif
