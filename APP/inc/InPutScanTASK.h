#ifndef _LAROUNDTASK_H
#define _LAROUNDTASK_H
#include "FreeRTOS.h"
#include "task.h"


//ÈÎÎñ¾ä±ú
extern TaskHandle_t		InPutScanTASK_Handler;
#define FICM_12V_ON    1

void InPutScanTASK_Create(u8 TASK_PRIO,u16 SIZE);




#endif

