#include "app.h"
#include "Hardware.h"
#include "EXTI.h"
#include "UartExp.h"

#define WAKETime	10
#define TASKMSG		5

//任务句柄
TaskHandle_t  wakeupTask_Handler;

//task1任务函数
u8 SYS_Process(void);
static void wakeup_task(void *pvParameters)
{
//	u8 time_cnt = 0;
	while(1)
	{
		SYS_Process();
		vTaskDelay(100);
//		if(time_cnt<10)time_cnt++;
//		else
//		{
//			time_cnt = 0;
//			printf("Time:%d-%d-%d %d:%d:%d\r\n",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);
//		}
	}
}



	//调用函数创建任务
void wakeupTASK_Create(u8 TASK_PRIO,u16 SIZE)
{
	xTaskCreate((TaskFunction_t )wakeup_task,           //任务函数     
							(const char*    )"wakeup_task",         //任务名   
							(uint16_t       )SIZE,               //任务堆栈大小	     
							(void*          )NULL,                  
							(UBaseType_t    )TASK_PRIO,          //任务优先级      
							(TaskHandle_t*  )&wakeupTask_Handler);  //任务句柄
}

void SYS_Dormancy(void)
{
	
	CAN_Ctrl(OFF);			//CAN进入低功耗
	EXTIX_Init();
	
#ifdef SKY121
	SYS_Inf.ACC_AWAKE = 0;
#endif
	
	SysTick->CTRL&=~1;
	Sys_Stopby();
	RCC_HSEConfig();
//	CAN_Mode_Init(1,8,9,8,0);
	SysTick->CTRL|=1;
	disable_exit();
	CAN_Ctrl(ON);			//CAN退出低功耗
	CAN_Ctrl(ON);
}

   
//系统逻辑
//
u8 up_cnt = 0;
void cl_upcnt(void)
{
	up_cnt = 0;
}

static u8 low_power_flag = 0;//低压警告
void SYS_Work(void);
void Rest_MPU(__START_Reason mode);
void Off_Time(void);
void Batter_check(void);

u8 SYS_Process(void)
{
	static u8 Cnt_T = 0;
	if(MCU_Awak == 0)//休眠模式中
	{
			//所有任务挂起，断电，休眠
			if(SYS_Inf.STA == 0)//吵醒
			{
				if(Cnt_T<WAKETime)Cnt_T++;
				else
				{
					Cnt_T = 0;
//					printf("system sleep\r\n");
					Mute_Ctrl(ON);
					vTaskDelay(500);
					PAMP_Ctrl(OFF);			//功放休眠
					Power_Ctrl(OFF);		//断电
					OUT_DISABLE();
					CAN_RXclean();
					SYS_Dormancy();
					OUT_ENABLE();
					
//					printf("system awak\r\n");
				}
				if(CAR_Inf.ACC == 1)//特殊启动，ACC上电，滤除解锁信号丢失导致的启动失败
				{
#ifdef	SKY121
					if(SYS_Inf.ACC_AWAKE == 1)
					{
						SYS_Inf.ACC_AWAKE = 0;
						MCU_Awak = 1;
						ContTime = 1;						//关机倒计时，默认10分钟，ACC上电触发成1分钟
					}
#else
					MCU_Awak = 1;
					ContTime = 1;						//关机倒计时，默认10分钟，ACC上电触发成1分钟
#endif					
				}
				else
				{
					ContTime = OFFTIME;						//关机倒计时，默认10分钟，ACC上电触发成1分钟	
				}
			}
			else//睡眠信号
			{
				Mute_Ctrl(ON);
				vTaskDelay(500);
				PAMP_Ctrl(OFF);			//功放休眠
				
				Power_Ctrl(OFF);		//断电
				RADIO_Inf.WORK_STA = 0;	//收音机关
				//必要数据写入EEPROM
				
				Cnt_T = WAKETime;	//直接超时，可立即休眠(实际有100ms系统延时)
				SYS_Inf.STA = 0;//去休眠
				return 0;
			}
	}
	else	//唤醒模式
	{

#ifdef	SKY121
		Batter_check();
#endif		
		
		if(SYS_Inf.STA == 0)//刚唤醒
		{
			
//			if()
			
			SYS_Inf.STA = 1;
			SYS_Inf.START_STA = 1;
			calendar.sta = 0;
			//上电
//			Power_Ctrl(ON);	//电源使能引脚

		}
		else if(SYS_Inf.STA == 1)//任务恢复中
		{
			
			
			if(CAR_Inf.ACC)Rest_MPU(ACCON);					//MPU复位、启动
			else Rest_MPU(PREBOOT);	
			
#ifdef TW2836
			LAROUND_Inf.STA = 1;				//环视，等待状态
#else 
			LAROUND_Inf.STA = 0;				//环视，等待状态
#endif	
			SYS_Inf.STA = 2;					//进入正常工作模式
			
			if(CAR_Inf.ReverseSta == 1)Astern_Ctrl(ON);//进入影像
			else	Astern_Ctrl(OFF);//进入影像
		}
		else if(SYS_Inf.STA == 2)//正常工作中
		{
			SYS_Work();
			if(SYS_Inf.BATTER_VERR==0)Data_Save();//电压保护以后，EEPROM断电 ，不能写入数据
			
		}
		else if(SYS_Inf.STA == 3)//升级模式中
		{
			if(up_cnt<60)up_cnt++;//升级模式保持6秒，6秒无升级包，退出升级模式
			else 
			{
				up_cnt = 0;		
				SYS_Inf.STA = 2;				
				CMD_8503(1);//退出并发送接收失败
				return 0;
			}
			
			if(UP_Data.STA == 1)//刚进入升级模式，准备好以后发送升级数据请求命令
			{
				CMD_8501();//开始升级数据传输
				UP_Data.STA = 2;
			}
			else if(UP_Data.STA == 3)//升级完成，可以重启了
			{
				CMD_8503(0);//发送接收成功
				vTaskDelay(1000);
				Mute_Ctrl(ON);
				vTaskDelay(1000);
				PAMP_Ctrl(OFF);	//功放使能
				vTaskDelay(1000);
				IWDG_Init(4,625);//养狗，等复位升级
				while(1)
				{
					vTaskDelay(1000);
					
				}
			}
			else if(UP_Data.STA == 4)//升级失败，退出
			{
				CMD_8503(1);//发送接收失败
				SYS_Inf.STA =2;//进正常工作模式
			}
		}
	}
	return 0;
}


