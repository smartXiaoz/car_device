#include "SOFtimers.h"
#include "Hardware.h"
#include "UartExp.h"

#define	IRQ_Time	2

//#if EN_USART1_RX   //如果使能了接收
////串口用软件定时器
//xTimerHandle			Usart1RX_Timer_Handle;
//extern void Usart1RX_vTimerCallback(xTimerHandle xTimer);
//#endif
//#if EN_USART2_RX   //如果使能了接收
//xTimerHandle			Usart2RX_Timer_Handle;
//extern void Usart2RX_vTimerCallback(xTimerHandle xTimer);
//#endif
//#if EN_USART3_RX   //如果使能了接收
//xTimerHandle			Usart3RX_Timer_Handle;
//extern void Usart3RX_vTimerCallback(xTimerHandle xTimer);
//#endif

TimerHandle_t 	KEY_Timer_Handle;			//按键处理定时器句柄

void KEYCallback(TimerHandle_t xTimer); 	//按键回调函数


//void Auto1Callback(TimerHandle_t xTimer); 	//定时器回调函数
//void Auto2Callback(TimerHandle_t xTimer); 	//定时器回调函数
//void Auto3Callback(TimerHandle_t xTimer); 	//定时器回调函数
//TimerHandle_t 	AutoTimer1_Handle;			//定时器句柄
//TimerHandle_t 	AutoTimer2_Handle;			//定时器句柄
//TimerHandle_t 	AutoTimer3_Handle;			//定时器句柄

void TimersCreate(void)
{

//#if EN_USART1_RX   //如果使能了接收
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
//#if EN_USART2_RX   //如果使能了接收
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
	
    KEY_Timer_Handle=xTimerCreate((const char*)"KEY_Timer",(TickType_t)5,(UBaseType_t)pdTRUE,(void*)1,(TimerCallbackFunction_t)KEYCallback); //周期定时器	
	if( KEY_Timer_Handle != NULL )
	{
		if( xTimerStart( KEY_Timer_Handle, 0 ) != pdPASS )//开启定时器
		{
			/* The timer could not be set into the Active state. */
//			printf("KEY_Timer_Handle Create error! \r\n");
			while(1);
		}
	}
}




//定时器的回调函数
void KEYCallback(TimerHandle_t xTimer)
{
	KEY_Get();//按键扫描，必须周期调用
	
}


//u8 KEY_Process(void);
//KEY_Process();
//u8 KEY_Process(void)
//{
//	u8 key = 0;
//	u8 sta = 0;
//	if(Check_TXSTA(4) == 0)//按键发送空闲时，查看按键缓冲
//	{
//		if(Read_KEY(&key,&sta))
//		{
//			CMD_8102(key,sta);
//		}
//	}
//	return 0;
//}






