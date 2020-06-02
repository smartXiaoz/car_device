#ifndef _LEDTASK_H
#define _LEDTASK_H
#include "FreeRTOS.h"
#include "task.h"

typedef enum{
	BRIGHT  = 0,	//����
	DARK = 1,		//����
	BREATH = 2,		//����
	BLINK = 3,		//��˸
}LED_MODE;

extern LED_MODE LED_GREEN;


//������
extern TaskHandle_t		ledTask_Handler;

void ledTASK_Create(u8 TASK_PRIO,u16 SIZE);

#endif

