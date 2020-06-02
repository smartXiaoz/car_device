/**
*-MCU��MPUͨ��Э�����
*-���������յ������ݣ���������ֽ⴫�ݣ�
*-Ӧ��ظ�����Ϣ�ϱ����Լ������
*-
*-
**/

#include "app.h"
#include "hardware.h"
#include "UartExp.h"

#include "InPut.h"
#include "can.h"


//������
TaskHandle_t  InPutScanTASK_Handler;

//task1������  
static u8 InPutProcess(void);
static void InPutScan_task(void *pvParameters)
{
	while(1)
	{
		InPutProcess();
		vTaskDelay(10);
	}
}

//���ú�����������
void InPutScanTASK_Create(u8 TASK_PRIO,u16 SIZE)
{
	xTaskCreate((TaskFunction_t )InPutScan_task,           //������     
				(const char*    )"InPutScan_task",         //������   
				(uint16_t       )SIZE,               //�����ջ��С	     
				(void*          )NULL,                  
				(UBaseType_t    )TASK_PRIO,          //�������ȼ�      
				(TaskHandle_t*  )&InPutScanTASK_Handler);  //������
}



u8 KEY_Process(void);
u8 CARInf_Process(void);
void RTC_Process(void);
void POWER_GET(void);
static u8 InPutProcess(void)
{
	RTC_Process();
	
	InPut_Scan();//ACC�ͱ������ɨ��
	
	CARInf_Process();
	
	Radio_Process();
	
	POWER_GET();
	
	return 0;
}

static u8 power_cnt = 200;
void POWER_GET(void)//��ص�ѹ����
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

extern u8 CMD_8201(void);//�ϱ�����״̬
extern u8 LA_CMD_82(u8 LAwark_Mod);
u8 CARInf_Process(void)
{
	static u8 canset = 0;
	u8 settemp = 0;
	
	static u8 Inf_buff[4]={0};//���⣬�г�״̬����ǰ�ţ�ACC
	static u16 speed = 0;
	static u8 cnt = 0;
	u8 car_sta = 0;
	u8 num=0,mode=0;
	
	if(cnt<50)cnt++;//500ms����һ�γ����ϱ�
	else 
	{
		cnt = 0;
		if(speed != CAR_Inf.Car_Speed)
		{
			speed = CAR_Inf.Car_Speed;
			CMD_8101(0);//�����ٶȣ���������
		}
	}
	
	
	if(CAR_Inf.NIGHTLIGHT != Inf_buff[0])//�������
	{
		Write_car(1,CAR_Inf.NIGHTLIGHT);
		Inf_buff[0] = CAR_Inf.NIGHTLIGHT;
	}
	
	if(CAR_Inf.DIVEDOOR != Inf_buff[2])//��ʻ����
	{
		Write_car(3,CAR_Inf.DIVEDOOR);
		Inf_buff[2] = CAR_Inf.DIVEDOOR;
	}
	
	if(CAR_Inf.ACC != Inf_buff[3])//ACC״̬
	{
//		
//		Write_car(4,CAR_Inf.ACC);//���ϱ�ACC״̬��ֻ����ʧ�ܿ���
		if(CAR_Inf.ACC)	{CMD_8106(0);Ignition_cnt++;}//��Ļʧ�ܿ���
		else			CMD_8106(1);//��Ļʧ�ܿ���
		Inf_buff[3] = CAR_Inf.ACC;
	}
	
	if(CAR_Inf.ReverseSta == 1)//����
	{
		car_sta = 2;
		Astern_Ctrl(ON);//����Ӳ���ź�
	}
	else //����ת��
	{
		Astern_Ctrl(OFF);//����Ӳ���ź�
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
	
	if(car_sta != Inf_buff[1])//�г�״̬
	{
		Inf_buff[1] = car_sta;
		Write_car(2,car_sta);
		LAROUND_Inf.VIS = car_sta;
#ifdef LOOK_AROUND
		LA_CMD_82(LAROUND_Inf.VIS);
#endif
		if(LAROUND_Inf.VIS == 2)
		{
			Astern_Ctrl(ON);//����Ӳ���ź�
		}
		else if((LAROUND_Inf.VIS == 3)||(LAROUND_Inf.VIS == 4))
		{
#ifdef LOOK_AROUND
				if(CAR_Inf.Car_Speed<(15<<8))
				{
					Astern_Ctrl(ON);//����Ӱ��
				}
				else
				{
					Astern_Ctrl(OFF);//�˳�Ӱ��
				}
#endif
		}
		else
		{
			Astern_Ctrl(OFF);//����Ӳ���ź�
		}
	}
	
	if(Check_TXSTA(5) == 0)//���Ϳ���ʱ���鿴���⡢����Ƶ���Ϣ�������񻺳壬��ӷ�������
	{
		if(Read_car(&num,&mode))
		{
			CMD_8103(num,mode);//����Ƶ��ڵ���Ϣ
//			printf("%d,%d;ok\r\n",num,mode);
		} 
	}
	
	settemp = (CAR_Set.OVERSPEE)+(CAR_Set.CARSEARCH<<1)+(CAR_Set.DRIVLOCK<<2)+(CAR_Set.PARKULOCK<<3)+(CAR_Set.DDWS<<4)+(CAR_Set.BSD<<5);	//�������ÿ���״̬
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
	
	if(RTC_IRQflag==1)//ICʱ��ͬ��
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

