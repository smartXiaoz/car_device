
#include "hardware.h"
#include "UartExp.h"
#include "LAroundExp.h"

//��������

#define MUTECHECK	1


#define CheckSun	0		//��У��ͼ��
#define CheckFN		0		//�����غż��


#define UART_SYNC	0x6AA6			//ͬ��ͷ
#define TX_oTime	125				//�ȴ��ط�ʱ��
#define TX_Cnt		2				//���ʹ���
#define DataSend	Uart3_TxBuff	//���巢�ʹ����뺯��

#define Frame_Cap	36		//��Ϣ��
//֡���
#define CMD8000	0	//ʱ�䡢����	
#define CMD8003	1	//�ػ�����ʱ
#define CMD8005	2	//����ԭ��
#define CMD8101	3	//����	
#define CMD8102	4	//����
#define CMD8103	5	//���⡢����Ƶ���Ϣ
#define CMD8104	6	//���⡢����Ƶ���Ϣ
#define CMD8201	7	//�������ÿ���״̬
#define CMD8301	8	//��������Ϣ	

#define CMD8500	9	//��������ظ�
#define CMD8501	10	//�������ݰ�����
#define CMD8502	11	//���ݰ��ظ�
#define CMD8503	12	//���ݴ�����

#define CMD8100 13	//����
#define CMD8200 14	//������Ϣ

#define CMD8105 15	//MUTE״̬�ظ�
#define CMD8106 16	//��Ļʧ�ܿ���


#define	CMD8600	17	//�������ģʽ����ظ�
#define	CMD8610	18	//EEPROM���Թ��ظ�
#define	CMD8621	19	//̨ͣ�����Ȼظ�
#define	CMD8622	20	//����Ȼظ�
#define	CMD8623	21	//Ƶ����Ӧ�ظ�
#define	CMD8630	22	//CAN���ݷ��ͽ���ظ�
#define	CMD8631	23	//CAN���ݽ���
#define	CMD8640	24	//���ӱ궨���
#define	CMD8650	25	//CAN���ݷ��ͽ���ظ�
#define	CMD8651	26	//CAN���ݽ���ת��
#define	CMD8660	27	//�汾���ϴ�
#define	CMD8670	28	//�Զ�����������ת��
#define	CMD8680	29	//CAN��λ������
#define	CMD8009	30	//ʹ��ʱ������
#define	CMD800A	31	//1��������
#define CMD8108 32	//�汾��

#define CMD8304 33	//RDS

#define CMD8661 34	//�������汾��

#define CMD810A 35	//Ӱ��״̬�ϱ�

#define ACKCHCELEN	8

typedef struct{
 u8  data[11];
} ACK_SF;

//static u8 ACK_Buff[11]={10,0x6A,0xA6,0,0,0,0,0,0,0,0};	//Ӧ��֡,��һ���ֽڴ��֡����

ACK_SF ACK_Chce[ACKCHCELEN]={0};
u8 ack_writ_p = 0;
u8 ack_read_p = 0;

static u8  Frame_Buff[Frame_Cap][64]={
	//֡���ݻ���,��һ���ֽڴ��֡���ȣ�ÿһ֡λ�ñ�����֡��Ŷ�Ӧ
	{18, 0x6A,0xA6, 1, 0,0, 0,10, 0x80,0x00, 0,0,0,0,0,0,0},	//0  CMD 80 00  ʱ�䡢����			2+8 =10
	{11, 0x6A,0xA6, 1, 0,0, 0,3,  0x80,0x03, 0,0,0,0,0,0,0},	//1  CMD 80 03  �ػ�����ʱ			2+1 =3
	{11, 0x6A,0xA6, 1, 0,0, 0,3,  0x80,0x05, 0,0,0,0,0,0,0},	//2  CMD 80 05  ����ԭ��			2+1 =3
	{12, 0x6A,0xA6, 0, 0,0, 0,4,  0x81,0x01, 0,0,0,0,0,0,0},	//3  CMD 81 01  ����				2+2 =4
	{12, 0x6A,0xA6, 1, 0,0, 0,4,  0x81,0x02, 0,0,0,0,0,0,0},	//4  CMD 81 02  ����				2+2 =4
	{12, 0x6A,0xA6, 1, 0,0, 0,4,  0x81,0x03, 0,0,0,0,0,0,0},	//5  CMD 81 03  ���⡢����Ƶ���Ϣ	2+2 =4
	{11, 0x6A,0xA6, 1, 0,0, 0,3,  0x81,0x04, 0,0,0,0,0,0,0},	//6  CMD 81 04  ����״̬			2+1 =3
	{11, 0x6A,0xA6, 1, 0,0, 0,3,  0x82,0x01, 0,0,0,0,0,0,0},	//7  CMD 82 01  �������ÿ���״̬	2+1 =3
	{14, 0x6A,0xA6, 1, 0,0, 0,6,  0x83,0x01, 0,0,0,0,0,0,0},	//8  CMD 83 01  ��������Ϣ			2+4 =6
	{11, 0x6A,0xA6, 0, 0,0, 0,3,  0x85,0x00, 0,0,0,0,0,0,0},	//9  CMD 85 00  ��������ظ�		2+1 =3
	{10, 0x6A,0xA6, 0, 0,0, 0,2,  0x85,0x01, 0,0,0,0,0,0,0},	//10 CMD 85 01  ������������		2+0 =2
	{13, 0x6A,0xA6, 0, 0,0, 0,5,  0x85,0x02, 0,0,0,0,0,0,0},	//11 CMD 85 02  ��������Ӧ��		2+3 =5
	{15, 0x6A,0xA6, 0, 0,0, 0,7,  0x85,0x03, 0,0,0,0,0,0,0},	//12 CMD 85 03  ��������Ӧ��		2+5 =7
	{10, 0x6A,0xA6, 0, 0,0, 0,2,  0x81,0x00, 0,0,0,0,0,0,0},	//13 CMD 81 00  ����				2+0 =2
	{12, 0x6A,0xA6, 1, 0,0, 0,4,  0x82,0x00, 0,0,0,0,0,0,0},	//14 CMD 82 00  ������Ϣ			2+2 =4
	{11, 0x6A,0xA6, 1, 0,0, 0,3,  0x81,0x05, 0,0,0,0,0,0,0},	//15 CMD 81 05  MUTE״̬��Ϣ		2+1 =3
	{11, 0x6A,0xA6, 1, 0,0, 0,3,  0x81,0x06, 0,0,0,0,0,0,0},	//16 CMD 81 06  ��Ļʧ�ܿ���		2+1 =3
	{11, 0x6A,0xA6, 1, 0,0, 0,3,  0x86,0x00, 0,0,0,0,0,0,0},	//17 CMD 86 00  �������ģʽ���	2+1 =3
	{11, 0x6A,0xA6, 1, 0,0, 0,3,  0x86,0x10, 0,0,0,0,0,0,0},	//18 CMD 86 10  EEPROM���Խ��		2+1 =3
	{13, 0x6A,0xA6, 1, 0,0, 0,5,  0x86,0x21, 0,0,0,0,0,0,0},	//19 CMD 86 21  ̨ͣ�������ϱ�		2+3 =5
	{13, 0x6A,0xA6, 1, 0,0, 0,5,  0x86,0x22, 0,0,0,0,0,0,0},	//20 CMD 86 22  ������ϱ�			2+3 =5
	{13, 0x6A,0xA6, 1, 0,0, 0,5,  0x86,0x23, 0,0,0,0,0,0,0},	//21 CMD 86 23  Ƶ����Ӧ�ϱ�		2+3 =5
	{15, 0x6A,0xA6, 1, 0,0, 0,7,  0x86,0x30, 0,0,0,0,0,0,0},	//22 CMD 86 30  CAN���ͽ��			2+5 =7
	{22, 0x6A,0xA6, 1, 0,0, 0,14, 0x86,0x31, 0,0,0,0,0,0,0},	//23 CMD 86 31  CAN���ݽ���			2+12=14
	{12, 0x6A,0xA6, 1, 0,0, 0,4,  0x86,0x40, 0,0,0,0,0,0,0},	//24 CMD 86 40  ���ӱ궨���		2+2=4
	{15, 0x6A,0xA6, 1, 0,0, 0,7,  0x86,0x50, 0,0,0,0,0,0,0},	//25 CMD 86 50  CAN���ͽ��			2+5 =7
	{22, 0x6A,0xA6, 1, 0,0, 0,14, 0x86,0x51, 0,0,0,0,0,0,0},	//26 CMD 86 51  CAN���ݽ���			2+12=14
	{25, 0x6A,0xA6, 1, 0,0, 0,17, 0x86,0x60, 0,0,0,0,0,0,0},	//27 CMD 86 60  �汾���ϴ�			2+15=17
	{10, 0x6A,0xA6, 0, 0,0, 0,2,  0x86,0x70, 0,0,0,0,0,0,0},	//28
	{11, 0x6A,0xA6, 0, 0,0, 0,3,  0x86,0x80, 0,0,0,0,0,0,0},	//29 CMD 86 80  
	{12, 0x6A,0xA6, 1, 0,0, 0,4,  0x80,0x09, 0,0,0,0,0,0,0},	//30 CMD 80 09  ʹ��ʱ��			2+15=17
	{11, 0x6A,0xA6, 0, 0,0, 0,3,  0x80,0x0A, 0,0,0,0,0,0,0},	//31 CMD 80 0A  һ���ӵ���ʱ
	{25, 0x6A,0xA6, 1, 0,0, 0,17, 0x81,0x08, 0,0,0,0,0,0,0},	//32 CMD 81 07  �汾���ϴ�			2+15=17
	{19, 0x6A,0xA6, 1, 0,0, 0,11, 0x83,0x04, 0,0,0,0,0,0,0},	//33 CMD 83 04  RDS			2+9=11
	{25, 0x6A,0xA6, 1, 0,0, 0,17, 0x86,0x61, 0,0,0,0,0,0,0},	//34 CMD 86 61  �������汾
	{11, 0x6A,0xA6, 1, 0,0, 0,3,  0x81,0x0A, 0,0,0,0,0,0,0},	//35 CMD 81 0A  Ӱ��״̬
};

static u16	TX_FN = 65535;	//������������
//static u8 ACK_Task = 0;		//ACK����
								
typedef struct{
	u16 Last_FN;	//��һ֡����
	u16 SYNC;		//ͬ��ͷ
	u8  ACK;		//Ӧ��
	u16 FN;			//����
	u16 LEN;		//֡����
	u8  CMD_H;		//������
	u8  CMD_L;		//������
	u8  CHECKSUM;	//У���
	u8  PARAM[256];	//�������ݻ�������
}UART_STA;
static UART_STA	UartRX = {32768};	//�յ������ݣ���ʼ��һ����������						


u8 lockmenu_flag = 0;

u8 (*TtimeIRQ)(void);
u8 null(void){return 0;}



