#ifndef _WAKEUPTASK_H
#define _WAKEUPTASK_H
#include "FreeRTOS.h"
#include "task.h"


//ÈÎÎñ¾ä±ú
extern TaskHandle_t		wakeupTask_Handler;

void wakeupTASK_Create(u8 TASK_PRIO,u16 SIZE);

void SYS_STOP(void);
void SYS_WAKE(void);
u8 Task_report(u8 num);
void cl_upcnt(void);
#endif

