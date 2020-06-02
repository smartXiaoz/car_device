#ifndef _LEDTASK_H
#define _LEDTASK_H
#include "FreeRTOS.h"
#include "task.h"

typedef enum{
	BRIGHT  = 0,	//长亮
	DARK = 1,		//长灭
	BREATH = 2,		//呼吸
	BLINK = 3,		//闪烁
}LED_MODE;

extern LED_MODE LED_GREEN;


//任务句柄
extern TaskHandle_t		ledTask_Handler;

void ledTASK_Create(u8 TASK_PRIO,u16 SIZE);

#endif

