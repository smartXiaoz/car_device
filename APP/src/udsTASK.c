#include "app.h"
#include "hardware.h"
#include "Pdu/pdu.h"
#include "CanTp/CanTp.h"
//������
TaskHandle_t  udsTask_Handler;

//task1������
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

	//���ú�����������
void udsTASK_Create(u8 TASK_PRIO,u16 SIZE)
{
	xTaskCreate((TaskFunction_t )uds_task,           //������     
							(const char*    )"uds_task",         //������   
							(uint16_t       )SIZE,               //�����ջ��С	     
							(void*          )NULL,                  
							(UBaseType_t    )TASK_PRIO,          //�������ȼ�      
							(TaskHandle_t*  )&udsTask_Handler);  //������
}





