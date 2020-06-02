#include "app.h"
#include "hardware.h"
#include "CanIf/uds_can_driver.h"
#include "information.h"

#define IC_CYCLE	250
#define BCM_CYCLE	100
#define ABS_CYCLE	100



static u8 BLUEtx_sta=0;	//����״̬
static u8 PHONEtx_sta=0;

//������
TaskHandle_t  CanTask_Handler;

Can_SF MPU2CAN[8];

//task1������
void MPU2CAN_Process(void);
static u8 Can_Process(void);
void FAULT_Process(void);
static void can_task(void *pvParameters)
{
	u32 lastWakeTime = xTaskGetTickCount();
	while(1)
	{
		MPU2CAN_Process();//MPU��CAN����ת��
		Can_Process();
		FAULT_Process();
//		vTaskDelay(10);
		vTaskDelayUntil(&lastWakeTime,10);
	}
}

//���ú�����������
void CanTASK_Create(u8 TASK_PRIO,u16 SIZE)
{
	xTaskCreate((TaskFunction_t )can_task,           //������     
				(const char*    )"can_task",         //������   
				(uint16_t       )SIZE,               //�����ջ��С	     
				(void*          )NULL,                  
				(UBaseType_t    )TASK_PRIO,          //�������ȼ�      
				(TaskHandle_t*  )&CanTask_Handler);  //������
}

u8 write_ip = 0;
u8 read_ip = 0;
void MPU2CAN_Write(Can_SF tx)
{
	MPU2CAN[write_ip++] = tx;
	if(write_ip>=8)write_ip = 0;
}
u8 MPU2CAN_Read(Can_SF *rx)
{
	if(write_ip == read_ip)return 0;
	*rx = MPU2CAN[read_ip++];
	if(read_ip>=8)read_ip = 0;
	return 1;
}

u8 CMD_8630(u32 ID,u8 result);
void MPU2CAN_Process(void)
{
	Can_SF temp;
	if(MPU2CAN_Read(&temp))
	{	
		if(CAR_Inf.ACC == 0)CMD_8630(temp.id,1);//ACC�µ磬����ʧ��
		if(Signal_Control.send)CMD_8630(temp.id,1);//�ѹرշ��ͣ�����ʧ��
		else if(FAULT_Inf.BUSOFF)CMD_8630(temp.id,1);//���߹��ϣ�����ʧ��
		else
		{
			if(CAN_Send_Msg(temp))CMD_8630(temp.id,1);//����ʧ��
			else CMD_8630(temp.id,0);//���ͳɹ�			
		}

	}
}

//�����������
#define	DTC_CNT	12
#define CLEAN_CYCLE	40
typedef struct {
u8 flag;
u32 cnt;	
}_DTC_RECORD;
_DTC_RECORD DTC_RECORD[DTC_CNT];

//��¼ʱ�����������
void DTC_Happen(u8 num)
{
	DTC_RECORD[num].flag = 1;
	DTC_RECORD[num].cnt = Ignition_cnt;
}


extern void DTC_Clean(u8 num);
//��������� Ignition_cnt++��������ã����ڼ��40���ڵ�û��
void DTC_process(void)
{
	for(u8 i=0;i<DTC_CNT;i++)
	{
		if(DTC_RECORD[i].flag == 1)
		{
			if((Ignition_cnt-DTC_RECORD[i].cnt)>=CLEAN_CYCLE)
			{
				DTC_Clean(i);
				
				DTC_RECORD[i].flag = 0;				
			}
		}
	}
}



#define CAN_CNT 3
u8 CAN_EXP(u32 ID,u8 *Dat);
u8 busy=0;
Can_SF	RX_MSG;
Can_SF	TX_MSG;
void TX_Vsorce(void);
void TX_RTCtime(void);
void TX_BLUEinfo(void);
void TX_PHONEinfo(void);
void BUS_OFF(void);

static u8 busoff_cnt = 5;//50ms
static u8 busoff_tim = 0;

void BUS_OFF(void)
{
	if(FAULT_Inf.BUSOFF)//Bus-Off
	{
		if(busoff_cnt !=0)
		{
			busoff_cnt--;
			CAN_Mode_Init(1,8,9,8,0);//3?��??��CAN
		}
		else	
		{
			busoff_tim++;
			if(busoff_tim>=100)//1s
			{
				busoff_tim =0;
				CAN_Mode_Init(1,8,9,8,0);//3?��??��CAN
			}
		}
	}
	else
	{
		busoff_tim = 0;
		busoff_cnt = 5;
	}
}