u8 CMD_8304(u8 Sta);
void loop_task(void)
{
	static u16 times = 0;
	if(times<10)times ++;
	else times = 0;
	
	switch(times)
	{
#ifdef RDS_ENABLE
		case 0:CMD_8304(0);break;
#endif
		case 1:break;
		case 2:break;
	}
	
}



//ACK����
void ACK_Write(ACK_SF dat)
{
	ACK_Chce[ack_writ_p++] = dat;
	if(ack_writ_p>=ACKCHCELEN)ack_writ_p=0;
	if(ack_writ_p==ack_read_p)ack_read_p++;
	if(ack_read_p>=ACKCHCELEN)ack_read_p=0;
}
u8 ACK_READ(ACK_SF *dat)
{
	if(ack_writ_p==ack_read_p)return 0;
	*dat = ACK_Chce[ack_read_p++];
	if(ack_read_p>=ACKCHCELEN)ack_read_p=0;
	return 1;
}

void ACK_Clear(void)
{
	ack_read_p = ack_writ_p;
}


//װ��������У���
static void FnCheckSun_load(u8 CH)
{
	u8 i = 0;
	u16 Temp = 0;
	++TX_FN;
	Frame_Buff[CH][4] = (u8)(TX_FN>>8)&0x00FF; //���Ÿ�λ
	Frame_Buff[CH][5] = (u8)TX_FN&0x00FF;		//���ŵ�λ
	//����У���
	for(i=3;i<(Frame_Buff[CH][0]);i++) Temp += Frame_Buff[CH][i];
	Frame_Buff[CH][Frame_Buff[CH][0]] = (u8)~(Temp)+1;
}
//ACKװ��
static void ACK_Load(u8 ACK_STA,u16 FN_Num,u8 Cmd_H,u8 Cmd_L)
{
	ACK_SF dat;
	u16 temp = 0;
	u8 i = 0;
	dat.data[0] = 10;
	dat.data[1] = 0x6A;
	dat.data[2] = 0xA6;
	dat.data[3] = ACK_STA|0x80;			//Ӧ��״̬����Ӧ���Ӧ����ӷ���
	dat.data[4] = (u8)(FN_Num>>8);		//���Ÿ�
	dat.data[5] = (u8)(FN_Num&0xff);	//���ŵ�
	dat.data[6] = 0x00;					//LEN��
	dat.data[7] = 0x02;					//LEN��
	dat.data[8] = Cmd_H;				//CMD_H
	dat.data[9] = Cmd_L;				//CMD_L
	//����У���
	for(i=2;i<9;i++) temp += dat.data[i];
	dat.data[10] = (u8)(~temp+1);		//У���
	//֪ͨ����
	
	ACK_Write(dat);
//	ACK_Task = 1;
}
//
//���ʹ�����
u8 TX_Stabuff[Frame_Cap][6]={0};//0������״̬��1����ʱ��ʱ����2����ʱʱ�䣬3���ط���������4�����ʹ�����5���ط�������
/**
*--���ܣ��鿴��������״̬
*--���룺CH ��֡���
*--���أ�0�����У�1��busy
**/
u8 Check_TXSTA(u8 CH)
{
	return TX_Stabuff[CH][0];
}
//

//����������񣬰���ACK
void clean_TXtask(void)
{
	u8 i = 0;
	for(i=0;i<Frame_Cap;i++)
	TX_Stabuff[i][0] = 0;
	lockmenu_flag = 0;
//	ACK_Task = 0;
	ACK_Clear();
}

void lock_check(void)
{
	if(lockmenu_flag==2)return;
	lockmenu_flag++;
//	if(CAR_Inf.ACCDe)	CMD_8106(0);//��Ļʧ�ܿ���
//	else				CMD_8106(1);//��Ļʧ�ܿ���
}

/**
*--���ܣ����һ�η�������
*--���룺CH ��֡���
*--		Cnt�����ʹ���
*--		Time����ʱʱ��
*--		Mode������ģʽ��0���������ͣ�1���ӳٷ���
*--���أ�0���ɹ���1��ʧ��
**/
u8 TX_TaskADD(u8 CH,u8 Cnt,u16 Time,u8 Mode)
{
	u8 CMD_STA = 0;
	if((SYS_Inf.MPU_STA != 1)&&(SYS_Inf.MPU_STA != 5)) return 1;//�ǹ���ģʽ�����߼��ģʽ���ܾ���������
	if(SYS_Inf.STA == 3)
	{
		switch(CH)//����ģʽ��������������
		{
			case 9:CMD_STA = 1;break;
			case 10:CMD_STA = 1;break;
			case 11:CMD_STA = 1;break;
			case 12:CMD_STA = 1;break;
			default:CMD_STA = 0;break;
		}
		if(CMD_STA == 0)return 2;
	}
	if(SYS_Inf.MPU_STA == 5)
	{
		switch(CH)//����ģʽ��������������
		{
			case 3:CMD_STA = 1;break;
			case CMD8100:CMD_STA = 1;break;
			case CMD8000:CMD_STA = 1;break;
			default:CMD_STA = 0;break;
		}
		if(CMD_STA == 0)return 2;
	}
	
	FnCheckSun_load(CH);
//	if(TX_Stabuff[CH][0] == 1) return 3;//��������У����ʧ�ܣ�����ԭ����
	if(Mode)TX_Stabuff[CH][1] = 0;		//��ʱ������	�ӳٷ���
	else	TX_Stabuff[CH][1] = Time;	//��ʱ������	��������
	TX_Stabuff[CH][2] = Time;			//��ʱʱ��
	TX_Stabuff[CH][4] = Cnt;			//���ʹ���
	TX_Stabuff[CH][3] = 0;
	TX_Stabuff[CH][0] = 1;				//�������
	return 0;
}
//
/**
*--���ܣ�ע��һ�������е�����
*--���룺CH ��֡���
*--���أ�0���ɹ���1��ʧ��
**/
u8 TX_TaskCancel(u8 CH)
{
	if(TX_Stabuff[CH][0] == 0) return 1;//����գ�
	TX_Stabuff[CH][0] = 0;				//�������	
	TX_Stabuff[CH][1] = 0;				//��ʱ�������
	TX_Stabuff[CH][2] = 0;				//��ʱʱ�����
	TX_Stabuff[CH][3] = 0;				//�ط����������
	return 0;
}
//


//��������
#include "TestCMD.h"
u8 KEY_Process(void)
{
	u8 key = 0;
	u8 sta = 0;
	if(SYS_Inf.KEY_ENABLE == 1)
	{
		clear_KEYvalue();
		return 0;
	}
	if(Check_TXSTA(4) == 0)//�������Ϳ���ʱ���鿴��������
	{
		if(Read_KEY(&key,&sta))
		{
			CMD_8102(key,sta);
			Test_keysend(key,sta);
		}
	}
	return 0;
}
//
//�������ݵ���ʱװ�أ�
//��ʱ�ص����� TtimeIRQ 
u16 Delay_cnt[2]={0};
u8 delay_task(u8 (*delay_fun)(void),u16 time)
{
//	if(Delay_cnt[0] == 1) return 0;
	TtimeIRQ = delay_fun;
	Delay_cnt[1] = time;
	Delay_cnt[0] = 1;
	return 1;
}

/**
*--���ܣ�������������
*--���룺��
*--���أ�0��������
*--Delay_cnt[] : 0������״̬��1����ʱ��ʱ����2����ʱʱ�䣬3���ط���������4�����ʹ�����5���ط�������
**/
u8 TX_Process(void)
{
	u8 i = 0;
	ACK_SF dat;
	
	KEY_Process();
	
	if(Delay_cnt[0] == 1)//��ʱ����
	{
		if(Delay_cnt[1]>0)Delay_cnt[1]--;
		else
		{
			Delay_cnt[0] = 0;
			TtimeIRQ();
			TtimeIRQ = null;
		}
	}
	
	loop_task();//��������
	
	if(ACK_READ(&dat))//Ӧ����
	{DataSend(dat.data);return 0;}
	
	for(i=0;i<Frame_Cap;i++)
	{
		if(TX_Stabuff[i][0] == 1)	//�ҳ���Ҫ���͵�����
		{
			if(TX_Stabuff[i][1]<TX_Stabuff[i][2])TX_Stabuff[i][1]++;	//��ʱ��ʱ
			else	//�ѳ�ʱ
			{
				TX_Stabuff[i][1] = 0;//���¼�ʱ
				if(TX_Stabuff[i][3]<TX_Stabuff[i][4])	//δ������
				{
					//����
					DataSend(Frame_Buff[i]);
					TX_Stabuff[i][3]++;
					if(TX_Stabuff[i][3]>=TX_Stabuff[i][4])//�Ѵ�����
					{
						TX_Stabuff[i][0] = 0;	//���������
						TX_Stabuff[i][1] = 0;	//��ʱ�������
						TX_Stabuff[i][3] = 0;	//�ط����������
						TX_Stabuff[i][4] = 0;
						TX_Stabuff[i][5] = 1;	//�ط��ﵽ����
					}
					return 0;//ֱ���˳��˴�ѭ������ֹ��������ƴ�ӣ�����������ȼ�����
				}
			}
		}
	}
	return 0;
}
//