static u16 MPU_REST_FLAG = 0;
static u16 LOA_WAIT = 0;
u8 batter_cnt1 = 0;
u8 batter_cnt2 = 0;
u8 batter_cnt3 = 0;
void SYS_Work(void)//100ms
{
	static u16 min_cnt = 0;
	static u8  sle = 0;
	
#ifdef	SKY121	
	Off_Time();
#else
	//不用电压保护
	if(CAR_Inf.Batter_Value>3600)//过压保护
	{
		if(batter_cnt2<100)batter_cnt2++;
		else
		{
			SYS_Inf.BATTER_VERR = 1;

			Mute_Ctrl(ON);			//功放禁音
			vTaskDelay(500);
			PAMP_Ctrl(OFF);			//功放休眠
			Power_Ctrl(OFF);		//断电
			RADIO_Inf.WORK_STA = 0;	//收音机关
			SYS_Inf.MPU_STA = 4;	//MPU关机
		}
		batter_cnt1 = 0;
		batter_cnt3 = 0;
	}
	else if(CAR_Inf.Batter_Value<890)//欠压保护
	{
		if(batter_cnt1<150)batter_cnt1++;
		else
		{
			SYS_Inf.BATTER_VERR = 2;
			
			Mute_Ctrl(ON);			//功放禁音
			vTaskDelay(500);
			PAMP_Ctrl(OFF);			//功放休眠
			Power_Ctrl(OFF);		//断电
			RADIO_Inf.WORK_STA = 0;	//收音机关
			SYS_Inf.MPU_STA = 4;	//MPU关机
		}
		batter_cnt2 = 0;
		batter_cnt3 = 0;
	}
	else if((CAR_Inf.Batter_Value>920)&&(CAR_Inf.Batter_Value<3550))//电压OK
	{
		if(SYS_Inf.BATTER_VERR != 0)//电压恢复
		{
			if(batter_cnt3<50)batter_cnt3++;
			else
			{
				if(CAR_Inf.ACCDe)	Rest_MPU(ACCON);
				else			Rest_MPU(PREBOOT);	
				SYS_Inf.BATTER_VERR = 0;
			}
		}
		batter_cnt1 = 0;
		batter_cnt2 = 0;			
	}
	else//
	{
		batter_cnt1 = 0;
		batter_cnt2 = 0;
		batter_cnt3 = 0;
	}	
#endif

	
	
	if(CAR_Inf.ACCDe == 0)//ACC 检测，掉电倒计时
	{
		if(ContTime>0)
		{
			if(min_cnt<600)min_cnt++;
			else 
			{
				min_cnt = 0;
				ContTime--;
//				CMD_8003();//上报倒计时
			}
			
#ifdef	SKY121
			if((ContTime == 1)&&(min_cnt>=300))
			{
				MCU_Awak = 0;
			}
#endif

		}
		else 
		{
			if(sle<10)sle++;
			else 
			{
				sle = 0;
				ContTime = 1;
				MCU_Awak = 0;
			}			
		}
		if((ContTime == 1)&&(SYS_Inf.OFF_TIME == 1))
		{
			if(SYS_Inf.START_STA == 1) 
			{
				CMD_8003();//上报倒计时
				SYS_Inf.OFF_TIME = 0;
			}
		}
	}
	else 
	{
		SYS_Inf.OFF_TIME = 1;
		ContTime = 1;
		min_cnt = 0;
		sle = 0;
		SYS_Inf.START_STA = 0;
	}
	
	switch(SYS_Inf.MPU_STA)
	{
		case 0://开机等待连线,超时后重启
			MPU_REST_FLAG++;
			if(MPU_REST_FLAG>=1800)
			{
				MPU_REST_FLAG = 0;
				Rest_MPU(OTRESET);
			}
		break;
		
		case 1://MPU正常在线
			MPU_REST_FLAG = 0;
		break;
		
		case 2://请求关机
			MCU_Awak = 0;
		break;
		
		case 3://升级离线
		break;
		
		case 4://关机状态
		break;
		
		case 5://MPU故障卡死，重启MPU
			Rest_MPU(OTRESET);
		break;
		default:break;
	}
#ifdef	LOOK_AROUND
	
#ifdef TW2836
#else 
	{
		if(LAROUND_Inf.STA == 0)
		{
			LOA_WAIT++;
			if(LOA_WAIT>=50)//等待30S，超时为故障
			{
				LAROUND_Inf.STA = 2;//标记故障
				FAULT_Inf.LOA = 1;	//标记DTC
			}
		}
	}
#endif
#endif
}


