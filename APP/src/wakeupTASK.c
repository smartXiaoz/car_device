#include "app.h"
#include "Hardware.h"
#include "EXTI.h"
#include "UartExp.h"

#define WAKETime	10
#define TASKMSG		5

//������
TaskHandle_t  wakeupTask_Handler;

//task1������
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



	//���ú�����������
void wakeupTASK_Create(u8 TASK_PRIO,u16 SIZE)
{
	xTaskCreate((TaskFunction_t )wakeup_task,           //������     
							(const char*    )"wakeup_task",         //������   
							(uint16_t       )SIZE,               //�����ջ��С	     
							(void*          )NULL,                  
							(UBaseType_t    )TASK_PRIO,          //�������ȼ�      
							(TaskHandle_t*  )&wakeupTask_Handler);  //������
}

void SYS_Dormancy(void)
{
	
	CAN_Ctrl(OFF);			//CAN����͹���
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
	CAN_Ctrl(ON);			//CAN�˳��͹���
	CAN_Ctrl(ON);
}

   
//ϵͳ�߼�
//
u8 up_cnt = 0;
void cl_upcnt(void)
{
	up_cnt = 0;
}

static u8 low_power_flag = 0;//��ѹ����
void SYS_Work(void);
void Rest_MPU(__START_Reason mode);
void Off_Time(void);
void Batter_check(void);