static u8 ct_cnt = 0;
//CAN��ʱ�����������ݷ���
static u8 ct1_cnt = 1;
static u8 ct2_cnt = 2;
static u8 ct3_cnt = 3;
static u8 ct4_cnt = 4;
static u8 powererr_cnt1 = 0;
static u8 powererr_cnt2 = 0;
static u8 Can_Process(void)
{

	calendar.flag++;//ʱ���־
	
	//�������ݴ���
	if(Can_Get_RXMsg(&RX_MSG))	CAN_EXP(RX_MSG.id,RX_MSG.data);
	
	BUS_OFF();
	
	//�������ݷ���
	if(busy == 0)	//���л�ȡ
	{
		u8 prio = 0;
		prio = Can_Get_TXMsg(&TX_MSG);
		if(1==prio) 
		{
			CAN_Send_Msg(TX_MSG);
		}
		else if(2==prio)
		{
			if(Signal_Control.send == 0)//���Ϳ����ŷ���
			{
				
				if((CAR_Inf.ACC == 1)&&(PowerNormal == 1))//ϵͳ����������
					busy = CAN_CNT;
			}
		}
		
	}
	else	//����
	{
		if((Signal_Control.send==0)&&(CAR_Inf.ACC == 1)&&(PowerNormal == 1))
		{
			if(ct_cnt<2)ct_cnt++;
			else
			{
				ct_cnt = 0;
				CAN_Send_Msg(TX_MSG);
				busy--;			
			}
		}
		else
		{
			busy = 0;
		}
	}
	
	if((Signal_Control.send)||(CAR_Inf.ACC != 1)) 
	{
		
		ct1_cnt = 1;//���õ�������ʱ�������ڻָ�����ʱ���ٷ���
		ct2_cnt = 2;
		ct3_cnt = 3;
		ct4_cnt = 4;
		
		PHONE_Inf.sta = 0;
		PHONEtx_sta = 0;
		BLUEtx_sta = 0;
		
		return 0;//���͹رգ�������
	}
//	if((SYS_Inf.STA != 1)&&(SYS_Inf.STA != 2)) return 0;//ϵͳû����������������
//	if() return 0;//ACC�µ磬����������
	if(PowerNormal != 1)//��ѹ��������
	{
		if(powererr_cnt2>0)powererr_cnt2--;//200ms�ڻ����Է��ͣ�
		else//200ms�Ժ�رշ��͹���
		{
			powererr_cnt1 = 20;
			ct1_cnt = 1;//���õ�������ʱ�������ڻָ�����ʱ���ٷ���
			ct2_cnt = 2;
			ct3_cnt = 3;
			ct4_cnt = 4;
			
			PHONE_Inf.sta = 0;
			PHONEtx_sta = 0;
			BLUEtx_sta = 0;
			
			return 0;			
		}
	}
	if(powererr_cnt1>0)//��ѹ�ָ��������ӳ�200ms���б��ķ���
	{
		powererr_cnt1--;
		powererr_cnt2 = 20;
		return 0;
	}
	
	
	if(ct1_cnt>0)ct1_cnt--;//1000MSʱ��ͬ��
	else
	{
		ct1_cnt = 99;
		TX_RTCtime();
	}
	if(ct2_cnt>0)ct2_cnt--;//200MS��Դͬ��
	else
	{
		ct2_cnt = 19;
		TX_Vsorce();
	}
	if(ct3_cnt>0)ct3_cnt--;//1000MS�����豸��ͬ��
	else
	{
		ct3_cnt = BLUE_Inf.time;
		TX_BLUEinfo();
	}
	if(ct4_cnt>0)ct4_cnt--;//ͨ����Ϣͬ��
	else
	{
		ct4_cnt = PHONE_Inf.time;
		TX_PHONEinfo();
	}
	
	return 0;
}

static u16 missIC_cnt = 0;
static u8  missIC_er = 0;
static u16 missBCM_cnt = 0;
static u8  missBCM_er = 0;
static u16 missABS_cnt = 0;
static u8  missABS_er = 0;

