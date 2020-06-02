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

static u8 power_cnt = 200;
void POWER_GET(void)//电池电压测量
{
	u16 AD_Value = 0;
	u16 AD_Batter_Value = 0;
	u16 AD_Batter_Temp = 0;
	
	AD_Value = getbattervalue();
	AD_Batter_Temp = (u16)((float)((AD_Value*36.3/4095)+0.98)*100+0.5);
	CAR_Inf.Batter_Value = CAR_Inf.Batter_Value*0.8+AD_Batter_Temp*0.2;
	AD_Batter_Value = CAR_Inf.Batter_Value /10;
	
#if  FICM_12V_ON	
	if(AD_Batter_Value >= 90 && AD_Batter_Value <=160 )
	{
		PowerHigh= 0;
		PowerLow= 0;
		PowerNormal = 1;
		power_cnt = 200;
	}
	else
	{
		PowerNormal = 0;
		if(power_cnt>0)power_cnt--;
		else
		{
			if(AD_Batter_Value <90)
			{
				PowerHigh= 0;
				PowerLow= 1;
				PowerNormal = 0;
//				printf("power_low %d\r\n",CAR_Inf.Batter_Value);
			}
			else if(AD_Batter_Value>160)
			{
				PowerHigh= 1;
				PowerLow= 0;
				PowerNormal = 0;
//				printf("power_high %d\r\n",CAR_Inf.Batter_Value);
			}
		}
	}
#else 
		if(AD_Batter_Value >= 180 && AD_Batter_Value <=310 )
	{
		PowerHigh= 0;
		PowerLow= 0; 
		PowerNormal = 1;
		power_cnt = 200;
	}
	else
	{
		PowerNormal = 0;
		if(power_cnt>0)power_cnt--;
		else
		{
			if(AD_Batter_Value <180)
			{
				PowerHigh= 0;
				PowerLow= 1;
				PowerNormal = 0;
//				printf("power_low %d\r\n",CAR_Inf.Batter_Value);
			}
			else if(AD_Batter_Value>310)
			{
				PowerHigh= 1;
				PowerLow= 0;
				PowerNormal = 0;
//				printf("power_high %d\r\n",CAR_Inf.Batter_Value);
			}
		}
	}
#endif
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

extern u8 CMD_8201(void);//上报开关状态
extern u8 LA_CMD_82(u8 LAwark_Mod);
u8 CARInf_Process(void)
{
	static u8 canset = 0;
	u8 settemp = 0;
	
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
		Write_car(1,CAR_Inf.NIGHTLIGHT);
		Inf_buff[0] = CAR_Inf.NIGHTLIGHT;
	}
	
	if(CAR_Inf.DIVEDOOR != Inf_buff[2])//驾驶室门
	{
		Write_car(3,CAR_Inf.DIVEDOOR);
		Inf_buff[2] = CAR_Inf.DIVEDOOR;
	}
	
	if(CAR_Inf.ACC != Inf_buff[3])//ACC状态
	{
//		
//		Write_car(4,CAR_Inf.ACC);//不上报ACC状态，只进行失能控制
		if(CAR_Inf.ACC)	{CMD_8106(0);Ignition_cnt++;}//屏幕失能控制
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
		Inf_buff[1] = car_sta;
		Write_car(2,car_sta);
		LAROUND_Inf.VIS = car_sta;
#ifdef LOOK_AROUND
		LA_CMD_82(LAROUND_Inf.VIS);
#endif
		if(LAROUND_Inf.VIS == 2)
		{
			Astern_Ctrl(ON);//倒车硬线信号
		}
		else if((LAROUND_Inf.VIS == 3)||(LAROUND_Inf.VIS == 4))
		{
#ifdef LOOK_AROUND
				if(CAR_Inf.Car_Speed<(15<<8))
				{
					Astern_Ctrl(ON);//进入影像
				}
				else
				{
					Astern_Ctrl(OFF);//退出影像
				}
#endif
		}
		else
		{
			Astern_Ctrl(OFF);//倒车硬线信号
		}
	}
	
	if(Check_TXSTA(5) == 0)//发送空闲时，查看背光、方向灯等信息发送任务缓冲，添加发送任务
	{
		if(Read_car(&num,&mode))
		{
			CMD_8103(num,mode);//背光灯调节等信息
//			printf("%d,%d;ok\r\n",num,mode);
		} 
	}
	
	settemp = (CAR_Set.OVERSPEE)+(CAR_Set.CARSEARCH<<1)+(CAR_Set.DRIVLOCK<<2)+(CAR_Set.PARKULOCK<<3)+(CAR_Set.DDWS<<4)+(CAR_Set.BSD<<5);	//车身设置开关状态
	if(settemp != canset)
	{
		canset = settemp;
		CMD_8201();
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

