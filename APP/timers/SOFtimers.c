#include "SOFtimers.h"
#include "Hardware.h"
#include "UartExp.h"

#define	IRQ_Time	2

//#if EN_USART1_RX   //���ʹ���˽���
////�����������ʱ��
//xTimerHandle			Usart1RX_Timer_Handle;
//extern void Usart1RX_vTimerCallback(xTimerHandle xTimer);
//#endif
//#if EN_USART2_RX   //���ʹ���˽���
//xTimerHandle			Usart2RX_Timer_Handle;
//extern void Usart2RX_vTimerCallback(xTimerHandle xTimer);
//#endif
//#if EN_USART3_RX   //���ʹ���˽���
//xTimerHandle			Usart3RX_Timer_Handle;
//extern void Usart3RX_vTimerCallback(xTimerHandle xTimer);
//#endif

TimerHandle_t 	KEY_Timer_Handle;			//��������ʱ�����

void KEYCallback(TimerHandle_t xTimer); 	//�����ص�����


//void Auto1Callback(TimerHandle_t xTimer); 	//��ʱ���ص�����
//void Auto2Callback(TimerHandle_t xTimer); 	//��ʱ���ص�����
//void Auto3Callback(TimerHandle_t xTimer); 	//��ʱ���ص�����
//TimerHandle_t 	AutoTimer1_Handle;			//��ʱ�����
//TimerHandle_t 	AutoTimer2_Handle;			//��ʱ�����
//TimerHandle_t 	AutoTimer3_Handle;			//��ʱ�����

void TimersCreate(void)
{

//#if EN_USART1_RX   //���ʹ���˽���
//	/* USART1 Timer */
//	Usart1RX_Timer_Handle = xTimerCreate ((const char*)"Usart1RX_Timer", IRQ_Time, pdFALSE, ( void * ) 2, Usart1RX_vTimerCallback );

//	if( Usart1RX_Timer_Handle != NULL )
//	{
//		if( xTimerStart( Usart1RX_Timer_Handle, 0 ) != pdPASS )
//		{
//			/* The timer could not be set into the Active state. */
//			printf("Usart1RX_Timer Create error! \r\n");
//			while(1);
//		}
//	}
//#endif
//#if EN_USART2_RX   //���ʹ���˽���
//	/* USART2 Timer */
//	Usart2RX_Timer_Handle = xTimerCreate ((const char*)"Usart2RX_Timer", IRQ_Time, pdFALSE, ( void * ) 2, Usart2RX_vTimerCallback );

//	if( Usart2RX_Timer_Handle != NULL )
//	{
//		if( xTimerStart( Usart2RX_Timer_Handle, 0 ) != pdPASS )
//		{
//			/* The timer could not be set into the Active state. */
//			printf("Usart2RX_Timer Create error! \r\n");
//			while(1);
//		}
//	}
//#endif

	
	vTaskDelay(20);
	
    KEY_Timer_Handle=xTimerCreate((const char*)"KEY_Timer",(TickType_t)5,(UBaseType_t)pdTRUE,(void*)1,(TimerCallbackFunction_t)KEYCallback); //���ڶ�ʱ��	
	if( KEY_Timer_Handle != NULL )
	{
		if( xTimerStart( KEY_Timer_Handle, 0 ) != pdPASS )//������ʱ��
		{
			/* The timer could not be set into the Active state. */
//			printf("KEY_Timer_Handle Create error! \r\n");
			while(1);
		}
	}
}




//��ʱ���Ļص�����
void KEYCallback(TimerHandle_t xTimer)
{
	KEY_Get();//����ɨ�裬�������ڵ���
	
}


//u8 KEY_Process(void);
//KEY_Process();
//u8 KEY_Process(void)
//{
//	u8 key = 0;
//	u8 sta = 0;
//	if(Check_TXSTA(4) == 0)//�������Ϳ���ʱ���鿴��������
//	{
//		if(Read_KEY(&key,&sta))
//		{
//			CMD_8102(key,sta);
//		}
//	}
//	return 0;
//}