void FAULT_Process(void)
{

	if(Signal_Control.receiver)return;//�ѹرս��գ�
	if((FAULT_Inf.BUSOFF)||(SYS_Inf.STA != 2))//  ���߹���||ϵͳû����
	{
		missBCM_cnt = 0;
		missBCM_er = 0;
		
		missIC_cnt = 0;
		missIC_er = 0;
		return;
	}
	
	
	if(missBCM_cnt<(BCM_CYCLE/10+1))missBCM_cnt++;
	else
	{
		missBCM_cnt = 0;
		if(missBCM_er<3)missBCM_er++;
		else	
		{
			FAULT_Inf.MISSBCM = 1;
			
			CAR_Inf.LTurnSts = CAR_Inf.LTurnSts;//�źŶ�ʧ����ת��ƴ���
			CAR_Inf.RTurnSts = CAR_Inf.RTurnSts;//�źŶ�ʧ����ת��ƴ���
		}
	}
	if(missIC_cnt<(IC_CYCLE/10+1))missIC_cnt++;
	else
	{
		missIC_cnt = 0;
		if(missIC_er<3)missIC_er++;
		else	
		{
			FAULT_Inf.MISSIC = 1;
			CAR_Inf.ReverseSta = CAR_Inf.ReverseSta;//�źŶ�ʧ�������ƴ���
		}
	}
	if(missABS_cnt<(ABS_CYCLE/10+1))missABS_cnt++;
	else
	{
		missABS_cnt = 0;
		if(missABS_er<3)missABS_er++;
		else	
		{
			FAULT_Inf.MISSABS = 1;
			CAR_Inf.Car_Speed = CAR_Inf.Car_Speed;//�źŶ�ʧ�����ٴ���
			Speed_flag = 0;
		}
	}
}
	
	
static u8 Speed_Get(u8 *Dat);
static u8 TurnLight_Get(u8 *Dat);
static u8 CarSet_Get1(u8 *Dat);
static u8 Power_Get(u8 *Dat);
static u8 KEYSTS_Get(u8 *Dat);
static u8 CarSet_Get2(u8 *Dat);
static u8 CarSet_Get3(u8 *Dat);
static u8 CarSet_Get4(u8 *Dat);
static u8 CarSet_Get5(u8 *Dat);

u8 CMD_8631(u32 ID,u8 *dat);
u8 CAN_EXP(u32 ID,u8 *Dat)
{
	Can_SF	can_temp;
	u8 i = 0;
	
	if(FACTOR_Inf.sta == 1)CMD_8631(ID,Dat);//ȫ��ת����MPU
	
	if(Signal_Control.receiver)//�ѹرս��գ�������ϵ�ַ������ȫ��������
	{
		if((ID == 0x18DA76FA)||(ID == 0x18DBFFFA)||(ID == 0x18DAFA76))
		{
			//���ID��
		}
		else return 0;//����ID��������
	}
	
	switch(ID)
	{
		case 0x18FEBF0B:Speed_Get(Dat);
			missABS_cnt = 0;
			missABS_er = 0;
			FAULT_Inf.MISSABS = 0;
			break;		//����Get
		case 0x10FF1021:TurnLight_Get(Dat);break;	//ת���Get
		case 0x18FF6FEE:CarSet_Get1(Dat);break;		//Ѱ���������г�������Ϩ�����Get
		case 0x08FF00DD:
			Power_Get(Dat);
			missBCM_cnt = 0;
			missBCM_er = 0;
			FAULT_Inf.MISSBCM = 0;
			break;		//������Դ��λGet
		case 0x10FF7521:KEYSTS_Get(Dat);break;		//Կ��״̬Get
		case 0x10FF4117:
			CarSet_Get2(Dat);
			missIC_cnt = 0;
			missIC_er = 0;
			FAULT_Inf.MISSIC = 0;
			break;									//������Get
		case 0x18A005E7:CarSet_Get3(Dat);break;		//ƣ�ͼ�ʻ����״̬Get
		case 0x1CA00701:CarSet_Get4(Dat);break;		//BSD��������״̬Get
		case 0x18F00503:CarSet_Get5(Dat);break;		//��λ״̬Get
		
		case 0x18DA76FA:
			can_temp.id = 0x18DA76FA;
			for(i = 0;i < 8; i++)can_temp.data[i] = Dat[i];
			CanIf_Received(can_temp);
			break;
		case 0x18DBFFFA:
			can_temp.id = 0x18DBFFFA;
			for(i = 0;i < 8; i++)can_temp.data[i] = Dat[i];
			CanIf_Received(can_temp);
		break;
		
		
		
		//�����ض�IDת��
		
	}
	return 0;
}

