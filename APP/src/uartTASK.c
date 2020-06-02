#include "app.h"
#include "Hardware.h"
#include "UartExp.h"
#include "LAroundExp.h"
#include "TestCMD.h"


u8 RTC_sta = 0;
u16 RTC_int_cnt = 0;
u32 cnt1 = 0;
//任务句柄
TaskHandle_t  uartTask_Handler;

void uart_process(void);
//task1任务函数
static void uart_task(void *pvParameters)
{
	while(1)
	{
		if(RTC_sta == 0)
		{
			if(RTC_int_cnt<200)
			{
				vTaskDelay(1);
				RTC_int_cnt++;
			}
			else 
			{
				RTC_int_cnt = 0;
				if(0 == RTC_Init4())//0为成功
				{
					RTC_sta = 1;
				}				
			}
		}
#if 0
		else
		{
			if(calendar.sta == 0)
			{
				calendar.sta = 1;
			}
			if(calendar.sta == 1)//0:开始；1：取样；2：对比；3：OK
			{
				cnt1 = Get_RTCcont();
				calendar.sta = 2;
			}
			else if(calendar.sta == 2)
			{
				if(cnt1 != Get_RTCcont())calendar.sta = 3;
			}q
		}
#else
		
		
#endif
		
		uart_process();
	}
}

	//调用函数创建任务
void uartTASK_Create(u8 TASK_PRIO,u16 SIZE)
{ 
	xTaskCreate((TaskFunction_t )uart_task,           //任务函数     
							(const char*    )"uart_task",         //任务名   
							(uint16_t       )SIZE,               //任务堆栈大小	     
							(void*          )NULL,                  
							(UBaseType_t    )TASK_PRIO,          //任务优先级      
							(TaskHandle_t*  )&uartTask_Handler);  //任务句柄
}


void Write_car(u8 ch,u8 sta);
void DEBUG_Printf(void);
extern u8 HT_SWITCH;
extern void Rest_MPU(__START_Reason mode);
static u16 RX_msg = 0;//目标帧总长度
static u8 data_buff[256]={0};//命令缓冲数组
static u16 U2RX_msg = 0;//目标帧总长度
static u8 U2data_buff[32]={0};//命令缓冲数组

u8 uart2Process(void);
u8 uart3Process(void);
void uart_process(void)
{
	u8 sta2 = 0;
	u8 sta3 = 0;

	
#ifdef LOOK_AROUND	

#ifdef TW2836

	TW2836_Process();

#else	
	
	sta2 = uart2Process();
	if(2 == sta2)
	{
	#ifdef	U2DATALOG
		printf("receive data:");
		for(u8 i = 0;i<U2RX_msg;i++)
		{
			if(U2data_buff[i]>0x0f)	printf("%x ",U2data_buff[i]);
			else					printf("0%x ",U2data_buff[i]);
		}
		printf("\r\n");
	#endif		
		LA_UART_explain(U2RX_msg,U2data_buff);
		U2RX_msg = 0;
	}
#endif
#endif

	
	sta3 = uart3Process();
	if(2 == sta3)
	{
		
#ifdef	U3DATALOG
		printf("receive data:");
		for(u8 i = 0;i<RX_msg;i++)
		{
			if(data_buff[i]>0x0f)	printf("%x ",data_buff[i]);
			else					printf("0%x ",data_buff[i]);
		}
		printf("\r\n");
#endif
		UART_explain(RX_msg,data_buff);
		RX_msg = 0;
	}
	
	
	Test_CMDProcess();
	
	if((sta2|sta3)==0)vTaskDelay(10);
//	else vTaskDelay(1);
}


u8 uart2Process(void)
{
	u8 temp = 0;
	static u8 fn = 0;
	static u16 xA5 = 256;
	static u8 sta = 0;//取缓存状态
	static u16 len = 0;//已获取长度
	
	if(uart2GetByte(&temp))
	{
//		printf(" 0x%x ",temp);
		fn++;
		if(sta == 1)
		{
			U2data_buff[len++] = temp;
			if(len == 4)
			{
				U2RX_msg = U2data_buff[3]+5;
				if(U2RX_msg>32)
				{
					sta = 0;
					len = 0;
					xA5 = 0;
					return 1;
				}
			}
			
			if(len == U2RX_msg)
			{
				sta = 0;
				len = 0;
				xA5 = 0;
				return 2;
			}
			return 1;
		}
		else
		{
			if(temp == 0x5A)
			{
				if(((fn - xA5)==1)||((fn == 0)&&(xA5 == 255)))
				{
					sta = 1;
					U2data_buff[0] = 0xA5;
					U2data_buff[1] = 0x5A;
					len = 2;
				}
				
			}
			xA5 = 256;
			if(temp == 0xA5)xA5 = fn;
			return 1;
		}
	}
	else return 0;	
}