//�������ݴ���
void CMD_8502(u8 mode);
/**
*--���ܣ��������ݴ�������Э�������
*--���룺Msg���յ������ݳ���
*--		 Dat���յ�����������
*--���أ���������
*--
**/
u8 ACK_Process(u16 CMD,u8 ACK);
static u8 Start_explain(u8 Cmd_L,u8 *Dat,u8 err);
static u8 Info_explain(u8 Cmd_L,u8 *Dat,u8 err);
static u8 CarSet_explain(u8 Cmd_L,u8 *Dat,u8 err);
static u8 Radio_explain(u8 Cmd_L,u8 *Dat,u8 err);
static u8 CanPro_explain(u8 Cmd_L,u8 *Dat,u8 err);
static u8 up_explain(u8 Cmd_L,u8 *Dat,u8 err);
static u8 factorymode_explain(u8 Cmd_L,u8 *Dat,u8 err);
void cl_mpudtc(void);
u8 UART_explain(u16 Msg,u8 *Dat)
{
	//����ֲ�����
	u16 temp = 0;	//����ͻ������
	u16 i = 0;		//
	u8 re = 0;		//�������ͱ��
	u8 cmdcheck = 0;
	
	//���ݼ��������
	UartRX.SYNC = (Dat[0]<<8)+Dat[1];	//���SYNC
	UartRX.LEN = (Dat[5]<<8)+Dat[6];	//���LEN
	UartRX.ACK = Dat[2];				//���ACK
	UartRX.CMD_H = Dat[7];				//���CMD_H
	UartRX.CMD_L = Dat[8];				//���CMD_L
	UartRX.FN = (Dat[3]<<8)+Dat[4];		//���FN
	//�������ͣ��޶�200�ֽڣ���ֹ���ݴ��������ѭ������Ч���ݲ��ᳬ��200��
	for(i=0;(i<(UartRX.LEN+5)&&(i<200));i++) temp += Dat[i+2];
	UartRX.CHECKSUM = (u8)~(temp)+1;	
	
	//����У��
	if(UartRX.SYNC != UART_SYNC)	re = 1;	//֡ͬ��ͷУ��
	if(UartRX.LEN != (Msg-8))	re = 2;	//����У��
#if	CheckSun
	if(UartRX.CHECKSUM != Dat[msg-1])	re = 3;	//У���У��
#endif
	if(re != 0) //У�鲻ͨ��������֡
	{
		if(((UartRX.CMD_H<<8)+UartRX.CMD_L)== 0x0501)//�������ݰ���������Ӧ��
			CMD_8502(1);
		return re;
	}
	
	cl_mpudtc();//����һ����˵����ȷ�յ������ˣ�MPUӦ�����߲Ŷԣ����MPU������Ϣ
	
	if(SYS_Inf.MPU_STA != 1)//����������ģʽ�µ��������
	{
		switch((UartRX.CMD_H<<8)+UartRX.CMD_L)
		{
			case 0x0000:cmdcheck = 1;break;
			default:cmdcheck = 255;break;
		}	
		if(cmdcheck == 255)return 0;
	}

	if((((UartRX.ACK&0x7F) == 0x02)||((UartRX.ACK&0x7F) == 0x03))&&(UartRX.CMD_H&0x80))//�յ�Ӧ���ź�
	{
		re = ACK_Process((u16)((UartRX.CMD_H<<8)+UartRX.CMD_L),(u8)(UartRX.ACK&0x7F));//Ӧ���źŴ���
		return re;
	}
	
#if CheckFN
	if(UartRX.FN == UartRX.Last_FN) return 5;	//��ͬ֡�����ظ�ִ��
	UartRX.Last_FN = UartRX.FN;					//��¼�µ�����
#endif
	
	//��������
	for(i=0;i<(UartRX.LEN-2);i++) UartRX.PARAM[i] = Dat[i+9];//��������
	//�����
	switch(UartRX.CMD_H)
	{
		case 0x00:re =  Start_explain(UartRX.CMD_L,UartRX.PARAM,6);;break;
		case 0x01:re =   Info_explain(UartRX.CMD_L,UartRX.PARAM,7);break;
		case 0x02:re = CarSet_explain(UartRX.CMD_L,UartRX.PARAM,8);break;
		case 0x03:re =  Radio_explain(UartRX.CMD_L,UartRX.PARAM,9);break;
		case 0x04:re = CanPro_explain(UartRX.CMD_L,UartRX.PARAM,10);break;
		case 0x05:re = up_explain(UartRX.CMD_L,UartRX.PARAM,11);break;
		case 0x06:re = factorymode_explain(UartRX.CMD_L,UartRX.PARAM,12);break;
		
		default:re = 255;break;
	}
	//Ӧ���ͣ�Ӧ����������ȼ���
	if((UartRX.ACK&0x7F) == 0x01)
	{
		if(re == 0)	ACK_Load(0x02,UartRX.FN,UartRX.CMD_H,UartRX.CMD_L);	//��Ӧ��0x02
		else		ACK_Load(0x03,UartRX.FN,UartRX.CMD_H,UartRX.CMD_L);	//��Ӧ��0x03
	}
	return re;
}
//

/**
*--���ܣ�Ӧ������
*--���룺CMD���յ�������CMD
*--		 ACK���յ���ACK
*--���أ���������
**/
u8 ACK_Process(u16 CMD,u8 ACK)
{
	u8 i = 0;
	switch(CMD)//���CMD��Ӧ��֡��ţ�����ע���ط�����
	{
		case 0x8000:i=0;break;
		case 0x8003:i=1;break;
		case 0x8005:i=2;break;
		case 0x8101:i=3;break;
		case 0x8102:i=4;break;
		case 0x8103:i=5;break;
		case 0x8104:i=6;break;
		case 0x8201:i=7;break;
		case 0x8301:i=8;break;	
		case 0x8500:i=9;break;
		case 0x8501:i=10;break;
		case 0x8502:i=11;break;
		case 0x8503:i=12;break;
		case 0x8100:i=13;break;
		case 0x8200:i=14;break;	
		case 0x8105:i=15;break;
		case 0x8106:i=16;break;
		case 0x8600:i=17;break;
		case 0x8610:i=18;break;
		case 0x8621:i=19;break;	
		case 0x8622:i=20;break;
		case 0x8623:i=21;break;
		case 0x8630:i=22;break;
		case 0x8631:i=23;break;
		case 0x8640:i=24;break;	
		case 0x8650:i=25;break;	
		case 0x8651:i=26;break;	
		case 0x8660:i=27;break;	
		case 0x8670:i=28;break;	
		case 0x8680:i=29;break;	
		case 0x8009:i=30;break;	
		case 0x800A:i=31;break;	
		case 0x8108:i=32;break;	
		case 0x8304:i=33;break;	
		case 0x8661:i=34;break;	
		case 0x810A:i=35;break;
		
		
		
		default:i=255;break;
	}
	if(i==255)return 4;//CMD�����޷�����
	if(ACK == 0X02)	//��Ӧ��
	{
		TX_TaskCancel(i);	//ע���ط�
	}
	else if(ACK == 0X03)//��Ӧ��
	{
		TX_TaskCancel(i);	//ע���ط�
		
	}
	else //����Ӧ��
	{
		return 4;
	}
	return 0;
}
//


//��������
static u8 CMD_0000(u8 *Dat);//���ֽ���ͨ��
static u8 CMD_0003(u8 *Dat);//ִ�йػ�
static u8 CMD_0006(u8 *Dat);//ִ��ʱ��У׼
static u8 CMD_8000(u8 *Dat);//�ظ�������Ϣ��ʱ�䣬���ã��汾��
static u8 CMD_8005(u8 *Dat);//�ظ�����ԭ��
static u8 Start_explain(u8 Cmd_L,u8 *Dat,u8 err)
{
	u8 re = 0;
	switch(Cmd_L)
	{
		case 0x00:CMD_0000(Dat);re = CMD_8000(Dat);break;	//�������� �ظ�0x80 0x00
		case 0x03:re = CMD_0003(Dat);break;					//����ػ� Ӧ��	��ִ�йػ����̣� 
		case 0x05:CMD_8005(Dat);break;					//����ԭ�� �ظ�0x80 0x05
		case 0x06:re = CMD_0006(Dat);break;				//ʱ��У׼ Ӧ��	��ִ��ʱ��У׼��
		default:re = err;break;
	}
	if(0 == re) return 0;
	else return re;
}
//
//����
static u8 CMD_0000(u8 *Dat)
{
	//��־MPU����
	SYS_Inf.MPU_STA = 1;//MPU���������
	return 0;
}
//
//MPU��������
static u8 CMD_0003(u8 *Dat)
{
	if((Dat[0]!=1)&&(Dat[0]!=2))	return 1;//������������
	//��־MPU���ߡ�ִ�йػ�
	SYS_Inf.MPU_STA = Dat[0]+1;//ת��Ϊ����״̬
	return 0;
}
//
//У׼ʱ��
static u8 CMD_0006(u8 *Dat)
{
	if((Dat[0]>110)||(Dat[1]>11)||(Dat[2]>31)||(Dat[3]>23)||(Dat[4]>59)||(Dat[5]>59)||(Dat[2]==0))	return 1;//���ݸ�ʽ����
//	printf("gettime\r\n");	
	RTC_Set(Dat[0]+1985,Dat[1]+1,Dat[2],Dat[3],Dat[4],Dat[5]+1);
	
	return 0;
}
//
//ʱ�䡢����ͬ��
//CMD_8000	�ظ�0000
static u8 CMD_8000(u8 *Dat)
{
	u8 i = 0;
	u8 re = 0;
	//����һ��ʱ��
	RTC_Get();
	//װ������
	Frame_Buff[CMD8000][10] = (calendar.w_year-1985);	//�꣨ƫ��1985��
	Frame_Buff[CMD8000][11] = calendar.w_month-1;			//��
	Frame_Buff[CMD8000][12] = calendar.w_date;			//��
	Frame_Buff[CMD8000][13] = calendar.hour;			//ʱ
	Frame_Buff[CMD8000][14] = calendar.min;				//�� 
	Frame_Buff[CMD8000][15] = calendar.sec;				//��
	//��ȡһ��������Ϣ
	
	Frame_Buff[CMD8000][16] = ((SYS_Config.LOC<<0)&0X01)+((SYS_Config.DDWS<<1)&0X02)+((SYS_Config.BSD<<2)&0x04);	//������Ϣ
#ifdef LOOK_AROUND
	Frame_Buff[CMD8200][11] = 2;	//����
	
	#ifdef TW2836
	Frame_Buff[CMD8200][11] = 1;	//����
	#endif
#else
	Frame_Buff[CMD8200][11] = (SYS_Config.LOA&0x03);	//����Ӱ��������Ϣ
#endif
	//�汾�� 
	Frame_Buff[CMD8000][18]=Version2_SIZE;
	for(i=0;i<Version2_SIZE;i++)
	Frame_Buff[CMD8000][19+i] = (u8)(*(Version2+i));
	//֡���ȡ�LENװ�أ�
	Frame_Buff[CMD8000][0] = 19+Version2_SIZE;
	Frame_Buff[CMD8000][7] = 11+Version2_SIZE;
	//֪ͨ����
	re = TX_TaskADD(CMD8000,TX_Cnt,TX_oTime,0);//�ظ�������Ϣ��
	
	//��������ʱ��Ҫ���͵���Ϣ
	RADIO_Inf.CMD = 2;
	CMD_8101(1);//�����ٶȣ��ӳٷ���
	
	
	return re;
}
//
//CMD_8003	�ػ�����ʱ����
void CMD_8003(void)
{
	Frame_Buff[CMD8003][10] = (u8)ContTime;	//����ʱ�����ӣ�
	//֪ͨ����
	TX_TaskADD(CMD8003,1,TX_oTime,0);
}
//
//����ԭ��
//CMD_8005 �ظ�0005
static u8 CMD_8005(u8 *Dat)
{
//	Frame_Buff[CMD8005][10] = (u8)START_Reason|0X80;	//����ԭ��
	
	if(CAR_Inf.ACC == 1)	Frame_Buff[CMD8005][10] = (u8)START_Reason|0X80;	//����ԭ��
	else						Frame_Buff[CMD8005][10] = (u8)START_Reason;	//����ԭ��
	//֪ͨ����
	TX_TaskADD(CMD8005,TX_Cnt,TX_oTime,0);
	return 0;
}
//