void Rest_MPU(__START_Reason mode)
{
	u8 Temp[2]={0};
	
	if((mode == STRESET)||(mode == OTRESET))//超时复位和设置复位需要断电延时
	{
		
		Mute_Ctrl(ON);
		vTaskDelay(500);
		PAMP_Ctrl(OFF);	//功放使能
		vTaskDelay(500);		
		
		Power_Ctrl(OFF);
		if(mode == OTRESET)//超时重启，记录故障
		{
			FAULT_Inf.MPUDIED = 1;
		}
		vTaskDelay(1000);
	}

	SYS_Inf.OFF_TIME = 1;
	SYS_Inf.MUTE_OK = 0;
	
	Power_Ctrl(ON);
	Mute_Ctrl(ON);
	
	vTaskDelay(1000);

	PAMP_Ctrl(ON);	//功放使能
	MPUReset_Ctrl(OFF);
		
//	RADIO_Inf.MODE = 0;					//默认收音机模式为FM状态，		
//	RADIO_Inf.FM_FRQ = Frequency[0];	//FM_FRQ设置为默认值
//	RADIO_Inf.AM_FRQ = Frequency[2];	//AM_FRQ设置为默认值
		
	RADIO_Inf.WORK_STA = 3;				//收音机设置为预启动状态
	clean_TXtask();						//清空串口所有发送任务
	SYS_Inf.MPU_STA = 0;				//MPU等待连线模式
	MPU_REST_FLAG = 0;					//MPU超时计时
	LOA_WAIT = 0;						//环视超时计时
	START_Reason = mode;				//启动原因
	
#ifdef TW2836	
	TW2836_init();
	LAROUND_Inf.STA = 1;
#endif		

	if(0 == AT24CXX_Check())
	{
//		printf("EEPROM ERROR!\r\n");	
//		vTaskDelay(800); 
		Data_read();
		Temp[0] = AT24CXX_ReadOneByte(EEPROM_ADD+17+14);
		Temp[1] = AT24CXX_ReadOneByte(EEPROM_ADD+17+14+1);
		SYS_Config.DDWS = (u8)(Temp[0]>>0)&0X01;
		SYS_Config.BSD = (u8)(Temp[0]>>1)&0X01;
		SYS_Config.LOA = (u8)(Temp[0]>>2)&0X01;
		SYS_Config.LOC = 1;//标配，强制为1
	
#ifdef LOOK_AROUND
		SYS_Config.LOA = 2;
#endif	
		
	
	}
	else//EEPROM故障默认
	{
		SYS_Config.DDWS = 0;
		SYS_Config.BSD = 0;
		SYS_Config.LOC = 1;//标配，强制为1
		SYS_Config.LOA = 1;
#ifdef LOOK_AROUND
		SYS_Config.LOA = 2;
#endif
	}
	
//		SYS_Config.DDWS = 1;
//		SYS_Config.BSD = 1;
	
//	printf("\r\nconfig= 0x%x,0x%x;\r\n\r\n",AT24CXX_ReadOneByte(EEPROM_ADD+17+14),AT24CXX_ReadOneByte(EEPROM_ADD+17+14+1));
		
}  