u8 SYS_Process(void)
{
	static u8 Cnt_T = 0;
	if(MCU_Awak == 0)//����ģʽ��
	{
			//����������𣬶ϵ磬����
			if(SYS_Inf.STA == 0)//����
			{
				if(Cnt_T<WAKETime)Cnt_T++;
				else
				{
					Cnt_T = 0;
//					printf("system sleep\r\n");
					Mute_Ctrl(ON);
					vTaskDelay(500);
					PAMP_Ctrl(OFF);			//��������
					Power_Ctrl(OFF);		//�ϵ�
					OUT_DISABLE();
					CAN_RXclean();
					SYS_Dormancy();
					OUT_ENABLE();
					
//					printf("system awak\r\n");
				}
				if(CAR_Inf.ACC == 1)//����������ACC�ϵ磬�˳������źŶ�ʧ���µ�����ʧ��
				{
#ifdef	SKY121
					if(SYS_Inf.ACC_AWAKE == 1)
					{
						SYS_Inf.ACC_AWAKE = 0;
						MCU_Awak = 1;
						ContTime = 1;						//�ػ�����ʱ��Ĭ��10���ӣ�ACC�ϵ紥����1����
					}
#else
					MCU_Awak = 1;
					ContTime = 1;						//�ػ�����ʱ��Ĭ��10���ӣ�ACC�ϵ紥����1����
#endif					
				}
				else
				{
					ContTime = OFFTIME;						//�ػ�����ʱ��Ĭ��10���ӣ�ACC�ϵ紥����1����	
				}
			}
			else//˯���ź�
			{
				Mute_Ctrl(ON);
				vTaskDelay(500);
				PAMP_Ctrl(OFF);			//��������
				
				Power_Ctrl(OFF);		//�ϵ�
				RADIO_Inf.WORK_STA = 0;	//��������
				//��Ҫ����д��EEPROM
				
				Cnt_T = WAKETime;	//ֱ�ӳ�ʱ������������(ʵ����100msϵͳ��ʱ)
				SYS_Inf.STA = 0;//ȥ����
				return 0;
			}
	}
	else	//����ģʽ
	{

#ifdef	SKY121
		Batter_check();
#endif		
		
		if(SYS_Inf.STA == 0)//�ջ���
		{
			
//			if()
			
			SYS_Inf.STA = 1;
			SYS_Inf.START_STA = 1;
			calendar.sta = 0;
			//�ϵ�
//			Power_Ctrl(ON);	//��Դʹ������

		}
		else if(SYS_Inf.STA == 1)//����ָ���
		{
			
			
			if(CAR_Inf.ACC)Rest_MPU(ACCON);					//MPU��λ������
			else Rest_MPU(PREBOOT);	
			
#ifdef TW2836
			LAROUND_Inf.STA = 1;				//���ӣ��ȴ�״̬
#else 
			LAROUND_Inf.STA = 0;				//���ӣ��ȴ�״̬
#endif	
			SYS_Inf.STA = 2;					//������������ģʽ
			
			if(CAR_Inf.ReverseSta == 1)Astern_Ctrl(ON);//����Ӱ��
			else	Astern_Ctrl(OFF);//����Ӱ��
		}
		else if(SYS_Inf.STA == 2)//����������
		{
			SYS_Work();
			if(SYS_Inf.BATTER_VERR==0)Data_Save();//��ѹ�����Ժ�EEPROM�ϵ� ������д������
			
		}
		else if(SYS_Inf.STA == 3)//����ģʽ��
		{
			if(up_cnt<60)up_cnt++;//����ģʽ����6�룬6�������������˳�����ģʽ
			else 
			{
				up_cnt = 0;		
				SYS_Inf.STA = 2;				
				CMD_8503(1);//�˳������ͽ���ʧ��
				return 0;
			}
			
			if(UP_Data.STA == 1)//�ս�������ģʽ��׼�����Ժ�������������������
			{
				CMD_8501();//��ʼ�������ݴ���
				UP_Data.STA = 2;
			}
			else if(UP_Data.STA == 3)//������ɣ�����������
			{
				CMD_8503(0);//���ͽ��ճɹ�
				vTaskDelay(1000);
				Mute_Ctrl(ON);
				vTaskDelay(1000);
				PAMP_Ctrl(OFF);	//����ʹ��
				vTaskDelay(1000);
				IWDG_Init(4,625);//�������ȸ�λ����
				while(1)
				{
					vTaskDelay(1000);
					
				}
			}
			else if(UP_Data.STA == 4)//����ʧ�ܣ��˳�
			{
				CMD_8503(1);//���ͽ���ʧ��
				SYS_Inf.STA =2;//����������ģʽ
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
	//���õ�ѹ����
	if(CAR_Inf.Batter_Value>3600)//��ѹ����
	{
		if(batter_cnt2<100)batter_cnt2++;
		else
		{
			SYS_Inf.BATTER_VERR = 1;

			Mute_Ctrl(ON);			//���Ž���
			vTaskDelay(500);
			PAMP_Ctrl(OFF);			//��������
			Power_Ctrl(OFF);		//�ϵ�
			RADIO_Inf.WORK_STA = 0;	//��������
			SYS_Inf.MPU_STA = 4;	//MPU�ػ�
		}
		batter_cnt1 = 0;
		batter_cnt3 = 0;
	}
	else if(CAR_Inf.Batter_Value<890)//Ƿѹ����
	{
		if(batter_cnt1<150)batter_cnt1++;
		else
		{
			SYS_Inf.BATTER_VERR = 2;
			
			Mute_Ctrl(ON);			//���Ž���
			vTaskDelay(500);
			PAMP_Ctrl(OFF);			//��������
			Power_Ctrl(OFF);		//�ϵ�
			RADIO_Inf.WORK_STA = 0;	//��������
			SYS_Inf.MPU_STA = 4;	//MPU�ػ�
		}
		batter_cnt2 = 0;
		batter_cnt3 = 0;
	}
	else if((CAR_Inf.Batter_Value>920)&&(CAR_Inf.Batter_Value<3550))//��ѹOK
	{
		if(SYS_Inf.BATTER_VERR != 0)//��ѹ�ָ�
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

	
	
	if(CAR_Inf.ACCDe == 0)//ACC ��⣬���絹��ʱ
	{
		if(ContTime>0)
		{
			if(min_cnt<600)min_cnt++;
			else 
			{
				min_cnt = 0;
				ContTime--;
//				CMD_8003();//�ϱ�����ʱ
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
				CMD_8003();//�ϱ�����ʱ
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
		case 0://�����ȴ�����,��ʱ������
			MPU_REST_FLAG++;
			if(MPU_REST_FLAG>=1800)
			{
				MPU_REST_FLAG = 0;
				Rest_MPU(OTRESET);
			}
		break;
		
		case 1://MPU��������
			MPU_REST_FLAG = 0;
		break;
		
		case 2://����ػ�
			MCU_Awak = 0;
		break;
		
		case 3://��������
		break;
		
		case 4://�ػ�״̬
		break;
		
		case 5://MPU���Ͽ���������MPU
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
			if(LOA_WAIT>=50)//�ȴ�30S����ʱΪ����
			{
				LAROUND_Inf.STA = 2;//��ǹ���
				FAULT_Inf.LOA = 1;	//���DTC
			}
		}
	}
#endif
#endif
}


void Rest_MPU(__START_Reason mode)
{
	u8 Temp[2]={0};
	
	if((mode == STRESET)||(mode == OTRESET))//��ʱ��λ�����ø�λ��Ҫ�ϵ���ʱ
	{
		
		Mute_Ctrl(ON);
		vTaskDelay(500);
		PAMP_Ctrl(OFF);	//����ʹ��
		vTaskDelay(500);		
		
		Power_Ctrl(OFF);
		if(mode == OTRESET)//��ʱ��������¼����
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

	PAMP_Ctrl(ON);	//����ʹ��
	MPUReset_Ctrl(OFF);
		
//	RADIO_Inf.MODE = 0;					//Ĭ��������ģʽΪFM״̬��		
//	RADIO_Inf.FM_FRQ = Frequency[0];	//FM_FRQ����ΪĬ��ֵ
//	RADIO_Inf.AM_FRQ = Frequency[2];	//AM_FRQ����ΪĬ��ֵ
		
	RADIO_Inf.WORK_STA = 3;				//����������ΪԤ����״̬
	clean_TXtask();						//��մ������з�������
	SYS_Inf.MPU_STA = 0;				//MPU�ȴ�����ģʽ
	MPU_REST_FLAG = 0;					//MPU��ʱ��ʱ
	LOA_WAIT = 0;						//���ӳ�ʱ��ʱ
	START_Reason = mode;				//����ԭ��
	
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
		SYS_Config.LOC = 1;//���䣬ǿ��Ϊ1
	
#ifdef LOOK_AROUND
		SYS_Config.LOA = 2;
#endif	
		
	
	}
	else//EEPROM����Ĭ��
	{
		SYS_Config.DDWS = 0;
		SYS_Config.BSD = 0;
		SYS_Config.LOC = 1;//���䣬ǿ��Ϊ1
		SYS_Config.LOA = 1;
#ifdef LOOK_AROUND
		SYS_Config.LOA = 2;
#endif
	}
	
//		SYS_Config.DDWS = 1;
//		SYS_Config.BSD = 1;
	
//	printf("\r\nconfig= 0x%x,0x%x;\r\n\r\n",AT24CXX_ReadOneByte(EEPROM_ADD+17+14),AT24CXX_ReadOneByte(EEPROM_ADD+17+14+1));
		
}  






extern u8 CMD_8009(u16 Dat);//ʹ��ʱ��
extern u8 CMD_800A(u8 Dat);//һ���ӵ���ʱ
#define	STARTTIME	20
#define	GAPTIME		20
#define MIN_CNT		600
static u32 use_time = 0;//ʹ�ü�ʱ
static u16 off_time = 0;//1���Ӽ�ʱ
static u8 last_mode = 0;
void Batter_check(void)
{
	static u8 batter_cnt1 = 0;
	static u8 batter_cnt2 = 0;
	static u8 batter_cnt3 = 0;
	
	if(CAR_Inf.Batter_Value <= 1190)
	{
		if(batter_cnt1<50) batter_cnt1++;
		else SYS_Inf.Batter_sta	= 2;//��ѹ����	
		batter_cnt2 = 0;
		batter_cnt3 = 0;
	}
	else if((CAR_Inf.Batter_Value>=1210)&&(CAR_Inf.Batter_Value<=1290))
	{
		if(batter_cnt2<20) batter_cnt2++;
		else SYS_Inf.Batter_sta	= 1;//Ƿѹ����
		batter_cnt1 = 0;
        batter_cnt3 = 0;
	}
	else if(CAR_Inf.Batter_Value>=1310)
	{
		if(batter_cnt3<10) batter_cnt3++;
		else SYS_Inf.Batter_sta	= 0;//��ѹ����
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
		if(off_time == 1)//��ѹ����
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
			MCU_Awak = 0;//�ػ�����
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
				if(((use_time-(STARTTIME*MIN_CNT))%(GAPTIME*MIN_CNT)) == 0)//ʹ�����ѣ���15���ӿ�ʼ��ÿ��10���ӣ�
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