//
//ʹ��ʱ��
//CMD_8009 �ϱ�ʹ��ʱ��
u8 CMD_8009(u16 Dat)
{
	Frame_Buff[CMD8009][10] = (u8)((Dat>>8)&0XFF);	//
	Frame_Buff[CMD8009][11] = (u8)((Dat>>0)&0XFF);	//
	
	//֪ͨ����
	TX_TaskADD(CMD8009,TX_Cnt,TX_oTime,0);
	return 0;
}
//
//��ѹ�ػ�����
//��ѹ���ͣ�һ���ӹػ�����
u8 CMD_800A(u8 Dat)
{
	Frame_Buff[CMD800A][10] = (u8)Dat;	//����ԭ��
	
	//֪ͨ����
	TX_TaskADD(CMD800A,TX_Cnt,TX_oTime,0);
	return 0;
}

//
//��Ϣ
static u8 CMD_0101(u8 *Dat);
static u8 CMD_0104(u8 *Dat);
static u8 CMD_0105(u8 *Dat);
static u8 CMD_0107(u8 *Dat);
u8 CMD_8102(u8 NUM,u8 STA);
u8 CMD_8103(u8 NUM,u8 STA);
static u8 CMD_8104(u8 STA);
u8 CMD_8105(u8 STA);
u8 CMD_8106(u8 STA);
u8 CMD_8108(void);
u8 CMD_810A(u8 sta);
static u8 Info_explain(u8 Cmd_L,u8 *Dat,u8 err)
{
	u8 re = 0;
	switch(Cmd_L)
	{
		case 0x01:re = CMD_0101(Dat);break;	//����״̬��Ϣ ����
		case 0x04:re = CMD_0104(Dat);break;	//���ӿ���
		case 0x05:re = CMD_0105(Dat);break; //����MUTE����
		case 0x07:re = CMD_0107(Dat);break; //����MUTE����
		case 0x08:re = CMD_8108();break;	//�ڲ��汾������
		case 0x0A:
			if(CAR_Inf.Car_Speed<(15<<8))re = CMD_810A(Video_sta);
			else re = CMD_810A(0);
		break;
		
		default:re = err;break;
	}
	if(0 == re) return 0;
	else return err;
}
//
//״̬
static u8 CMD_0101(u8 *Dat)
{
	//״̬��Ϣ�����ϣ�
	lock_check();
	FAULT_Inf.BLTH = Dat[0]?1:0;
	FAULT_Inf.WIFI = Dat[1]?1:0;
	FAULT_Inf.VOICE = Dat[2]?1:0;
	return 0;
}
//
//���ӿ���
extern u8 LA_CMD_82(u8 LAwark_Mod);
static u8 CMD_0104(u8 *Dat)
{
	//���ӿ��ƣ���������ת��
	if(Dat[0]<5)
	{

		if(LAROUND_Inf.STA == 1)	//��������״̬
		{
			LAROUND_Inf.VIS = (u8)Dat[0];
			
#ifdef TW2836
			if(LAROUND_Inf.VIS == 1)TW2836_Switch(2);
			if(LAROUND_Inf.VIS == 2)TW2836_Switch(4);
			if(LAROUND_Inf.VIS == 3)TW2836_Switch(0);
			if(LAROUND_Inf.VIS == 4)TW2836_Switch(1);
#else 
			LA_CMD_82(LAROUND_Inf.VIS);			
#endif
			
			CMD_8104(0);
		}
		else if(LAROUND_Inf.STA == 2)	CMD_8104(1);	//���ϣ�������
		else if(LAROUND_Inf.STA == 0)	CMD_8104(2);	//δ���ߣ�������
		return 0;
	}
	else return 1;
}


void LowPow_warn(void);
static u8 CMD_0105(u8 *Dat)
{
	//����MUTE����
	LowPow_warn();
	if(Dat[0]<2)
	{
		if(Dat[0] == 1)	SYS_Inf.MUTE_STA = 1;
		else	SYS_Inf.MUTE_STA = 0;	
		
		if(SYS_Inf.MUTE_OK == 0xFF)
		{
			Mute_Ctrl(SYS_Inf.MUTE_STA);
		}
		
		CMD_8105(SYS_Inf.MUTE_STA);		
		
		return 0;
	}
	else return 1;
} 

/*
static u8 CMD_0106(u8 *Dat)
{
	
}
*/

static u8 CMD_0107(u8 *Dat)
{

	if(CAR_Inf.ReverseSta == 1)	SYS_Inf.MUTE_OK |= 0x0F;
	else						SYS_Inf.MUTE_OK |= 0xFF;
	
	if(SYS_Inf.MUTE_OK == 0xFF)
	{
		Mute_Ctrl(SYS_Inf.MUTE_STA);
	}
	return 0;
} 

//static u8 CMD_010A(u8 *Dat)
//{
//	
//	
//}


//
u8 MPUHAERT_Cnt = 0;
void cl_mpudtc(void)//MPU�������
{
	MPUHAERT_Cnt = 0;
	if(SYS_Inf.MPU_STA == 5)SYS_Inf.MPU_STA=1;
	FAULT_Inf.MPUDIED = 0;
}
//CMD_8100
u8 CMD_8100(void)
{
	//֪ͨ����
	TX_TaskADD(CMD8100,1,TX_oTime,0);//һ�Σ��������ͣ�û���ط�����
	
#ifdef EMMCDEBUG
	return 0;
#endif
	if(MPUHAERT_Cnt<6)MPUHAERT_Cnt++;//���Σ���ǹ���
	else 
	{
		SYS_Inf.MPU_STA = 5;
		FAULT_Inf.MPUDIED = 1;
	}
	return 0;
}

//CMD_8101  //������Ϣ��
u8 CMD_8101(u8 mode)
{
	Frame_Buff[CMD8101][10] = (u8)(CAR_Inf.Car_Speed>>8)&0xFF;	//���ٸ�λ
	Frame_Buff[CMD8101][11] = (u8)(CAR_Inf.Car_Speed&0xFF);	//���ٵ�λ
	//֪ͨ����
	TX_TaskADD(CMD8101,TX_Cnt,TX_oTime,mode);//����
	
	return 0;
}
//
//CMD_8102	��������
u8 CMD_8102(u8 NUM,u8 STA)
{
	if(TX_Stabuff[CMD8102][0] == 1) return 1;
	Frame_Buff[CMD8102][10] = NUM;	//�������
	Frame_Buff[CMD8102][11] = STA;	//����״̬
	//֪ͨ����
	TX_TaskADD(CMD8102,1,TX_oTime,0);
	return 0;
}
//CMD_8103	����
u8 CMD_8103(u8 NUM,u8 STA)
{
	if(TX_Stabuff[CMD8103][0] == 1) return 1;
	Frame_Buff[CMD8103][10] = NUM;	//��Ϣ��� ���⡢ת��
	Frame_Buff[CMD8103][11] = STA;	//״̬
	//֪ͨ����
	TX_TaskADD(CMD8103,TX_Cnt,TX_oTime,0);
	return 0;
}
//
//CMD_8104	����״̬�ظ�
static u8 CMD_8104(u8 STA)
{
	Frame_Buff[CMD8104][10] = STA;	//
	//֪ͨ����
	TX_TaskADD(CMD8104,TX_Cnt,TX_oTime,0);
	return 0;
}
//
//CMD_8105	MUTE״̬�ظ�
u8 CMD_8105(u8 STA)
{
//	Mute_Ctrl(STA);
	Frame_Buff[CMD8105][10] = STA;	//
	//֪ͨ����
	TX_TaskADD(CMD8105,TX_Cnt,TX_oTime,0);
	return 0;
}
//
//CMD_8106	��Ļʧ�ܿ���
u8 CMD_8106(u8 STA)
{
	
//	SYS_Inf.MUTE_STA = STA;
//	CMD_8105(SYS_Inf.MUTE_STA);
	
	Frame_Buff[CMD8106][10] = STA;	//
	//֪ͨ����
	TX_TaskADD(CMD8106,TX_Cnt,TX_oTime,0);
	return 0;
}
//
//

u8 CMD_8108(void)
{
	Frame_Buff[CMD8108][10]=(u8)(Version_SIZE);
	for(u8 i=0;i<Version_SIZE;i++)
	Frame_Buff[CMD8108][11+i] = (u8)(*(Version+i));
	//֡���ȡ�LENװ�أ�
	Frame_Buff[CMD8108][0] = 11+Version_SIZE;
	Frame_Buff[CMD8108][7] = 3+Version_SIZE;
	TX_TaskADD(CMD8108,1,TX_oTime,0);
	return 0;
}
//

u8 CMD_810A(u8 sta)
{
	
	Frame_Buff[CMD810A][10] = sta;	//
	//֪ͨ����
	TX_TaskADD(CMD810A,TX_Cnt,TX_oTime,0);
	
//	printf("report video_sta:%d\r\n",sta);
	
	return 0;
}





//��������
static u8 CMD_0202(u8 *Dat);
u8 CMD_8200(u8 *Dat);
u8 CMD_8201(void);
static u8 CarSet_explain(u8 Cmd_L,u8 *Dat,u8 err)
{
	u8 re = 0;
//	lock_check();
	switch(Cmd_L)
	{
		case 0x00:re = CMD_8200(Dat);break;
		case 0x01:re = CMD_8201();break;	//�������ÿ���״̬ �ظ�0x82 0x01
		case 0x02:re = CMD_0202(Dat);break;	//�������� Ӧ�� (ִ�п������� CANת��)
		default:re = err;break;
	}
	if(0 == re) return 0;
	else return err;
}