u8 uart3Process(void)
{
	u8 temp = 0;
	static u8 fn = 0;
	static u8 x6A = 0;
	static u8 xA6 = 0;
	static u8 sta = 0;//取缓存状态
	static u16 len = 0;//已获取长度

	if(uart3GetByte(&temp))
	{
		fn++;
		if(sta == 1)//正常取出
		{
			data_buff[len++] = temp;//开始拷贝数据到缓存帧
			if(len == 7)//取出目标长度，此时已经拿到帧长度数据
			{
				RX_msg = (data_buff[5]<<8)+(data_buff[6]+8);//取出帧总长度（数据长度+加上辅助字节长度）
				if(RX_msg>256) 
				{
					sta = 0;
					len = 0;
					return 1;	
				}
			}
			if(RX_msg == len)//一个命令取完了
			{
				sta = 0;
				len = 0;
				return 2;
			}
			return 1;
		}
		else
		{
			if(xA6 == 1)//刚刚取到了同步头，看是不是应答，是表示为一帧开头，开始连续取出该帧，
			{
				if(((temp&0x7F)==0)||((temp&0x7F)==1)||((temp&0x7F)==2)||((temp&0x7F)==3))
				{
					len = 3;//已经拿出来的数据放到缓存帧
					data_buff[0] = 0x6A;
					data_buff[1] = 0xA6;
					data_buff[2] = temp;
					sta = 1;	
				}				
			}
			xA6 = 0;//必须马上清除，防止误操作
			if(temp == 0x6A)	//同步头6A
			{
					x6A = fn;
			}
			if(temp == 0xA6)	//取到A6，恰好刚刚又取到了6A，说明刚好是一帧的头，
			{
				if(((fn - x6A)==1)||(fn == 0))
				{
					xA6 = 1;
					x6A = 0;
				}
			}
			return 1;
		}
	}
	else 
	return 0;
}





extern void batctrl_log(void);

void DEBUG_Printf(void)
{
	printf("\r\nTime:%04d-%02d-%02d %02d:%02d:%02d %d \r\n",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec,calendar.week);
//	printf("SYS :%04d-%02d-%02d,%02d:%02d:%02d\r\n",YEAR,MONTH,DAY,HOUR,MINUTE,SECOND);	//设置时间（程序编译时间）Video_sta
	printf("Video_sta ; %d\r\n",Video_sta);
	printf("SYS_Inf.STA=%d,SYS_Inf.MPU_STA=%d,RADIO_Inf.WORK_STA=%d,CAR_Inf.ACC=%d,\r\n",SYS_Inf.STA,SYS_Inf.MPU_STA,RADIO_Inf.WORK_STA,CAR_Inf.ACC);
	printf("CAR_Inf:Batter_Value = %0.2fV,NIGHTLIGHT=%d,ReverseSta=%d,Door_lock=%d\r\n",(float)CAR_Inf.Batter_Value/100,CAR_Inf.NIGHTLIGHT,CAR_Inf.ReverseSta,CAR_Inf.Door_lock);
	printf("CAR_Inf:LTurnSts=%d,RTurnSts=%d,SPEED:%d\r\n",CAR_Inf.LTurnSts,CAR_Inf.RTurnSts,CAR_Inf.Car_Speed);
	batctrl_log();
	
	printf("RADIO_Inf.Last_FRQ=%d\r\n",RADIO_Inf.Last_FRQ);
	printf("RADIO_Inf.FM_FRQ=%d\r\n",RADIO_Inf.FM_FRQ);
	printf("RADIO_Inf.AM_FRQ=%d\r\n",RADIO_Inf.AM_FRQ);
	
	printf("SYS_Config.LOA=%d\r\n",SYS_Config.LOA);
	printf("LAROUND_Inf.STA=%d\r\n",LAROUND_Inf.STA);
	printf("FAULT_Inf.BUSOFF=%d\r\n",FAULT_Inf.BUSOFF);
	printf("FAULT_Inf.BUSOFF_Flag=%d\r\n",FAULT_Inf.BUSOFF_Flag);
	printf("FAULT_Inf.CAN_OFF=%d\r\n",FAULT_Inf.CAN_OFF);

//	printf("xPortFreeHeapSize = %d \r\n",xPortGetFreeHeapSize());
//	printf("Can = %ld \r\n",uxTaskGetStackHighWaterMark(CanTask_Handler));
//	printf("wakeup = %ld \r\n",uxTaskGetStackHighWaterMark(wakeupTask_Handler));
//	printf("Process = %ld \r\n",uxTaskGetStackHighWaterMark(ProcessTASK_Handler));
//	printf("InPutScan = %ld \r\n",uxTaskGetStackHighWaterMark(InPutScanTASK_Handler));
//	printf("uart = %ld \r\n",uxTaskGetStackHighWaterMark(uartTask_Handler));
//	printf("UDS = %ld \r\n",uxTaskGetStackHighWaterMark(udsTask_Handler));
	printf("version:%s\r\n",Version);
	printf("Build time:%s %s\r\n", __DATE__, __TIME__);
	printf("radio_version:%s \r\n",radio_version);
	
	printf("CAN1->ESR =0x%x\r\n",CAN1->ESR);
	printf("CAN1->MSR =0x%x\r\n",CAN1->MSR);
	printf("CAN1->TSR =0x%x\r\n",CAN1->TSR);	
	printf("CAN1->MCR =0x%x\r\n",CAN1->MCR);
	printf("CAN1->BTR =0x%x\r\n",CAN1->BTR);
	
	printf("RADIO_Inf.RDS_Sta:0x0%x\r\n",RADIO_Inf.RDS_Sta);//RADIO_Inf
	printf("RDS_BUF=0x%x 0x%x 0x%x\r\n",RADIO_Inf.RDS_BUF[0],RADIO_Inf.RDS_BUF[1],RADIO_Inf.RDS_BUF[2]);
	
	printf("RTC->CRH =0x%x\r\n",RTC->CRH);
	printf("RTC->CRL =0x%x\r\n",RTC->CRL);
//	printf("CAN1->TSR =0x%x\r\n",CAN1->TSR);	
//	printf("CAN1->MCR =0x%x\r\n",CAN1->MCR);
//	printf("CAN1->BTR =0x%x\r\n",CAN1->BTR);
}







