#include "app.h"
#include "hardware.h"
#include "Pdu/pdu.h"
#include "CanTp/CanTp.h"
//任务句柄
TaskHandle_t  udsTask_Handler;

//task1任务函数
void uds_dtc_check(void);
static void uds_task(void *pvParameters)
{
	u32 lastWakeTime = xTaskGetTickCount();
	CanTp_Init();
	Uds_Init();
	while(1)
	{
		CanTp_MainTask();
		Uds_MainTask();
		Uds_Tick(20U);
		uds_dtc_check();
		vTaskDelayUntil(&lastWakeTime,20);
	}
}

	//调用函数创建任务
void udsTASK_Create(u8 TASK_PRIO,u16 SIZE)
{
	xTaskCreate((TaskFunction_t )uds_task,           //任务函数     
							(const char*    )"uds_task",         //任务名   
							(uint16_t       )SIZE,               //任务堆栈大小	     
							(void*          )NULL,                  
							(UBaseType_t    )TASK_PRIO,          //任务优先级      
							(TaskHandle_t*  )&udsTask_Handler);  //任务句柄
}





