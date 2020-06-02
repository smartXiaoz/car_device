
#include "app.h"
#include "hardware.h"

#include "UartExp.h"
#include "LAroundExp.h"


//任务句柄
TaskHandle_t  ProcessTASK_Handler;
//task1任务函数

u8 HT_SWITCH = 0xFF;


static void Process_task(void *pvParameters)
{
	static u16 m_cnt = 0;
	static u16 l_cnt = 0;
	static u16 ACC_cnt = 0;
	static u16 CAN_STA_CNT = 0;
	
	
	while(1)
	{

		if(SYS_Inf.STA == 2)//系统启动
		{
			//CAN检测测试
			if(CAN_STA_CNT<500)CAN_STA_CNT++;
			else
			{
				CAN_STA_CNT = 0;
				CMD_8680();
			}
			

			if(HT_SWITCH&0x01)//
			{
				if((SYS_Inf.MPU_STA == 1)||(SYS_Inf.MPU_STA == 5))
				{
					if(m_cnt<500)m_cnt++;
					else m_cnt=0,CMD_8100();//MPU心跳
				}
				else m_cnt = 0;
			}
#ifdef LOOK_AROUND
#ifdef TW2836

#else 
			if(HT_SWITCH&0x02)//配置为环视才有心跳
			{
				if(LAROUND_Inf.STA != 0)
				{
					if(l_cnt<1250)l_cnt++;
					else l_cnt=0,LA_CMD_81();//环视心跳
				}
				else l_cnt = 0;
			}
#endif
#endif
//			if(CAR_Inf.ACCDe == 0)//ACC掉电倒计时通知
//			{
//				if(ACC_cnt<1000)ACC_cnt++;
//				else
//				{
//					ACC_cnt = 0;
//					CMD_8003();
//				}
//			}
		}

		TX_Process();
		LA_TX_Process();
		vTaskDelay(4);
	}
}

	//调用函数创建任务
void ProcessTASK_Create(u8 TASK_PRIO,u16 SIZE)
{
	xTaskCreate((TaskFunction_t )Process_task,           //任务函数     
							(const char*    )"Process_task",         //任务名   
							(uint16_t       )SIZE,               //任务堆栈大小	     
							(void*          )NULL,                  
							(UBaseType_t    )TASK_PRIO,          //任务优先级      
							(TaskHandle_t*  )&ProcessTASK_Handler);  //任务句柄
}






