#include "app.h"
#include "hardware.h"

#include "information.h"
#include "stdlib.h"


u8 buf[8][8]={
{0xF1,0xF2,0X20,0xC1,'A','B','C','D'},
{0xF1,0xF2,0X20,0xC2,'E','F','G','H'},
{0xF1,0xF2,0X20,0xC3,'I','J','K','L'},
{0xF1,0xF2,0X20,0xC4,'M','N','O','P'},
{0xF3,0xF4,0X20,0xC1,'0','1','0','2'},
{0xF3,0xF4,0X20,0xC2,'0','3','0','4'},
{0xF3,0xF4,0X20,0xC3,'0','5','0','6'},
{0xF3,0xF4,0X20,0xC4,'0','7','0','8'},
};





//任务句柄
TaskHandle_t  ledTask_Handler;

//task1任务函数
static void led_task(void *pvParameters)
{
	u8 RDS_index = 0;
	u8 PI_COD = 0;
	u8 cnt = 0;
	while(1)
	{
		
		if(cnt<100)cnt++;
		else
		{
			cnt = 0;
			PI_COD = PI_COD?0:1;
		}
		
		RDS_index = rand()%4;
		
		RADIO_Inf.RDS_Sta = 1;
		RADIO_Inf.RDS_Block[2] = buf[PI_COD*4+RDS_index][0]; 
		RADIO_Inf.RDS_Block[3] = buf[PI_COD*4+RDS_index][1];
		RADIO_Inf.RDS_Block[4] = buf[PI_COD*4+RDS_index][2];
		RADIO_Inf.RDS_Block[5] = buf[PI_COD*4+RDS_index][3];
		RADIO_Inf.RDS_Block[6] = buf[PI_COD*4+RDS_index][4];
		RADIO_Inf.RDS_Block[7] = buf[PI_COD*4+RDS_index][5];
		RADIO_Inf.RDS_Block[8] = buf[PI_COD*4+RDS_index][6];
		RADIO_Inf.RDS_Block[9] = buf[PI_COD*4+RDS_index][7];
		
		vTaskDelay(100);
	}
}

	//调用函数创建任务
void ledTASK_Create(u8 TASK_PRIO,u16 SIZE)
{
	xTaskCreate((TaskFunction_t )led_task,           //任务函数     
							(const char*    )"led_task",         //任务名   
							(uint16_t       )SIZE,               //任务堆栈大小	     
							(void*          )NULL,                  
							(UBaseType_t    )TASK_PRIO,          //任务优先级      
							(TaskHandle_t*  )&ledTask_Handler);  //任务句柄
}