static u8 Speed_Get(u8 *Dat)
{
	CAR_Inf.Car_Speed = (u16)((Dat[1]<<8) + Dat[0]);	//����
	Speed_flag = CAR_Inf.Car_Speed;
	return 0;
}

static u8 TurnLight_Get(u8 *Dat)
{
	static u8 cnt_L = 0;
	static u8 cnt_R = 0;
	//ת��ƣ���Ҫ��ʱ����CAN�ź�Ϊ��˸
	
	if(((Dat[3]>>4)&0x03)==1)CAR_Inf.LTurnSts = 1,cnt_L=0;
	else
	{
		if(cnt_L<55)cnt_L++;
		else CAR_Inf.LTurnSts = 0;
	}
	
	if(((Dat[3]>>2)&0x03)==1)CAR_Inf.RTurnSts = 1,cnt_R=0;
	else
	{
		if(cnt_R<55)cnt_R++;
		else CAR_Inf.RTurnSts = 0;
	}
	
	
	return 0;
}

static u8 CarSet_Get1(u8 *Dat)
{
	CAR_Set.CARSEARCH = (u8)(Dat[0]&0x04)?1:0;	//Ѱ����������
	CAR_Set.DRIVLOCK = (u8)(Dat[0]&0x08)?1:0;	//�г���������
	CAR_Set.PARKULOCK = (u8)(Dat[0]&0x10)?1:0;	//Ϩ���������
	return 0;
}

static u8 Power_Get(u8 *Dat)
{
	CAR_Inf.Lock = (u8)(Dat[0]&0x0F);			//ң�ؽ������
	
	CAR_Inf.POWER = (u8)((Dat[2]>>2)&0x03);		//������Դ��λ
//	if(CAR_Inf.POWER != 0) CAR_Inf.ACC = 1;		//�и�Ӳ���ź�
//	else CAR_Inf.ACC = 0;
	
	if(1 == CAR_Inf.Lock)//���Ž���
	{
		MCU_Awak = 1;
	}
	else if(2 == CAR_Inf.Lock)//��������
	{
		MCU_Awak = 0;
	}
	
	return 0;
}


static u8 KEYSTS_Get(u8 *Dat)
{
	CAR_Inf.KEY_STS = (u8)(Dat[3]&0x0F);		//Կ��״̬
	
	//�ſ����ź�
	CAR_Inf.DIVEDOOR = (u8)((Dat[2]>>4)&0x03);		//��ʻ�೵��
	CAR_Inf.PSNGDOOR = (u8)((Dat[2]>>6)&0x03);		//����ʻ�೵��

#ifdef	DORAWAK
	if(CAR_Inf.DIVEDOOR == 1)//��ʻ�࿪������
	{
		MCU_Awak = 1;
	}
#endif
	
	return 0;
}

static u8 CarSet_Get2(u8 *Dat)
{
	CAR_Inf.ReverseSta = (u8)((Dat[0]>>2)&0x03);//������״̬
	CAR_Set.OVERSPEE = (u8)(Dat[2]&0x03);		//���ٱ�������
	return 0;
}

static u8 CarSet_Get3(u8 *Dat)
{
	CAR_Set.DDWS = (u8)((Dat[0]>>5)&0x03);		//ƣ�ͼ�ʻ����
	return 0;
}  

static u8 CarSet_Get4(u8 *Dat)
{
	CAR_Set.BSD = (u8)(Dat[0]&0x03);			//ä���⿪��
	return 0;
}

static u8 CarSet_Get5(u8 *Dat)
{
//	u8 temp = 0;
//	temp = (u8)(Dat[7]);
//	if(temp == 'R')	CAR_Inf.ReverseSta = 1;		//��λ״̬
//	else CAR_Inf.ReverseSta = 0;				//��λ״̬
	return 0;
}