//
//�������ÿ��ؿ���
static u8 CMD_0202(u8 *Dat)
{
	//CANת��
	Can_SF	can_temp;
	if((Dat[0]==0)&&(Dat[1]==0xFF))
	{
		//Dat[0]Ϊ0ʱ���ָ�Ĭ������,Ĭ��ȫ��
		CAR_Set.OVERSPEE  = 1;
		CAR_Set.CARSEARCH = 1;
		CAR_Set.DRIVLOCK  = 1;
		CAR_Set.PARKULOCK = 1;
		CAR_Set.BSD       = 1;
		CAR_Set.DDWS      = 1;
	}
	else if( (Dat[0]>0) &&(Dat[0]<7) && (Dat[1]<2))
	{
		//����ֵ����Ҫ���棬����ȥ�����ˣ�����ʱû��CAN�����Ա�����ģ��
//		switch(Dat[0])
//		{
//			case 1:CAR_Set.OVERSPEE = Dat[1];break;
//			case 2:CAR_Set.CARSEARCH = Dat[1];break;
//			case 3:CAR_Set.DRIVLOCK = Dat[1];break;
//			case 4:CAR_Set.PARKULOCK = Dat[1];break;
//			case 5:CAR_Set.DDWS = Dat[1];break;
//			case 6:CAR_Set.BSD = Dat[1];break;
//		}
		switch(Dat[0])
		{
			case 1:;break;
			case 2:;break;
			case 3:;break;
			case 4:;break;
			case 5:Dat[0] = 6;break;
			case 6:Dat[0] = 5;break;
		}
		
	}
	else return 1;
	
	can_temp.id = 0x18FFC676;
	can_temp.data[0] = 0xFC|CAR_Set.OVERSPEE;
	can_temp.data[1] = 0xFC|CAR_Set.CARSEARCH;
	can_temp.data[2] = 0xFC|CAR_Set.DRIVLOCK;
	can_temp.data[3] = 0xFC|CAR_Set.PARKULOCK;
	can_temp.data[4] = 0xFC|CAR_Set.BSD;
	can_temp.data[5] = 0xFC|CAR_Set.DDWS;
	if(Dat[0]!=0)can_temp.data[Dat[0]-1] = 0xFC|Dat[1];
	can_temp.data[6] = 0xFF;
	can_temp.data[7] = 0xFF;
	
	Can_Write_TXMsg(can_temp,1);//���뻺���������ȼ�Ϊ1�������ȼ�

	//�ӳٷ������ý��������
	delay_task(CMD_8201,500);//�ӳ�2��
	
	return 0;
}
//
//CMD_8200	�ظ�0200
u8 CMD_8200(u8 *Dat)
{
	Frame_Buff[CMD8200][10] = ((SYS_Config.LOC<<0)&0X01)+((SYS_Config.DDWS<<1)&0X02)+((SYS_Config.BSD<<2)&0x04);	//������Ϣ
	
#ifdef LOOK_AROUND
	Frame_Buff[CMD8200][11] = 2;	//����
	#ifdef TW2836
	Frame_Buff[CMD8200][11] = 1;	//����
	#endif
	
#else
	Frame_Buff[CMD8200][11] = (SYS_Config.LOA&0x03);	//����Ӱ��������Ϣ
#endif
	//֪ͨ����
	TX_TaskADD(CMD8200,TX_Cnt,TX_oTime,0);
	return 0;
}
//

//CMD_8201	�ظ�0201
u8 CMD_8201(void)
{
	Frame_Buff[CMD8201][10] =   (CAR_Set.OVERSPEE&0x01)+\
								((CAR_Set.CARSEARCH&0x01)<<1)+\
								((CAR_Set.DRIVLOCK&0x01)<<2)+\
								((CAR_Set.PARKULOCK&0x01)<<3)+\
								((CAR_Set.DDWS&0x01)<<4)+\
								((CAR_Set.BSD&0x01)<<5);	//�������ÿ���״̬
	//֪ͨ����
	TX_TaskADD(CMD8201,2,10,0);
	return 0;
}
//

//
//������
static u8 CMD_0301(u8 *Dat);
static u8 CMD_0302(u8 *Dat);
static u8 CMD_0303(u8 *Dat);
static u8 Radio_explain(u8 Cmd_L,u8 *Dat,u8 err)
{
	u8 re = 0;
	switch(Cmd_L)	// ȫ����Ҫִ�к�ظ�
	{
		case 0x01:re = CMD_0301(Dat);break;	//ģʽ�л� �ظ�0x83 0x01 
		case 0x02:re = CMD_0302(Dat);break;	//�������� �ظ�0x83 0x01
		case 0x03:re = CMD_0303(Dat);break;	//��ת���� �ظ�0x83 0x01
		default:re = err;break;
	}
	if(0 == re) return 0;
	else return err;
}
//
//������ģʽ�л�
static u8 CMD_0301(u8 *Dat)
{
	//ģʽ�л�
	u16 FRQ_Temp = 0;
	if(RADIO_Inf.CMD==0)//æ�ж�
	{
		if((Dat[0]<2)&&(RADIO_Inf.CMD==0))
		{
			FRQ_Temp = (Dat[1]<<8)+Dat[2];
			if(0==Dat[0])
			{
#ifdef RDS_ENABLE
#else
				FRQ_Temp = FRQ_Temp*10;
#endif
				if((FRQ_Temp>=Frequency[0])&&(FRQ_Temp<=Frequency[1]))
				{
					RADIO_Inf.MODE = Dat[0];
					RADIO_Inf.FM_FRQ = FRQ_Temp;
					RADIO_Inf.CMD = 1;
					return 0;
				}
			}
			else if((1==Dat[0])&&( (FRQ_Temp>=Frequency[2])&&(FRQ_Temp<=Frequency[3]) )) 
			{
				RADIO_Inf.MODE = Dat[0];
				RADIO_Inf.AM_FRQ = FRQ_Temp;
				RADIO_Inf.CMD = 1;
				return 0;
			}
			else return 1;
		}
		else return 1;
	}
	return 1;
}
//
//��������������
static u8 CMD_0302(u8 *Dat)
{
	//��������
	if(RADIO_Inf.CMD==0)//æ�ж�
	{
		if(Dat[0]<5)
		{
			RADIO_Inf.CMD = Dat[0]+3;
			CMD_8301(4);
			if((RADIO_Inf.CMD == 4)||(RADIO_Inf.CMD == 5)||(RADIO_Inf.CMD == 7)) {RADIO_Inf.Last_FRQ = RADIO_Inf.MODE?RADIO_Inf.AM_FRQ:RADIO_Inf.FM_FRQ;}			
			return 0;
		}
		else return 1;
	}
	else if((Dat[0]==3)&&((RADIO_Inf.CMD == 4)||(RADIO_Inf.CMD == 5)||(RADIO_Inf.CMD == 7)))//��̨�����е�ֹͣ�����
			RADIO_Inf.CMD = Dat[0]+3;
	return 0;
}
//
//������Ƶ����ת
static u8 CMD_0303(u8 *Dat)
{
//Ƶ����ת
	u16 FRQ_Temp = 0;
	if(RADIO_Inf.CMD==0)//æ�ж�
	{
		FRQ_Temp = (Dat[0]<<8)+Dat[1];
		if(0==RADIO_Inf.MODE)
		{
#ifdef RDS_ENABLE
#else
		FRQ_Temp = FRQ_Temp*10;
#endif			
			if((FRQ_Temp>=Frequency[0])&&(FRQ_Temp<=Frequency[1]))
			{
				RADIO_Inf.FM_FRQ = FRQ_Temp;
				RADIO_Inf.CMD = 2;
				return 0;
			}
		}
		else if(1==RADIO_Inf.MODE)
		{
			if((FRQ_Temp>=Frequency[2])&&(FRQ_Temp<=Frequency[3]))
			{
				RADIO_Inf.AM_FRQ = FRQ_Temp;
				RADIO_Inf.CMD = 2;
				return 0;
			}
		}
		else return 1;
	}
	return 0;
}
//
//CMD_8301	�ظ���������Ϣ
u8 CMD_8301(u8 Sta)
{
#ifdef RDS_ENABLE
#else
	u16 temp = 0;
#endif
	if((Sta!=0)&&(Sta!=1)&&(Sta!=2)&&(Sta!=3)&&(Sta!=4)&&(Sta!=5)) return 0;	//��Ч����(0:��ת����������������ѯ�����1�������У�2���������,3��ȫ���������ѵ�̨,4��ȫ��������,5ȫ����������)
	if(RADIO_Inf.WORK_STA == 0) return 0;		//�������ر�
	Frame_Buff[CMD8301][10] = RADIO_Inf.MODE;	//������ģʽ
	if(RADIO_Inf.WORK_STA==2)//����
	{
		Frame_Buff[CMD8301][11] = 4;	//������״̬������
		Frame_Buff[CMD8301][12] = 0xFF;	//Ƶ�ʸ�λ
		Frame_Buff[CMD8301][13] = 0xFF;	//Ƶ�ʵ�λ
	}
	else
	{  
		if(RADIO_Inf.MODE == 1)
		{
			Frame_Buff[CMD8301][12] = (u8)(RADIO_Inf.AM_FRQ>>8)&0xFF;	//Ƶ�ʸ�λ
			Frame_Buff[CMD8301][13] = (u8)(RADIO_Inf.AM_FRQ&0xFF);		//Ƶ�ʵ�λ		
		}
		else
		{
#ifdef RDS_ENABLE
			Frame_Buff[CMD8301][12] = (u8)(RADIO_Inf.FM_FRQ>>8)&0xFF;	//Ƶ�ʸ�λ
			Frame_Buff[CMD8301][13] = (u8)(RADIO_Inf.FM_FRQ&0xFF);		//Ƶ�ʵ�λ
#else
			Frame_Buff[CMD8301][12] = (u8)((RADIO_Inf.FM_FRQ/10)>>8)&0xFF;	//Ƶ�ʸ�λ
			Frame_Buff[CMD8301][13] = (u8)((RADIO_Inf.FM_FRQ/10)&0xFF);		//Ƶ�ʵ�λ
#endif
		}
		switch(Sta)
		{
			case 0:Frame_Buff[CMD8301][11] = RADIO_Inf.SIGQUA;break;	//������״̬	���ź�״̬
			case 1:Frame_Buff[CMD8301][11] = 2;break;					//������״̬	��������
			case 2:Frame_Buff[CMD8301][11] = 3;break;					//������״̬	���������
			case 3:Frame_Buff[CMD8301][11] = 5;break;					//������״̬	���ѵ�̨
			case 4:Frame_Buff[CMD8301][11] = 6;break;					//������״̬	��������
			case 5:Frame_Buff[CMD8301][11] = 7;break;					//������״̬	����������
			default:break;
		}
	}
	//֪ͨ����
#ifdef RDS_ENABLE
	TX_TaskADD(CMD8301,1,TX_oTime,0);
#else
	TX_TaskADD(CMD8301,TX_Cnt,TX_oTime,0);
#endif
	return 0;
}
//

u8 CMD_8304(u8 Sta)
{
	if(RADIO_Inf.RDS_OK == 0)return 0;
	RADIO_Inf.RDS_OK = 0;
	Frame_Buff[CMD8304][10] = (u8)(RADIO_Inf.RDS_Sta);
	
	Frame_Buff[CMD8304][11] = (u8)(RADIO_Inf.RDS_Block[2]);
	Frame_Buff[CMD8304][12] = (u8)(RADIO_Inf.RDS_Block[3]);
	Frame_Buff[CMD8304][13] = (u8)(RADIO_Inf.RDS_Block[4]);
	Frame_Buff[CMD8304][14] = (u8)(RADIO_Inf.RDS_Block[5]);
	Frame_Buff[CMD8304][15] = (u8)(RADIO_Inf.RDS_Block[6]);
	Frame_Buff[CMD8304][16] = (u8)(RADIO_Inf.RDS_Block[7]);
	Frame_Buff[CMD8304][17] = (u8)(RADIO_Inf.RDS_Block[8]);
	Frame_Buff[CMD8304][18] = (u8)(RADIO_Inf.RDS_Block[9]);
	
	TX_TaskADD(CMD8304,1,TX_oTime,0);
	return 0;
}





