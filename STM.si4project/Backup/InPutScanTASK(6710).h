#ifndef _LAROUNDTASK_H
#define _LAROUNDTASK_H
#include "FreeRTOS.h"
#include "task.h"


//хннЯ╬Д╠З
extern TaskHandle_t		InPutScanTASK_Handler;

void InPutScanTASK_Create(u8 TASK_PRIO,u16 SIZE);




#endif

