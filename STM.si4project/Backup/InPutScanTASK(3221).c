/**
*-MCU与MPU通信协议解析
*-解析串口收到的数据，进行命令分解传递，
*-应答回复，消息上报，以及命令发送
*-
*-
**/

#include "app.h"
#include "hardware.h"
#include "UartExp.h"

#include "InPut.h"
#include "can.h"


//任务句柄
TaskHandle_t  InPutScanTASK_Handler;

//task1任务函数  
static u8 InPutProcess(void);
static void InPutScan_task(void *pvParameters)
{
	while(1)
	{
		InPutProcess();
		vTaskDelay(10);
	}
}

//调用函数创建任务
void InPutScanTASK_Create(u8 TASK_PRIO,u16 SIZE)
{
	xTaskCreate((TaskFunction_t )InPutScan_task,           //任务函数     
				(const char*    )"InPutScan_task",         //任务名   
				(uint16_t       )SIZE,               //任务堆栈大小	     
				(void*          )NULL,                  
				(UBaseType_t    )TASK_PRIO,          //任务优先级      
				(TaskHandle_t*  )&InPutScanTASK_Handler);  //任务句柄
}



u8 KEY_Process(void);
u8 CARInf_Process(void);
void RTC_Process(void);
void POWER_GET(void);
static u8 InPutProcess(void)
{
	RTC_Process();
	
	InPut_Scan();//ACC和背光调节扫描
	
	CARInf_Process();
	
	Radio_Process();
	
	POWER_GET();
	
	return 0;
}


void POWER_GET(void)//电池电压测量
{
	u16 AD_Value = 0;
	
//	AD_Value = Get_Adc_Average(6,32);
	AD_Value = getbattervalue();
	
	
//	CAR_Inf.Batter_Value = AD_Value*6*330/4095+70;
	
	CAR_Inf.Batter_Value = (((AD_Value*3.3/4095)*11)+0.5)*100;
}

u8 CARInf_Buff[16][2]={0};
u8 CARInf_write = 0;
u8 CARInf_read = 0;
void Write_car(u8 ch,u8 sta)
{
	CARInf_Buff[CARInf_write][0] = ch;
	CARInf_Buff[CARInf_write][1] = sta;
	CARInf_write++;
	if(CARInf_write>=16)CARInf_write = 0;
}

u8 Read_car(u8 *ch,u8 *sta)
{
	if(CARInf_read == CARInf_write) return 0;
	else
	{
		*ch = CARInf_Buff[CARInf_read][0];
		*sta = CARInf_Buff[CARInf_read][1];
		CARInf_read++;
		if(CARInf_read>=16)CARInf_read = 0;
		return 1;
	}
}


extern u8 LA_CMD_82(u8 LAwark_Mod);
u8 CARInf_Process(void)
{
	static u8 Inf_buff[4]={0};//背光，行车状态，左前门，ACC
	static u16 speed = 0;
	static u8 cnt = 0;
	u8 car_sta = 0;
	u8 num=0,mode=0;
	
	if(cnt<50)cnt++;//500ms进行一次车速上报
	else 
	{
		cnt = 0;
		if(speed != CAR_Inf.Car_Speed)
		{
			speed = CAR_Inf.Car_Speed;
			CMD_8101(0);//发送速度，立即发送
		}
	}
	
	
	if(CAR_Inf.NIGHTLIGHT != Inf_buff[0])//背光调节
	{
		Write_car(0,CAR_Inf.NIGHTLIGHT);
		Inf_buff[0] = CAR_Inf.NIGHTLIGHT;
	}
	
	if(CAR_Inf.DIVEDOOR != Inf_buff[2])//驾驶室门
	{
		Write_car(2,CAR_Inf.DIVEDOOR);
		Inf_buff[2] = CAR_Inf.DIVEDOOR;
	}
	
	if(CAR_Inf.ACC != Inf_buff[3])//ACC状态
	{
//		
//		Write_car(3,CAR_Inf.ACC);//不上报ACC状态，只进行失能控制
		if(CAR_Inf.ACC)	CMD_8106(0);//屏幕失能控制
		else			CMD_8106(1);//屏幕失能控制
		Inf_buff[3] = CAR_Inf.ACC;
	}
	
	if(CAR_Inf.ReverseSta == 1)//倒车
	{
		car_sta = 2;
		Astern_Ctrl(ON);//倒车硬线信号
	}
	else //左右转向
	{
		Astern_Ctrl(OFF);//倒车硬线信号
		if((CAR_Inf.LTurnSts == 1)&&(CAR_Inf.RTurnSts == 0))
		{
			car_sta = 3;
		}
		else if((CAR_Inf.LTurnSts == 0)&&(CAR_Inf.RTurnSts == 1))
		{
			car_sta = 4;
		}
		else car_sta = 1;
	}
	
	if(car_sta != Inf_buff[1])//行车状态
	{
		Write_car(1,car_sta);
		Inf_buff[1] = car_sta;
		LAROUND_Inf.VIS = car_sta;
		LA_CMD_82(LAROUND_Inf.VIS);
		if(LAROUND_Inf.VIS == 2)	{Astern_Ctrl(ON);}//倒车硬线信号
		else 						{Astern_Ctrl(OFF);}//倒车硬线信号
	}
	
	if(Check_TXSTA(5) == 0)//发送空闲时，查看背光、方向灯等信息发送任务缓冲，添加发送任务
	{
		if(Read_car(&num,&mode))
		{
			CMD_8103(num,mode);//背光灯调节等信息
			printf("%d,%d;ok\r\n",num,mode);
		} 
	}
	return 0;
}



void RTC_Process(void)
{
//	Can_SF	can_temp;
	
	if(RTC_IRQflag==1)//IC时间同步
	{
		RTC_Get();
		RTC_IRQflag = 2;
//		can_temp.id = 0X18FEE6EE;
//		can_temp.data[0] = calendar.sec*4;
//		can_temp.data[1] = calendar.min;
//		can_temp.data[2] = calendar.hour;
//		can_temp.data[3] = calendar.w_month;
//		can_temp.data[4] = calendar.w_date*4+calendar.hour/6;
//		can_temp.data[5] = calendar.w_year-1985;
//		Can_Write_TXMsg(can_temp,1);
		
	}
}