//TX_Information

void TX_RTCtime(void)
{
	Can_SF	tx;
	
	tx.id = 0X18FEE6EE;
	tx.data[0] = calendar.sec*4+calendar.flag/13;
	tx.data[1] = calendar.min;
	tx.data[2] = calendar.hour;
	tx.data[3] = calendar.w_month;
	tx.data[4] = calendar.w_date*4+calendar.hour/6;
	tx.data[5] = calendar.w_year-1985;
	tx.data[6] = 0xFF;
	tx.data[7] = 0xFF;
	
	CAN_Send_Msg(tx);
}


void TX_Vsorce(void)
{
	Can_SF	tx;
	
	if(RADIO_Inf.MODE)
	VOICE_Inf.radiofrq = RADIO_Inf.AM_FRQ;
	else VOICE_Inf.radiofrq = RADIO_Inf.FM_FRQ*100;
	
	tx.id = 0X18FFB676;//VOICE_Inf
	tx.data[0] = VOICE_Inf.source;
	tx.data[1] = VOICE_Inf.playsta;
	tx.data[2] = (u8)(VOICE_Inf.radiofrq&0xFF);
	tx.data[3] = (u8)((VOICE_Inf.radiofrq>>8)&0xFF);
	tx.data[4] = (u8)((VOICE_Inf.radiofrq>>16)&0xFF);
	tx.data[5] = VOICE_Inf.volume|(VOICE_Inf.mutesta<<7);
	tx.data[6] = VOICE_Inf.phonesta;
	tx.data[7] = 0xFF;
	CAN_Send_Msg(tx);
}




void TX_BLUEinfo(void)
{
	Can_SF	tx;
	static u8 len = 0;
	static u8 dmsg = 0;
	static u8 fn = 0;
	u8 indx = 0;
	u8 i = 0;
	
	if(BLUE_Inf.sta)//���
	{
		if(PHONEtx_sta!=0)
		{
			BLUE_Inf.time = 1;
			return;
		}
		if(BLUEtx_sta==0)//�㲥ͷ
		{
			BLUEtx_sta = 1;
			BLUE_Inf.time = 1;
			tx.id = 0x18ECFF76;//�㲥��ַ
			dmsg = BLUE_Inf.name_len/7;
			dmsg+= (((BLUE_Inf.name_len%7)>0)?1:0);
			len = 0;
			fn = 1;
			tx.data[0] = 32;//������
			tx.data[1] = (u8)((BLUE_Inf.name_len>>0)&0xFF);//�ֽ���
			tx.data[2] = (u8)((BLUE_Inf.name_len>>8)&0xFF);
			tx.data[3] = dmsg;//����
//			tx.data[4] = 0xFF;
//			tx.data[5] = (u8)((0xFF62>>0)&0xFF);//������PGN
//			tx.data[6] = (u8)((0xFF62>>8)&0xFF);
//			tx.data[7] = (u8)((0xFF62>>16)&0xFF);
			tx.data[4] = (u8)((0xFF62>>0)&0xFF);//������PGN0
			tx.data[5] = (u8)((0xFF62>>8)&0xFF);
			tx.data[6] = (u8)((0xFF62>>16)&0xFF);
			tx.data[7] = (u8)((0xFF62>>24)&0xFF);
			CAN_Send_Msg(tx);
		}
		else if(BLUEtx_sta==1)//���ݰ�
		{
			tx.id = 0x18FF6276;//����
			tx.data[0] = fn++;//����
			if((BLUE_Inf.name_len-len)>7)indx = 7;
			else
			{
				indx = BLUE_Inf.name_len-len;
				BLUEtx_sta = 0;
				
				BLUE_Inf.time=99;
			}
			for(i=0;i<indx;i++)
			tx.data[1+i]=BLUE_Inf.name[len++];
			for(;i<7;i++)
			tx.data[1+i]=0xFF;
			CAN_Send_Msg(tx);
		}
	}
	else//��֡
	{
		BLUEtx_sta = 0;
		BLUE_Inf.time=99;
		tx.id = 0x18FF6276;
		for(u8 i=0;i<8;i++)
		tx.data[i] = 0xFF;
		CAN_Send_Msg(tx);
	}
}


