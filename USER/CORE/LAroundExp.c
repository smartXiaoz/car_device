
#include "hardware.h"
#include "LAroundExp.h"

//��������

#define LA_CheckSun	0		//��У��ͼ��

///
#define LA_UART_SYNC	0xA55A			//ͬ��ͷ
#define LA_TX_oTime		100				//�ȴ��ط�ʱ��
#define LA_TX_Cnt		1				//���ʹ���
#define LA_DataSend		Uart2_TxBuff	//���巢�ʹ����뺯��

#define LA_Frame_Cap	3		//��Ϣ��
//֡���
#define LA_CMD81	0	//����	
#define LA_CMD82	1	//����
#define LA_CMD83	2	//����


static u8 LA_ACK_Buff[7]={6,0xA5,0x5A,0,0,0,0};	//Ӧ��֡,��һ���ֽڴ��֡����
static u8  LA_Frame_Buff[LA_Frame_Cap][10]={	//֡���ݻ���,��һ���ֽڴ��֡���ȣ�ÿһ֡λ�ñ�����֡��Ŷ�Ӧ
										{6, 0xA5,0x5A, 0x01, 1,0x81, 0,0},	//2+8	����	
										{7, 0xA5,0x5A, 0x01, 2,0x82, 0,0},	//2+8	����	
										{7, 0xA5,0x5A, 0x01, 2,0x83, 0,0},	//2+5	��ͼ���궨
										};

static u8 LA_ACK_Task = 0;		//ACK����
								
typedef struct{
	u16 SYNC;		//ͬ��ͷ
	u8  ACK;		//Ӧ��
	u8  LEN;		//֡����
	u8  CMD;		//����
	u8  CHECKSUM;	//У���
	u8  PARAM[2];	//�������ݻ�������
}LA_UART_STA;
static LA_UART_STA	LA_UartRX;
								