//CANͶӰ
static u8 CMD_0401(u8 *Dat);
static u8 CMD_0402(u8 *Dat);
static u8 CMD_0403(u8 *Dat);
static u8 CanPro_explain(u8 Cmd_L,u8 *Dat,u8 err)
{
	u8 re = 0;
	switch(Cmd_L)	//ȫ����Ҫִ��CANת��
	{
		case 0x01:re = CMD_0401(Dat);break;	//��Դ��Ϣ Ӧ��
		case 0x02:re = CMD_0402(Dat);break;	//�����豸 Ӧ��
		case 0x03:re = CMD_0403(Dat);break;	//ͨ����Ϣ Ӧ��
		default:re = err;break;
	}

	if(0 == re) return 0;
	else return err;
}
//
//��Դ��ϢͶӰ
static u8 CMD_0401(u8 *Dat)
{
	//CANת��
	
	if(Dat[0]==3)Dat[0] = 2;
	VOICE_Inf.source = Dat[0];//����Դ
	VOICE_Inf.playsta = Dat[1];//����ģʽ
	VOICE_Inf.volume = Dat[2]&0x7F;//4.0-4.6����
	VOICE_Inf.mutesta = Dat[3]&0x01;//4.7
	VOICE_Inf.phonesta = Dat[4]&0x0F;//�绰״̬
	
	
	return 0;
	
/*	
	Can_SF	can_temp;
	u32 frq = 0;
	
	can_temp.id = 0x18FFB676;
	can_temp.data[0]=Dat[0];//����Դ
	can_temp.data[1]=Dat[1];//����ģʽ
	
	if(RADIO_Inf.MODE == 1)	frq = RADIO_Inf.AM_FRQ;
	else frq = (RADIO_Inf.FM_FRQ*100);
	can_temp.data[2]=(u8)((frq>>16)&0xFF);//�㲥Ƶ�� KHz
	can_temp.data[3]=(u8)((frq>>8)&0xFF);
	can_temp.data[4]=(u8)(frq&0xFF);
	can_temp.data[5]=Dat[2]&0x7F;//40-46���� 
	can_temp.data[5]|= (Dat[3]==1)?0x80:0x00;//47 ����
	can_temp.data[6]=Dat[4]&0x0F;//�绰״̬
	can_temp.data[7]=0xFF;
	
	Can_Write_TXMsg(can_temp,1);//���뻺���������ȼ�Ϊ1�������ȼ�
	
	return 0;
*/
//

}
//

//�����豸��ϢͶӰ
static u8 CMD_0402(u8 *Dat)
{
	//CANת��
//	Can_SF	can_temp;
	u8 i= 0;
//	u8 msg_l = 0;//��Ϣ����
//	u8 msg_n = 0;//����
	
	if((Dat[0]==3)&&(Dat[1]==0xff)&&(Dat[2]==0xff)&&(Dat[3]==0xff))
	{
		BLUE_Inf.sta = 0;
//		printf("***************************bluetooth OK\r\n");
		return 0;
	}
	BLUE_Inf.sta = 1;
	BLUE_Inf.name_len = Dat[0]+1;
	if(BLUE_Inf.name_len>33)BLUE_Inf.name_len=33;
	for(i=0;i<(BLUE_Inf.name_len-1);i++)
	BLUE_Inf.name[i]=Dat[i+1];
	BLUE_Inf.name[BLUE_Inf.name_len-1]=0x01;
//	printf("***************************bluetooth OK\r\n");
	return 0;
	
	
//	msg_l = Dat[0]+1;
//	Dat[msg_l] = 0x01;//�����λ���־
//	msg_n = msg_l/7;
//	msg_n += ((msg_l%7)>0)?1:0;
//	
//	for(i=0;i<7;i++)
//	Dat[msg_l+1+i] = 0xFF;
	
	//�㲥��
//	can_temp.id = 0x18ECFF76;//�㲥��ַ
//	can_temp.data[0] = 32;//������
//	can_temp.data[1] = (u8)((msg_l>>8)&0xFF);//�ֽ���
//	can_temp.data[2] = (u8)(msg_l&0xFF);
//	can_temp.data[3] = msg_n;//����
//	can_temp.data[4] = (u8)((0xFF62>>24)&0xFF);//������PGN
//	can_temp.data[5] = (u8)((0xFF62>>16)&0xFF);
//	can_temp.data[6] = (u8)((0xFF62>>8)&0xFF);
//	can_temp.data[7] = (u8)(0xFF62&0xFF);
//	
//	Can_Write_TXMsg(can_temp,1);//���뻺���������ȼ�Ϊ1�������ȼ�
//	
//	//���ݰ�
//	can_temp.id = 0x18FF6276;//�㲥��ַ
//	
//	for(i=1;i<msg_l;i++)
//	{
//		can_temp.data[0] = i/7;//���
//		if((i%7)==0)
//		{
//			can_temp.data[7] = Dat[i];//�ֽ���
//			Can_Write_TXMsg(can_temp,1);//���뻺���������ȼ�Ϊ1�������ȼ�
//		}
//		else
//		can_temp.data[i%7] = Dat[i];//�ֽ���
//	}
//	if((i%7)==0)can_temp.data[7] = 0x01;//�����λ���־
//	else can_temp.data[(i++)%7] = 0x01;//�����λ���־
//	while((i%7)!=0)
//	{
//		can_temp.data[i%7] = 0xFF;//��0xFF
//		i++;
//	}
//	can_temp.data[7] = 0xFF;//��0xFF
//	can_temp.data[0] = msg_n;//���
//	Can_Write_TXMsg(can_temp,1);//���뻺���������ȼ�Ϊ1�������ȼ�
	
//	return 0;
}
//

//ͨ����ϢͶӰ
static u8 CMD_0403(u8 *Dat)
{
	//CANת��
	u8 i = 0;
	
	PHONE_Inf.num_len = Dat[0]+1;//����λ��
	PHONE_Inf.name_len = Dat[1]+1;//����λ��
	
	if(PHONE_Inf.num_len>33)PHONE_Inf.num_len = 33;
	if(PHONE_Inf.name_len>33)PHONE_Inf.name_len = 33;
	
	for(i=0;i<(PHONE_Inf.num_len-1);i++)
	PHONE_Inf.number[i] = Dat[i+2];
	PHONE_Inf.number[i] = 0x03;

	if(PHONE_Inf.name_len == 1)
	{
		PHONE_Inf.name_len = PHONE_Inf.num_len;
		for(i=0;i<(PHONE_Inf.name_len - 1);i++)
		PHONE_Inf.name[i] = Dat[i+2];
		PHONE_Inf.name[i] = 0x03;
	}
	else
	{
		for(i=0;i<(PHONE_Inf.name_len-1);i++)
		PHONE_Inf.name[i] = Dat[i+Dat[0]+2];
		PHONE_Inf.name[i] = 0x01;
	}
	PHONE_Inf.sta = 1;

//	printf("***************************phone OK\r\n");
	return 0;
	
//	Can_SF	can_temp;
//	u8 i= 0;
//	u8 num_msg_l = 0;//��Ϣ����
//	u8 nam_msg_l = 0;//��Ϣ����
//	u8 num_msg_n = 0;//����
//	u8 nam_msg_n = 0;//����
//	
//	num_msg_l = Dat[0]+1;
//	nam_msg_l = Dat[1]+1;

//	num_msg_n = num_msg_l/7;
//	nam_msg_n = nam_msg_l/7;
//	num_msg_n += ((num_msg_l%7)>0)?1:0;
//	nam_msg_n += ((nam_msg_l%7)>0)?1:0;
//	
//	//�㲥��
//	can_temp.id = 0x18ECFF76;//�㲥��ַ
//	can_temp.data[0] = 32;//������
//	can_temp.data[1] = (u8)((num_msg_l>>8)&0xFF);//�ֽ���
//	can_temp.data[2] = (u8)(num_msg_l&0xFF);
//	can_temp.data[3] = num_msg_n;//����
//	can_temp.data[4] = (u8)((0xFF63>>24)&0xFF);//������PGN
//	can_temp.data[5] = (u8)((0xFF63>>16)&0xFF);
//	can_temp.data[6] = (u8)((0xFF63>>8)&0xFF);
//	can_temp.data[7] = (u8)(0xFF63&0xFF);
//	
//	Can_Write_TXMsg(can_temp,1);//���뻺���������ȼ�Ϊ1�������ȼ�
//	
//	//���ݰ�������
//	can_temp.id = 0x18FF6376;//��ַ
//	
//	for(i=1;i<num_msg_l;i++)
//	{
//		can_temp.data[0] = i/7;//���
//		if((i%7)==0)
//		{
//			can_temp.data[7] = Dat[i+1];//�ֽ���
//			Can_Write_TXMsg(can_temp,1);//���뻺���������ȼ�Ϊ1�������ȼ�
//		}
//		else
//		can_temp.data[i%7] = Dat[i+1];//�ֽ���
//	}
//	if((i%7)==0)can_temp.data[7] = 0x01;//�����λ���־
//	else can_temp.data[(i++)%7] = 0x01;//�����λ���־
//	while((i%7)!=0)
//	{
//		can_temp.data[i%7] = 0xFF;//��0xFF
//		i++;
//	}
//	can_temp.data[7] = 0xFF;//��0xFF
//	can_temp.data[0] = num_msg_n;//���
//	Can_Write_TXMsg(can_temp,1);//���뻺���������ȼ�Ϊ1�������ȼ�
//	
//	//�㲥��
//	can_temp.id = 0x18ECFF76;//�㲥��ַ
//	can_temp.data[0] = 32;//������
//	can_temp.data[1] = (u8)((nam_msg_l>>8)&0xFF);//�ֽ���
//	can_temp.data[2] = (u8)(nam_msg_l&0xFF);
//	can_temp.data[3] = nam_msg_n;//����
//	can_temp.data[4] = (u8)((0xFF64>>24)&0xFF);//������PGN
//	can_temp.data[5] = (u8)((0xFF64>>16)&0xFF);
//	can_temp.data[6] = (u8)((0xFF64>>8)&0xFF);
//	can_temp.data[7] = (u8)(0xFF64&0xFF);
//	
//	Can_Write_TXMsg(can_temp,1);//���뻺���������ȼ�Ϊ1�������ȼ�
//	
//	//���ݰ�������
//	can_temp.id = 0x18FF6476;//��ַ
//	
//	for(i=1;i<nam_msg_l;i++)
//	{
//		can_temp.data[0] = i/7;//���
//		if((i%7)==0)
//		{
//			can_temp.data[7] = Dat[i+num_msg_l];//�ֽ���
//			Can_Write_TXMsg(can_temp,1);//���뻺���������ȼ�Ϊ1�������ȼ�
//		}
//		else
//		can_temp.data[i%7] = Dat[i+num_msg_l];//�ֽ���
//	}
//	if((i%7)==0)can_temp.data[7] = 0x01;//�����λ���־
//	else can_temp.data[(i++)%7] = 0x01;//�����λ���־
//	while((i%7)!=0)
//	{
//		can_temp.data[i%7] = 0xFF;//��0xFF
//		i++;
//	}
//	can_temp.data[7] = 0xFF;//��0xFF
//	can_temp.data[0] = nam_msg_n;//���
//	Can_Write_TXMsg(can_temp,1);//���뻺���������ȼ�Ϊ1�������ȼ�
//	

//	return 0;
}
//