void TX_PHONEinfo(void)
{
	Can_SF	tx;
//	static u8 tx_sta=0;
	static u8 len = 0;
	static u8 dmsg = 0;
	static u8 fn = 0;
	u8 indx = 0;
	u8 i = 0;
	
	if(BLUEtx_sta!=0)
	{
		PHONE_Inf.time = 1;
		return;
	}
	
	if(PHONE_Inf.sta == 0)PHONE_Inf.time = 1;
	else
	{
		if(PHONEtx_sta==0)//�㲥ͷ
		{
			PHONEtx_sta = 1;
			tx.id = 0x18ECFF76;//�㲥��ַ
			dmsg = PHONE_Inf.num_len/7;
			dmsg+= (((PHONE_Inf.num_len%7)>0)?1:0);
			len = 0;
			fn = dmsg;
			tx.data[0] = 32;//������
			tx.data[1] = (u8)((PHONE_Inf.num_len>>0)&0xFF);//�ֽ���
			tx.data[2] = (u8)((PHONE_Inf.num_len>>8)&0xFF);
			tx.data[3] = fn;//����
//			tx.data[4] = 0xFF;
//			tx.data[5] = (u8)((0xFF63>>0)&0xFF);//������PGN
//			tx.data[6] = (u8)((0xFF63>>8)&0xFF);
//			tx.data[7] = (u8)((0xFF63>>16)&0xFF);
			tx.data[4] = (u8)((0xFF63>>0)&0xFF);//������PGN0
			tx.data[5] = (u8)((0xFF63>>8)&0xFF);
			tx.data[6] = (u8)((0xFF63>>16)&0xFF);
			tx.data[7] = (u8)((0xFF63>>24)&0xFF);
			
			CAN_Send_Msg(tx);
		}
		else if(PHONEtx_sta==1)//����
		{
			tx.id = 0x18FF6376;//����
			tx.data[0] = fn++;//����
			if((PHONE_Inf.num_len-len)>7)indx = 7;
			else 
			{
				indx = PHONE_Inf.num_len-len;
				PHONEtx_sta = 2;
			}
			for(i=0;i<indx;i++)
			tx.data[1+i]=PHONE_Inf.number[len++];
			for(;i<7;i++)
			tx.data[1+i]=0xFF;
			CAN_Send_Msg(tx);
		}
		else if(PHONEtx_sta==2)//�㲥ͷ
		{
			PHONEtx_sta = 3;
			tx.id = 0x18ECFF76;//�㲥��ַ
			dmsg = PHONE_Inf.name_len/7;
			dmsg+= (((PHONE_Inf.name_len%7)>0)?1:0);
			len = 0;
			fn = dmsg;
			tx.data[0] = 32;//������
			tx.data[1] = (u8)((PHONE_Inf.name_len>>0)&0xFF);//�ֽ���
			tx.data[2] = (u8)((PHONE_Inf.name_len>>8)&0xFF);
			tx.data[3] = fn;//����
//			tx.data[4] = 0xFF;
//			tx.data[5] = (u8)((0xFF64>>0)&0xFF);//������PGN
//			tx.data[6] = (u8)((0xFF64>>8)&0xFF);
//			tx.data[7] = (u8)((0xFF64>>16)&0xFF);
			tx.data[4] = (u8)((0xFF64>>0)&0xFF);//������PGN0
			tx.data[5] = (u8)((0xFF64>>8)&0xFF);
			tx.data[6] = (u8)((0xFF64>>16)&0xFF);
			tx.data[7] = (u8)((0xFF64>>24)&0xFF);
			CAN_Send_Msg(tx);
		}
		else if(PHONEtx_sta==3)//����
		{
			tx.id = 0x18FF6476;//����
			tx.data[0] = fn++;//����
			if((PHONE_Inf.name_len-len)>7)indx = 7;
			else 
			{
				indx = PHONE_Inf.name_len-len;
				PHONEtx_sta = 0;
				PHONE_Inf.sta --;
				PHONE_Inf.time = 9;
			}
			for(i=0;i<indx;i++)
			tx.data[1+i]=PHONE_Inf.name[len++];
			for(;i<7;i++)
			tx.data[1+i]=0xFF;
			CAN_Send_Msg(tx);
		}
	}

}



