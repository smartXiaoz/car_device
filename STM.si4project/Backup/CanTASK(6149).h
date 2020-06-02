#ifndef _CANTASK_H
#define _CANTASK_H
#include "FreeRTOS.h"
#include "task.h"

//хннЯ╬Д╠З
extern TaskHandle_t		CanTask_Handler;
extern  u16  Speed_flag;
void CanTASK_Create(u8 TASK_PRIO,u16 SIZE);

#endif