UPGRADE UP_Data={0,0,0,0,0,0,0,0x0801B800,0};
u8 CMD_0500(u8 *Dat);
u8 CMD_0501(u8 *Dat);
static u8 up_explain(u8 Cmd_L,u8 *Dat,u8 err)
{
	u8 re = 0;
	switch(Cmd_L)	//�������ݽ���
	{
		case 0x00:re = CMD_0500(Dat);break;	//����������
		case 0x01:re = CMD_0501(Dat);break;	//������
		default:re = err;break;
	}

	if(0 == re) return 0;
	else return err;
	
}
//
//��������
u8 CMD_0500(u8 *Dat)
{
	UP_Data.EsDatLen = (Dat[0]<<24)+(Dat[1]<<16)+(Dat[2]<<8)+(Dat[3]<<0);
	if(UP_Data.EsDatLen<=71680)
	{
		Frame_Buff[CMD8500][10] = 0;
		UP_Data.AcDatLen = 0;
		UP_Data.Dat_write = 0;
		UP_Data.L_FN = 0;
		UP_Data.FN = 0;
		UP_Data.MSG = 0;
		UP_Data.Save_AD = 0x0801B800;
		SYS_Inf.STA = 3;
		UP_Data.STA = 1;
	}
	else 
	{
		Frame_Buff[CMD8500][10] = 1;
	}
	TX_TaskADD(CMD8500,1,TX_oTime,0);
	return 0;
}
//

//�������ݰ�����
void CMD_8503(u8 result);
extern void cl_upcnt(void);
u8 CMD_0501(u8 *Dat)
{
	u16 i = 0;
	u8 Len = 0;
	
	if(UP_Data.STA == 0) return 1;//��������ģʽ��
	
	TX_TaskCancel(CMD8501);
	TX_TaskCancel(CMD8502);
	
	cl_upcnt();//�������ģʽ���ּ�����
	
	UP_Data.FN = (u16)(Dat[2]<<8)+Dat[3];//��ǰ���հ���ţ�
	if(UP_Data.FN == 1)//��һ��
	{
		UP_Data.MSG = (u16)(Dat[0]<<8)+Dat[1];
	}
	if(UP_Data.FN == (UP_Data.L_FN+1))
	{
		UP_Data.L_FN = UP_Data.FN;//��¼�µ����
		Len = Dat[4];
		for(i=0;i<Len;i++)
		{
			if((UP_Data.Dat_write%2)==0)
			{
				UP_Data.Data[UP_Data.Dat_write/2] &= 0xFF00;
				UP_Data.Data[UP_Data.Dat_write/2] |= Dat[5+i];
			}
			else
			{
				UP_Data.Data[UP_Data.Dat_write/2] &= 0x00FF;
				UP_Data.Data[UP_Data.Dat_write/2] |= Dat[5+i]<<8;
			}
			UP_Data.Dat_write++;
			UP_Data.AcDatLen++;//�����ֽ�������
			if(UP_Data.Dat_write==2048)
			{
				//��������дһ��
				IAP_write(UP_Data.Save_AD,UP_Data.Data);
				UP_Data.Save_AD += 2048;
				UP_Data.Dat_write = 0;
			}
		}
		if(UP_Data.FN==UP_Data.MSG)//������ɣ�
		{
			if(UP_Data.Dat_write != 0)//ûд�꣬ʣ��һ�㣬����д��ȥ
			{
				IAP_write(UP_Data.Save_AD,UP_Data.Data);
			}
			if(UP_Data.AcDatLen==UP_Data.EsDatLen)//У�鳤��
			{   
				//У��ͨ������������
				
				for(i=0;i<1024;i++)UP_Data.Data[i] = 0xFFFF;	//�����
				UP_Data.Data[0] = 0x0001;						//������־
				UP_Data.Data[1] = (u16)((UP_Data.AcDatLen>>16)&0xffff);//����������
				UP_Data.Data[2] = (u16)(UP_Data.AcDatLen&0xffff);
				IAP_write(0x0803F800,UP_Data.Data);//������Ϣд��ȥ
				UP_Data.STA = 3;
			}	
			else
			{				
				UP_Data.STA = 4;//У��ʧ�ܣ��˳�����
			}
		}
	}
	else //����Ų��ԣ�
	{
	UP_Data.ssst ++;
	}
	CMD_8502(0);
	return 0;
}


void CMD_8501(void)
{
	TX_TaskADD(CMD8501,5,TX_oTime,0);
}

void CMD_8502(u8 Mode)
{
	Frame_Buff[CMD8502][10] = (u8)((UP_Data.FN>>8)&0xFF);
	Frame_Buff[CMD8502][11] = (u8)(UP_Data.FN&0xFF);
	Frame_Buff[CMD8502][12] = Mode;
	if(UP_Data.STA == 2)	TX_TaskADD(CMD8502,1,10,0);
	else 					TX_TaskADD(CMD8502,1,10,0);
}
//
//�������������ս����0��ȷ��1����
//
void CMD_8503(u8 result)
{
	Frame_Buff[CMD8503][14] = result;//У��ʧ�ܣ��˳�����
	Frame_Buff[CMD8503][10] = (u8)((UP_Data.AcDatLen>>24)&0xFF);
	Frame_Buff[CMD8503][11] = (u8)((UP_Data.AcDatLen>>16)&0xFF);
	Frame_Buff[CMD8503][12] = (u8)((UP_Data.AcDatLen>>8)&0xFF);
	Frame_Buff[CMD8503][13] = (u8)(UP_Data.AcDatLen&0xFF);
	TX_TaskADD(CMD8503,1,50,0);//��ʱ���ͣ���һ��,500ms����
}


//��������ģʽ


u8 CMD_0600(u8 *Dat);
u8 CMD_0610(u8 *Dat);
u8 CMD_0620(u8 *Dat);
u8 CMD_0630(u8 *Dat);
u8 CMD_0640(u8 *Dat);
u8 CMD_0650(u8 *Dat);
u8 CMD_0660(u8 *Dat);
u8 CMD_0670(u8 *Dat);
u8 CMD_0680(u8 *Dat);
u8 CMD_8660(void);
u8 CMD_8661(void);
static u8 factorymode_explain(u8 Cmd_L,u8 *Dat,u8 err)
{
	u8 re = 0;

#ifdef FKY31
	if((FACTOR_Inf.sta!=1)&&(Cmd_L!=0)&&(Cmd_L!=0x50)&&(Cmd_L!=0x70)&&(Cmd_L!=0x80)&&(Cmd_L!=0x40))return 0;
#else	
	if((FACTOR_Inf.sta!=1)&&(Cmd_L!=0)&&(Cmd_L!=0x50)&&(Cmd_L!=0x70))return 0;
#endif	
	
	switch(Cmd_L)	//
	{
		case 0x00:re = CMD_0600(Dat);break;	//�������ģʽ
		case 0x10:re = CMD_0610(Dat);break;	//EEPROM����
		case 0x20:re = CMD_0620(Dat);break;	//Radio����
		case 0x30:re = CMD_0630(Dat);break;	//CAN���ݷ���
		case 0x40:re = CMD_0640(Dat);break;	//�������ÿ���
		case 0x50:re = CMD_0650(Dat);break;	//CAN���ݷ���2
		case 0x60:re = CMD_8660();break;	//�ڲ��汾������
		case 0x61:re = CMD_8661();break;	//�������汾������
		case 0x70:re = CMD_0670(Dat);break;
		
		case 0x80:re = CMD_0680(Dat);break;
		
		default:re = err;break;
	}

	if(0 == re) return 0;
	else return err;
}

u8 CMD_8600(u8 result);
u8 CMD_8610(u8 result);
u8 CMD_8621(u8 mode,u32 ss);
u8 CMD_8622(u8 mode,u32 stnr);
u8 CMD_8623(u8 mode,u32 fres);
u8 CMD_8630(u32 ID,u8 result);

u8 CMD_0600(u8 *Dat)//�������ģʽ
{
	if(Dat[0])
	{
		if(CAR_Inf.ACC)
		{
			FACTOR_Inf.sta = 1;
			CMD_8600(0);
		}
		else
		{
			FACTOR_Inf.sta = 0;
			CMD_8600(1);
		}
	}
	else
	{
		if(1 == FACTOR_Inf.sta)
		{
			FACTOR_Inf.sta = 0;
			CMD_8600(0);
		}
		else
		{
			CMD_8600(1);
		}
	}
	return 0;	
}



u8 CMD_0610(u8 *Dat)//EEPROM����
{
	u8 re = 0;
	if(AT24CXX_ReadOneByte(255)!=EEPROM_FLAG)//����
	{
		if(AT24CXX_ReadOneByte(255)!=EEPROM_FLAG)re = 0;//����һ�Σ�Ҳ��
		else re = 1;
	}
	else re = 1;
	
	if(re == 1)	CMD_8610(0);//����
	else		CMD_8610(1);//OK
	return 0;
	
}



u8 CMD_0620(u8 *Dat)//Radio����
{
//Ƶ����ת
	u16 FRQ_Temp = 0;
	if(RADIO_Inf.CMD==0)//æ�ж�
	{
		RADIO_Inf.MODE = Dat[0];
		FRQ_Temp = (Dat[1]<<8)+Dat[2];
//		FRQ_Temp = 9410;
		if((0==RADIO_Inf.MODE)&&( (FRQ_Temp>=Frequency[0])&&(FRQ_Temp<=Frequency[1]) )) 
		{
			RADIO_Inf.FM_FRQ = FRQ_Temp;
			RADIO_Inf.CMD = 8;
			return 0;
		}
		else if((1==RADIO_Inf.MODE)&&( (FRQ_Temp>=Frequency[2])&&(FRQ_Temp<=Frequency[3]) )) 
		{
			RADIO_Inf.AM_FRQ = FRQ_Temp;
			RADIO_Inf.CMD = 8;
			return 0;
		}
		else return 1;
	}
	return 0;
}
void MPU2CAN_Write(Can_SF tx);
void APP2CAN_Write(Can_SF tx);
u8 CMD_0630(u8 *Dat)//CAN���ݷ���
{
	Can_SF temp;
	temp.id = (u32)((Dat[0]<<24)|(Dat[1]<<16)|(Dat[2]<<8)|(Dat[3]));
	temp.data[0] = Dat[4];
	temp.data[1] = Dat[5];
	temp.data[2] = Dat[6];
	temp.data[3] = Dat[7];
	temp.data[4] = Dat[8];
	temp.data[5] = Dat[9];
	temp.data[6] = Dat[10];
	temp.data[7] = Dat[11];
	MPU2CAN_Write(temp);
	return 0;
}
extern u8 LA_CMD_83(u8 cmds);
u8 CMD_0640(u8 *Dat)//
{
#ifdef LOOK_AROUND
	
#ifdef TW2836
	if(Dat[0] == 3)TW2836_Switch(4);//�ķ�
#else 
	switch(Dat[0])
	{
		case 0: break;
		case 1:LA_CMD_83(1);break;//��ͼ
		case 2:LA_CMD_83(2);break;//�궨
		case 3:LA_CMD_83(3);break;//�ķ���
		case 4:LA_CMD_83(4);break;//���
		case 5:LA_CMD_83(5);break;//����
		case 6:LA_CMD_83(6);break;//����
		case 7:LA_CMD_83(7);break;//����
		
		default: break;
	}
#endif
#endif
	return 0;
}



