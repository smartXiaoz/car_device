
#include "app.h"
#include "hardware.h"

#include "UartExp.h"
#include "LAroundExp.h"


//������
TaskHandle_t  ProcessTASK_Handler;
//task1������

u8 HT_SWITCH = 0xFF;


static void Process_task(void *pvParameters)
{
	static u16 m_cnt = 0;
	static u16 l_cnt = 0;
	static u16 ACC_cnt = 0;
	static u16 CAN_STA_CNT = 0;
	
	
	while(1)
	{

		if(SYS_Inf.STA == 2)//ϵͳ����
		{
			//CAN������
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
					else m_cnt=0,CMD_8100();//MPU����
				}
				else m_cnt = 0;
			}
#ifdef LOOK_AROUND
#ifdef TW2836

#else 
			if(HT_SWITCH&0x02)//����Ϊ���Ӳ�������
			{
				if(LAROUND_Inf.STA != 0)
				{
					if(l_cnt<1250)l_cnt++;
					else l_cnt=0,LA_CMD_81();//��������
				}
				else l_cnt = 0;
			}
#endif
#endif
//			if(CAR_Inf.ACCDe == 0)//ACC���絹��ʱ֪ͨ
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

	//���ú�����������
void ProcessTASK_Create(u8 TASK_PRIO,u16 SIZE)
{
	xTaskCreate((TaskFunction_t )Process_task,           //������     
							(const char*    )"Process_task",         //������   
							(uint16_t       )SIZE,               //�����ջ��С	     
							(void*          )NULL,                  
							(UBaseType_t    )TASK_PRIO,          //�������ȼ�      
							(TaskHandle_t*  )&ProcessTASK_Handler);  //������
}






