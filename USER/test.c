#include "sys.h"
#include "delay.h"
#include "Hardware.h"		
#include "FreeRTOS.h"
#include "task.h"
#include "app.h"
#include "stmflash.h"




//开始任务句柄
TaskHandle_t StartTask_Handler;
//开始任务函数
void start_task(void *pvParameters);

int main(void)
{

	
	JTAG_Set(SWD_ENABLE);
	MY_NVIC_PriorityGroupConfig(4);
	Stm32_Clock_Init(9);//系统时钟设置
	OutPut_Init();		  	//初始化与LED连接的硬件接口
	InPut_Init(); 
	//升级验证
	
#ifdef	upflag_AD
	u16 Temp = 0;	
	STMFLASH_Read(upflag_AD,&Temp,1);
	if(0x0002 == Temp)
	{
		Temp = 0x0003;
		STMFLASH_Write(upflag_AD,&(Temp),1);
		//while(1);
	} 
	else if((0x0003 == Temp)||(0x0004 == Temp))
	{
		Temp = 0x0000;
		STMFLASH_Write(upflag_AD,&(Temp),1);
	}
#endif	
	
	Tick_init(72);							//延时初始化
	_HARDWARE_INT();
//	while(RTC_Init4())delay_xms(1000);		//RTC初始化	，一定要初始化成功 	
	
	//创建开始任务
    xTaskCreate((TaskFunction_t )start_task,			//任务函数
                (const char*    )"start_task",			//任务名称
                (uint16_t       )512,					//任务堆栈大小
                (void*          )NULL,					//传递给任务函数的参数
                (UBaseType_t    )1,						//任务优先级
                (TaskHandle_t*  )&StartTask_Handler);	//任务句柄              
    vTaskStartScheduler();								//开启任务调度	
	
	
	while(1); 
}


//开始任务任务函数
void start_task(void *pvParameters)
{
	
	//创建软件定时器
	TimersCreate();

	//进入临界区	
	taskENTER_CRITICAL();	
	
	//创建消息队列
	
	//BSP初始化
//	_HARDWARE_INT();
	//创建任务 
	APP_INT();
	//删除开始任务
	vTaskDelete(StartTask_Handler);
	//退出临界区
	taskEXIT_CRITICAL();
}


// END 






