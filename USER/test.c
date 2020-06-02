#include "sys.h"
#include "delay.h"
#include "Hardware.h"		
#include "FreeRTOS.h"
#include "task.h"
#include "app.h"
#include "stmflash.h"




//��ʼ������
TaskHandle_t StartTask_Handler;
//��ʼ������
void start_task(void *pvParameters);

int main(void)
{

	
	JTAG_Set(SWD_ENABLE);
	MY_NVIC_PriorityGroupConfig(4);
	Stm32_Clock_Init(9);//ϵͳʱ������
	OutPut_Init();		  	//��ʼ����LED���ӵ�Ӳ���ӿ�
	InPut_Init(); 
	//������֤
	
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
	
	Tick_init(72);							//��ʱ��ʼ��
	_HARDWARE_INT();
//	while(RTC_Init4())delay_xms(1000);		//RTC��ʼ��	��һ��Ҫ��ʼ���ɹ� 	
	
	//������ʼ����
    xTaskCreate((TaskFunction_t )start_task,			//������
                (const char*    )"start_task",			//��������
                (uint16_t       )512,					//�����ջ��С
                (void*          )NULL,					//���ݸ��������Ĳ���
                (UBaseType_t    )1,						//�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);	//������              
    vTaskStartScheduler();								//�����������	
	
	
	while(1); 
}


//��ʼ����������
void start_task(void *pvParameters)
{
	
	//���������ʱ��
	TimersCreate();

	//�����ٽ���	
	taskENTER_CRITICAL();	
	
	//������Ϣ����
	
	//BSP��ʼ��
//	_HARDWARE_INT();
	//�������� 
	APP_INT();
	//ɾ����ʼ����
	vTaskDelete(StartTask_Handler);
	//�˳��ٽ���
	taskEXIT_CRITICAL();
}


// END 