u8 CMD_0650(u8 *Dat)//CAN���ݷ���
{
	Can_SF temp;
	temp.id = (u32)((Dat[0]<<24)|(Dat[1]<<16)|(Dat[2]<<8)|(Dat[3]));
	temp.data[0] = Dat[4];
	temp.data[1] = Dat[5];
	temp.data[2] = Dat[6];
	temp.data[3] = Dat[7];
	temp.data[4] = Dat[8];
	temp.data[5] = Dat[9];
	temp.data[6] = Dat[10];
	temp.data[7] = Dat[11];
	APP2CAN_Write(temp);
	return 0;
}


u8 CMD_8600(u8 result)//���빤��ģʽ���
{
	Frame_Buff[CMD8600][10] = (u8)result;
	TX_TaskADD(CMD8600,TX_Cnt,TX_oTime,0);
	return 0;
}


u8 CMD_8610(u8 result)//EEPROM���Խ��
{
	Frame_Buff[CMD8610][10] = (u8)result;
	TX_TaskADD(CMD8610,TX_Cnt,TX_oTime,0);
	return 0;
}


u8 CMD_8621(u8 mode,u32 ss)//̨ͣ������
{
	Frame_Buff[CMD8621][10] = (u8)mode;
	Frame_Buff[CMD8621][11] = (u8)((ss>>8)&0xFF);
	Frame_Buff[CMD8621][12] = (u8)((ss>>0)&0xFF);
//	Frame_Buff[CMD8621][11] = (u8)((ss>>24)&0xFF);
//	Frame_Buff[CMD8621][12] = (u8)((ss>>16)&0xFF);
//	Frame_Buff[CMD8621][13] = (u8)((ss>>8)&0xFF);
//	Frame_Buff[CMD8621][14] = (u8)((ss>>0)&0xFF);
	TX_TaskADD(CMD8621,TX_Cnt,TX_oTime,0);
	return 0;
}


u8 CMD_8622(u8 mode,u32 stnr)//�����
{
	Frame_Buff[CMD8622][10] = (u8)mode;
	Frame_Buff[CMD8622][11] = (u8)((stnr>>8)&0xFF);
	Frame_Buff[CMD8622][12] = (u8)((stnr>>0)&0xFF);
//	Frame_Buff[CMD8622][11] = (u8)((stnr>>24)&0xFF);
//	Frame_Buff[CMD8622][12] = (u8)((stnr>>16)&0xFF);
//	Frame_Buff[CMD8622][13] = (u8)((stnr>>8)&0xFF);
//	Frame_Buff[CMD8622][14] = (u8)((stnr>>0)&0xFF);
	TX_TaskADD(CMD8622,TX_Cnt,TX_oTime,0);
	return 0;
}


u8 CMD_8623(u8 mode,u32 fres)//Ƶ����Ӧ
{
	Frame_Buff[CMD8623][10] = (u8)mode;
	Frame_Buff[CMD8623][11] = (u8)((fres>>8)&0xFF);
	Frame_Buff[CMD8623][12] = (u8)((fres>>0)&0xFF);
//	Frame_Buff[CMD8623][11] = (u8)((fres>>24)&0xFF);
//	Frame_Buff[CMD8623][12] = (u8)((fres>>16)&0xFF);
//	Frame_Buff[CMD8623][13] = (u8)((fres>>8)&0xFF);
//	Frame_Buff[CMD8623][14] = (u8)((fres>>0)&0xFF);
	TX_TaskADD(CMD8623,TX_Cnt,TX_oTime,0);
	return 0;
}


u8 CMD_8630(u32 ID,u8 result)//CAN���ͽ��
{
	Frame_Buff[CMD8630][10] = (u8)((ID>>24)&0xFF);
	Frame_Buff[CMD8630][11] = (u8)((ID>>16)&0xFF);
	Frame_Buff[CMD8630][12] = (u8)((ID>>8)&0xFF);
	Frame_Buff[CMD8630][13] = (u8)((ID>>0)&0xFF);
	
	Frame_Buff[CMD8630][14] = (u8)result;
	TX_TaskADD(CMD8630,TX_Cnt,TX_oTime,0);
	return 0;
}


u8 CMD_8631(u32 ID,u8 *dat)//CAN����ת��
{
	Frame_Buff[CMD8631][10] = (u8)((ID>>24)&0xFF);
	Frame_Buff[CMD8631][11] = (u8)((ID>>16)&0xFF);
	Frame_Buff[CMD8631][12] = (u8)((ID>>8)&0xFF);
	Frame_Buff[CMD8631][13] = (u8)((ID>>0)&0xFF);
	
	Frame_Buff[CMD8631][14] = (u8)(dat[0]);
	Frame_Buff[CMD8631][15] = (u8)(dat[1]);
	Frame_Buff[CMD8631][16] = (u8)(dat[2]);
	Frame_Buff[CMD8631][17] = (u8)(dat[3]);
	Frame_Buff[CMD8631][18] = (u8)(dat[4]);
	Frame_Buff[CMD8631][19] = (u8)(dat[5]);
	Frame_Buff[CMD8631][20] = (u8)(dat[6]);
	Frame_Buff[CMD8631][21] = (u8)(dat[7]);
	
	TX_TaskADD(CMD8631,TX_Cnt,TX_oTime,0);
	return 0;
}


u8 CMD_8640(u8 mode,u8 result)//���ӽ����
{
	if(mode == 0) return 0;
	if(mode > 10) return 0;
	if(result > 1) return 0;
	
	Frame_Buff[CMD8640][10] = (u8)(mode);
	Frame_Buff[CMD8640][11] = (u8)(result);
	
	TX_TaskADD(CMD8640,TX_Cnt,TX_oTime,0);
	return 0;
}

u8 CMD_8650(u32 ID,u8 result)//CAN���ͽ��
{
	Frame_Buff[CMD8650][10] = (u8)((ID>>24)&0xFF);
	Frame_Buff[CMD8650][11] = (u8)((ID>>16)&0xFF);
	Frame_Buff[CMD8650][12] = (u8)((ID>>8)&0xFF);
	Frame_Buff[CMD8650][13] = (u8)((ID>>0)&0xFF);
	
	Frame_Buff[CMD8650][14] = (u8)result;
	TX_TaskADD(CMD8650,TX_Cnt,TX_oTime,0);
	return 0;
}
u8 CMD_8651(u32 ID,u8 *dat)//CAN����
{
	Frame_Buff[CMD8651][10] = (u8)((ID>>24)&0xFF);
	Frame_Buff[CMD8651][11] = (u8)((ID>>16)&0xFF);
	Frame_Buff[CMD8651][12] = (u8)((ID>>8)&0xFF);
	Frame_Buff[CMD8651][13] = (u8)((ID>>0)&0xFF);
	                
	Frame_Buff[CMD8651][14] = (u8)(dat[0]);
	Frame_Buff[CMD8651][15] = (u8)(dat[1]);
	Frame_Buff[CMD8651][16] = (u8)(dat[2]);
	Frame_Buff[CMD8651][17] = (u8)(dat[3]);
	Frame_Buff[CMD8651][18] = (u8)(dat[4]);
	Frame_Buff[CMD8651][19] = (u8)(dat[5]);
	Frame_Buff[CMD8651][20] = (u8)(dat[6]);
	Frame_Buff[CMD8651][21] = (u8)(dat[7]);
	
	TX_TaskADD(CMD8651,1,TX_oTime,0);
	return 0;
}

u8 CMD_8660(void)
{
	Frame_Buff[CMD8660][10]=(u8)(Version_SIZE);
	for(u8 i=0;i<Version_SIZE;i++)
	Frame_Buff[CMD8660][11+i] = (u8)(*(Version+i));
	//֡���ȡ�LENװ�أ�
	Frame_Buff[CMD8660][0] = 11+Version_SIZE;
	Frame_Buff[CMD8660][7] = 3+Version_SIZE;
	TX_TaskADD(CMD8660,1,TX_oTime,0);
	return 0;
}

u8 CMD_8661(void)
{
	u8 len = RVersion_SIZE;
	if(len >30)len = 30;
	Frame_Buff[CMD8661][10]=(u8)(RVersion_SIZE);
	
	for(u8 i=0;i<len;i++)
	Frame_Buff[CMD8661][11+i] = (u8)(radio_version[i]);
	//֡���ȡ�LENװ�أ�
	Frame_Buff[CMD8661][0] = 11+len;
	Frame_Buff[CMD8661][7] = 3+len;
	TX_TaskADD(CMD8661,1,TX_oTime,0);
	return 0; 
}

u8 CMD_8670(u8 cmdlen,u8 *cd)
{
	Frame_Buff[CMD8670][0] = 11 + cmdlen;
	Frame_Buff[CMD8670][7] = 3 + cmdlen;
	Frame_Buff[CMD8670][10] = cmdlen;
	
	for(u8 i=0;i<cmdlen;i++)
	Frame_Buff[CMD8670][11+i] = cd[i];
	
	TX_TaskADD(CMD8670,1,0,0);
	return 0;
}



u8 CMD_8680(void)
{
	Frame_Buff[CMD8680][10] = FAULT_Inf.BUSOFF_Flag;
	TX_TaskADD(CMD8680,1,0,0);
	return 0;
}



#include "TestCMD.h"
u8 CMD_0670(u8 *Dat)//
{
	_cache_frame temp;
	temp.len = UartRX.LEN + 4;//��������
	for(u8 i=0;i<(UartRX.LEN-2);i++)
	{
		temp.dat[i+4] = Dat[i];
	}
	Write_sendcache(temp);
	return 0;
}

u8 CMD_0680(u8 *Dat)//
{
	TW2836_Adjust(Dat[0],Dat[1]);
	return 0;
}








