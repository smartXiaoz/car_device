#ifndef _UARTEXPTASK_H
#define _UARTEXPTASK_H
#include "FreeRTOS.h"
#include "task.h"


//������
extern TaskHandle_t		ProcessTASK_Handler;

void ProcessTASK_Create(u8 TASK_PRIO,u16 SIZE);




#endif