//װ��У���
static void LA_CheckSun_load(u8 CH)
{
	u8 i = 0;
	u16 Temp = 0;
	//����У���
	for(i=3;i<(LA_Frame_Buff[CH][0]);i++) Temp += LA_Frame_Buff[CH][i];
	LA_Frame_Buff[CH][LA_Frame_Buff[CH][0]] = (u8)~(Temp);
}
//ACKװ��
static void LA_ACK_Load(u8 ACK_STA,u8 Cmd)
{
	u16 temp = 0;
	u8 i = 0;
	LA_ACK_Buff[3] = ACK_STA|0x80;			//Ӧ��״̬����Ӧ���Ӧ��
	LA_ACK_Buff[4] = 0x01;				//LEN
	LA_ACK_Buff[5] = Cmd;				//CMD
	//����У���
	for(i=3;i<6;i++) temp += LA_ACK_Buff[i];
	LA_ACK_Buff[6] = (u8)(~temp);		//У���
	//֪ͨ����
	LA_ACK_Task = 1;
}
//
//���ʹ�����
u8 LA_TX_Stabuff[LA_Frame_Cap][6]={0};//0������״̬��1����ʱ��ʱ����2����ʱʱ�䣬3���ط���������4�����ʹ�����5���ط�������
/**
*--���ܣ����һ�η�������
*--���룺CH ��֡���
*--		Cnt�����ʹ���
*--		Time����ʱʱ��
*--		Mode������ģʽ��0���������ͣ�1���ӳٷ���
*--���أ�0���ɹ���1��ʧ��
**/
u8 LA_TX_TaskADD(u8 CH,u8 Cnt,u8 Time,u8 Mode)
{
//	if(LAROUND_Inf.STA != 1) return 1;//���ߺ�Ԥ����ģʽ���ܾ���������ע���򻺴棩
	LA_CheckSun_load(CH);
//	if(TX_Stabuff[CH][0] == 1) return 3;//��������У����ʧ�ܣ�ע���򸲸�ԭ����
	if(Mode)LA_TX_Stabuff[CH][1] = 0;		//��ʱ������	�ӳٷ���
	else	LA_TX_Stabuff[CH][1] = Time;	//��ʱ������	��������
	LA_TX_Stabuff[CH][2] = Time;			//��ʱʱ��
	LA_TX_Stabuff[CH][4] = Cnt;			//���ʹ���
	LA_TX_Stabuff[CH][3] = 0;
	LA_TX_Stabuff[CH][0] = 1;				//�������
	return 0;
}
//
/**
*--���ܣ�ע��һ�������е�����
*--���룺CH ��֡���
*--���أ�0���ɹ���1��ʧ��
**/
u8 LA_TX_TaskCancel(u8 CH)
{
	if(LA_TX_Stabuff[CH][0] == 0) return 1;//����գ�
	LA_TX_Stabuff[CH][0] = 0;				//�������	
	LA_TX_Stabuff[CH][1] = 0;				//��ʱ�������
	LA_TX_Stabuff[CH][2] = 0;				//��ʱʱ�����
	LA_TX_Stabuff[CH][3] = 0;				//�ط����������
//	TX_Stabuff[CH][5] = 0;
	return 0;
}
//
/**
*--���ܣ�������������
*--���룺��
*--���أ�0��������
*--					
**/
u8 LA_TX_Process(void)
{
	u8 i = 0;
	
	if(LAROUND_Inf.STA == 0) return 0;//�������ߣ������з�������
	
	if(LA_ACK_Task == 1)//Ӧ����
		{LA_DataSend(LA_ACK_Buff);LA_ACK_Task = 0;return 0;}
	
	for(i=0;i<LA_Frame_Cap;i++)
	{
		if(LA_TX_Stabuff[i][0] == 1)	//�ҳ���Ҫ���͵�����
		{
			if(LA_TX_Stabuff[i][1]<LA_TX_Stabuff[i][2])LA_TX_Stabuff[i][1]++;	//��ʱ��ʱ
			else	//�ѳ�ʱ
			{
				LA_TX_Stabuff[i][1] = 0;//���¼�ʱ
				if(LA_TX_Stabuff[i][3]<LA_TX_Stabuff[i][4])	//δ������
				{
					//����
					LA_DataSend(LA_Frame_Buff[i]);
					LA_TX_Stabuff[i][3]++;
					if(LA_TX_Stabuff[i][3]>=LA_TX_Stabuff[i][4])//�Ѵ�����
					{
						LA_TX_Stabuff[i][0] = 0;	//���������
						LA_TX_Stabuff[i][1] = 0;	//��ʱ�������
						LA_TX_Stabuff[i][3] = 0;	//�ط����������
						LA_TX_Stabuff[i][4] = 0;
						LA_TX_Stabuff[i][5] = 1;	//�ط��ﵽ����
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

/**
*--���ܣ��������ݴ�������Э�������
*--���룺Msg���յ������ݳ���
*--		 Dat���յ�����������
*--���أ���������
*--
**/
u8 LA_ACK_Process(u16 CMD,u8 ACK);
static u8 LA_CMD_00(u8 *Dat);
void cl_ladtc(void);
static u8 LA_CMD_03(u8 *Dat);
u8 LA_UART_explain(u16 Msg,u8 *Dat)
{
	//����ֲ�����
	u16 temp = 0;	//����ͻ������
	u16 i = 0;		//
	u8 re = 0;		//�������ͱ��
	
	//���ݼ��������
	LA_UartRX.SYNC = (Dat[0]<<8)+Dat[1];	//���SYNC
	LA_UartRX.ACK = Dat[2];					//���ACK	
	LA_UartRX.LEN = Dat[3];					//���LEN
	LA_UartRX.CMD = Dat[4];					//���CMD_H
	//�������ͣ��޶�32�ֽڣ���ֹ���ݴ��������ѭ������Ч���ݲ��ᳬ��200��
	for(i=0;(i<(LA_UartRX.LEN+2)&&(i<32));i++) temp += Dat[i+2];
	LA_UartRX.CHECKSUM = (u8)~(temp);	
	
	//����У��
	if(LA_UartRX.SYNC != LA_UART_SYNC)	re = 1;	//֡ͬ��ͷУ��
	else if(LA_UartRX.LEN != (Msg-5))	re = 2;	//����У��
#if	LA_CheckSun
	if(LA_UartRX.CHECKSUM != Dat[msg-1])	re = 3;	//У���У��
#endif
	if(re != 0) return re;//У�鲻ͨ��������֡
		
	cl_ladtc();
		
	if((((LA_UartRX.ACK&0x7F) == 0x02)||((LA_UartRX.ACK&0x7F) == 0x03))&&(LA_UartRX.CMD&0x80))//�յ�Ӧ���ź�
	{
		re = LA_ACK_Process(LA_UartRX.CMD,(u8)(LA_UartRX.ACK&0x7F));//Ӧ���źŴ���
		return re;
	}
	
	
	//��������
	for(i=0;i<(LA_UartRX.LEN-1);i++) LA_UartRX.PARAM[i] = Dat[i+5];//��������
	//�����
	switch(LA_UartRX.CMD)
	{
		case 0x00:re =  LA_CMD_00(LA_UartRX.PARAM);;break;//����
		case 0x03:re =  LA_CMD_03(LA_UartRX.PARAM);break;//�궨���
		
		
		
		
		default:re = 12;break;
	}
	//Ӧ���ͣ�Ӧ����������ȼ���
	if((LA_UartRX.ACK&0x7F) == 0x01)
	{
		if(re == 0)	LA_ACK_Load(0x02,LA_UartRX.CMD);	//��Ӧ��0x02
		else		LA_ACK_Load(0x03,LA_UartRX.CMD);	//��Ӧ��0x03
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
u8 LA_ACK_Process(u16 CMD,u8 ACK)
{
	u8 i = 0;
	switch(CMD)//���CMD��Ӧ��֡��ţ�����ע���ط�����
	{
		case 0x81:i=0;break;
		case 0x82:i=1;break;

		default:i=255;break;
	}
	if(i==255)return 4;//CMD�����޷�����
	if(ACK == 0X02)	//��Ӧ��
	{
		LA_TX_TaskCancel(i);	//ע���ط�
	}
	else if(ACK == 0X03)//��Ӧ��
	{
		LA_TX_TaskCancel(i);	//ע���ط�
	}
	else //����Ӧ��
	{
		return 4;
	}
	return 0;
}
//


//����
//CMD_00
static u8 LA_CMD_00(u8 *Dat)
{
	//��ǻ�������
	LAROUND_Inf.STA = 1;
	
	LA_Frame_Buff[LA_CMD82][6] = LAROUND_Inf.VIS;	//ģʽ
	LA_TX_TaskADD(LA_CMD82,LA_TX_Cnt,LA_TX_oTime,1);//�ӳٷ��͵�ǰģʽ
	
	return 0;
}


//�궨���
extern u8 CMD_8640(u8 mode,u8 result);//�궨���
static u8 LA_CMD_03(u8 *Dat)
{
	CMD_8640(Dat[0],Dat[1]);
}


u8 LAHAERT_Cnt = 0;
void cl_ladtc(void)//MPU�������
{
	LAHAERT_Cnt = 0;
	FAULT_Inf.LOA = 0;
//	if(LAROUND_Inf.STA == 2)
		LAROUND_Inf.STA=1;
}
void LA_CMD_81(void)
{
	if(CAR_Inf.ACCDe == 0)//����û�ϵ�
	{
		LAHAERT_Cnt = 0;
		FAULT_Inf.LOA = 0;
		return ;
	}		
#ifdef	LOOK_AROUND	
#ifdef TW2836
	
#else
	LA_TX_TaskADD(LA_CMD81,1,LA_TX_oTime,0);
	if(LAHAERT_Cnt<20)LAHAERT_Cnt++;
	else 
	{
		LAROUND_Inf.STA = 2;
		FAULT_Inf.LOA = 1;//���DTC
	}
#endif
#endif	
}
//�ӽ��л�����
//CMD_82
u8 LA_CMD_82(u8 LAwark_Mod)
{
	LA_Frame_Buff[LA_CMD82][6] = LAwark_Mod;	//ģʽ

	//֪ͨ����
	LA_TX_TaskADD(LA_CMD82,LA_TX_Cnt,LA_TX_oTime,0);
	return 0;
}

//��ͼ���궨����
//CMD_83
u8 LA_CMD_83(u8 cmds)
{
	LA_Frame_Buff[LA_CMD83][6] = cmds;	//ģʽ

	//֪ͨ����
	LA_TX_TaskADD(LA_CMD83,LA_TX_Cnt,LA_TX_oTime,0);
	return 0;
}