extern u8 CMD_8009(u16 Dat);//使用时间
extern u8 CMD_800A(u8 Dat);//一分钟倒计时
#define	STARTTIME	20
#define	GAPTIME		20
#define MIN_CNT		600
static u32 use_time = 0;//使用计时
static u16 off_time = 0;//1分钟计时
static u8 last_mode = 0;
void Batter_check(void)
{
	static u8 batter_cnt1 = 0;
	static u8 batter_cnt2 = 0;
	static u8 batter_cnt3 = 0;
	
	if(CAR_Inf.Batter_Value <= 1190)
	{
		if(batter_cnt1<50) batter_cnt1++;
		else SYS_Inf.Batter_sta	= 2;//低压警告	
		batter_cnt2 = 0;
		batter_cnt3 = 0;
	}
	else if((CAR_Inf.Batter_Value>=1210)&&(CAR_Inf.Batter_Value<=1290))
	{
		if(batter_cnt2<20) batter_cnt2++;
		else SYS_Inf.Batter_sta	= 1;//欠压提醒
		batter_cnt1 = 0;
        batter_cnt3 = 0;
	}
	else if(CAR_Inf.Batter_Value>=1310)
	{
		if(batter_cnt3<10) batter_cnt3++;
		else SYS_Inf.Batter_sta	= 0;//电压正常
		batter_cnt1 = 0;
		batter_cnt2 = 0;
	}
	else
	{
//		batter_cnt1 = 0;
//		batter_cnt2 = 0;
//		batter_cnt3 = 0;		
	}
//	if(SYS_Inf.Batter_sta == 2){low_power_flag = 1;last_mode = 1;}
	if(SYS_Inf.Batter_sta == 0)low_power_flag = 0;	
}

u8 last_acc = 1;

void LowPow_warn(void)
{
	if(SYS_Inf.Batter_sta == 2)
	{
		CMD_800A(1);
		last_mode = 1;
	}
}

void Off_Time(void)
{
	
	if(last_acc != CAR_Inf.ACC)
	{
		last_acc = CAR_Inf.ACC;
		if((SYS_Inf.Batter_sta == 2)&&(CAR_Inf.ACC == 1))
		{
			CMD_800A(1);
			last_mode = 1;
		}
	}
	
	if((CAR_Inf.ACC == 0)||(SYS_Inf.Batter_sta == 2))
	{
//		if(use_time>=(STARTTIME*MIN_CNT))
//		{
//			CMD_8009((u16)(0U));
//		}
		use_time = 0;
		off_time++;
		if(off_time == 1)//低压提醒
		{
			if(CAR_Inf.ACC)
			{
				last_mode = 1;
				CMD_800A(1);
			}
		}
		if(off_time == 600)
		{
			off_time = 0;
			low_power_flag = 1;
			MCU_Awak = 0;//关机休眠
			last_mode = 0;
		}
	}
	else
	{
		if(last_mode == 1)
		{
			CMD_800A(0);
			last_mode = 0;
		}		
		off_time = 0;		
		
		if(SYS_Inf.Batter_sta == 1)
		{
			use_time++;
			if(use_time>=(STARTTIME*MIN_CNT))
			{
				if(((use_time-(STARTTIME*MIN_CNT))%(GAPTIME*MIN_CNT)) == 0)//使用提醒（从15分钟开始，每隔10分钟）
				{
					CMD_8009((u16)(use_time/MIN_CNT));
				}
			}
		}
		else
		{
//			if(use_time>=(STARTTIME*MIN_CNT))
			if(use_time>0)
			{
				CMD_8009((u16)(0U));
			}
			use_time = 0;
		}
	}
}


void batctrl_log(void)
{
	printf("SYS_Inf.Batter_sta = %d\r\n",SYS_Inf.Batter_sta);
	printf("use_time = %d\r\n",use_time);
	printf("off_time = %d\r\n",off_time);
}









