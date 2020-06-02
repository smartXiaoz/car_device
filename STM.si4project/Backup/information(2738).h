#ifndef __INFORMATION_H
#define __INFORMATION_H	
#include "sys.h"
#include "string.h"

#define		LOOK_AROUND

extern u8 PowerNormal,PowerLow,PowerHigh;

extern u32 Ignition_cnt;//ACC�ϵ��������,�ӳ�ʼ0��ʼ����Ƭ���ϵ������

//�źſ���	0�������շ���1����ֹ�շ�
typedef struct {
u8 send;
u8 receiver;
}__Signal_Control;
extern __Signal_Control		Signal_Control;


//	//FICM
//u8 ID18FEE6EE;	//���ͣ����ڣ�ʱ��ͬ����
//u8 ID18FFB676;	//���ͣ����ڣ�����Դ
//u8 ID18FF6276;	//���ͣ����ڣ�������Ϣ
//u8 ID18FF6376;	//���ͣ��¼���ͨ������
//u8 ID18FF6476;	//���ͣ��¼���ͨ������
//u8 ID18FFC676;	//���ͣ��¼������ÿ���
//	//BSM
//u8 ID18FEBF0B;	//���գ����ڣ�ǰ��ת��
//	//BCM
//u8 ID10FF1021;	//���գ����ڣ�ת���
//u8 ID18FF6FEE;	//���գ����ڣ�Ѱ���������г�������Ϩ���������״̬
//u8 ID08FF00DD;	//���գ����ڣ�ң����״̬
//u8 ID10FF7521;	//���գ����ڣ����š�Կ��״̬
//	//IC
//u8 ID10FF4117;	//���գ����ڣ������ơ����ٱ�������״̬
//	//DDSW
//u8 ID18A005E7;	//���գ����ڣ�ƣ�ͼ�ʻ����״̬
//	//BSD
//u8 ID1CA00701;	//���գ����ڣ�BSD����״̬
//	//TCU
//u8 ID18f00503;	//���գ����ڣ���λ�ź�



//������Ϣ	0��û�У�1����
typedef struct {
u8 LOA;			//���ӡ����� 0Ϊ���ӣ�
u8 LOC;			//Ѱ������
u8 DDWS;		//ƣ��Ԥ��
u8 BSD;			//ä����� 
}__SYS_Config;
extern __SYS_Config		SYS_Config;


//������Ϣ 0��������1������
typedef struct{
u8 BLTH;		//����	0/1 
u8 WIFI;		//WIFI	
u8 VOICE;		//����	
u8 LOA;			//����	
u8 RADIO;		//������	
u8 MPUDIED;		//����	
u8 BUSOFF;		//���߹رգ�0������������1�����߹ر�
u8 MISSABS;		//miss ABS
u8 MISSIC;		//miss IC
u8 MISSBCM;		//miss BCM
//u8 OVERVOL;		//��ѹ
//u8 UNDERVOL;	//��ѹ

}__FAULT_Inf;
extern __FAULT_Inf		FAULT_Inf;


//�������ÿ��� 0���أ�1����
typedef struct{
u8 OVERSPEE;	//���ٱ���	 
u8 CARSEARCH;	//Ѱ������	
u8 DRIVLOCK;	//�г�����	
u8 PARKULOCK;	//ͣ������
u8 DDWS;		//ƣ�ͼ�ʻ	
u8 BSD;			//ä����	
}__CAR_Set;
extern __CAR_Set		CAR_Set;


//������״̬��Ϣ
typedef struct{
u8 WORK_STA;	//����״̬	�ء��������ϡ�������	/0/1/2/3	
u8 MODE;		//����ģʽ	AM/FM 1/0	
u16 Last_FRQ;	//��̨��
u16 AM_FRQ;		//AMƵ��	531  -- 1602
u16 FM_FRQ;		//FMƵ��	875 -- 1080
u8 SIGQUA;		//�ź�����	0�����źţ�1�����ź�
u8 CMD;			//��������	0�����У�1��ģʽ�л���2����ת��ָ��Ƶ�ʣ�3����ѯ��ǰƵ�ʣ�4����ǰλ��������̨��5����ǰλ��������̨��6��ֹͣ��̨��7��ȫ����̨,8:����ģʽ��ת
u16 FM_LEVEL;	//fm̨ͣ������
u16 AM_LEVEL;	//am̨ͣ������
u16 FM_STNR;	//fm�����
u16 FM_FRES;	//fmƵ����Ӧ
}__RADIO_Inf;
extern __RADIO_Inf		RADIO_Inf;


//����״̬��Ϣ
typedef struct{
//u8 NUM;			//��Ϣ���
//u8 STA;			//״̬
u16 Batter_Value;//��ص�ѹ	
u8 NIGHTLIGHT;	//ҹ�ƿ�����������	0�����죬1��ҹ��
u8 ReverseSta;	//����״̬			0�������г���1������
u8 DIVEDOOR;	//��ʻ����			0���رգ�1����
u8 PSNGDOOR;	//����ʻ��			0���رգ�1����
u8 ACC;			//ACC״̬			0���µ磬1���ϵ�
u8 POWER;		//������Դ״̬		0��OFF��1��ACC��2��ON
u8 KEY_STS;		//Կ��λ��			1��Stop and Key absent��2��Reserved��4��ON��6��Stop and Key In��C��Reserved��E��Error��F��Not Avaliable������������
u8 LTurnSts;	//��ת���			0���أ�1����
u8 RTurnSts;	//��ת���			0���أ�1����
u16 Car_Speed;	//ǰ��ƽ���ٶ�
u8	Lock;		//����				0��������1���Ž�����2��������
}__CAR_Inf;
extern __CAR_Inf		CAR_Inf;


//ϵͳ״̬��Ϣ
typedef struct{
u8 STA;			//ϵͳ״̬	0�����ߣ�1�����ѹ��̣�2�����������У�3���������ݽ��չ����У�
u8 MPU_STA;		//MPU״̬   0���ȴ����ߣ�1���������ߣ�2��MPU����ػ���3��MPU�������ߣ�4���ػ�״̬��5������
u8 BATTER_VERR;	//��ѹ������־
u8 MUTE_STA;
}__SYS_Inf;
extern __SYS_Inf		SYS_Inf;


//����״̬��Ϣ
typedef struct{
u8 NUM;			//�������	8������
u8 STA;			//����״̬	0���ͷţ�1���̰���2������	
}__KEY_Inf;
extern __KEY_Inf		KEY_Inf;


//�����ӽ�״̬
//typedef enum{
//FRONT=1,		//ǰ
//AFFTER,			//��
//LEFT,			//��
//RIGHT,			//��
//}__LAROUND_Sta;
//extern __LAROUND_Sta	LAROUND_Sta;

//�����ӽ�״̬
typedef struct{
u8 STA;			//����״̬	0���ȴ����ߣ�1������������2������
u8 VIS;			//�ӽ�		0��ֹͣͼ�������1��ǰ��2����3����4���ң�
}__LAROUND_Inf;
extern __LAROUND_Inf	LAROUND_Inf;

//����Դ��Ϣ
typedef struct{
u8 source;		//Դ
u8 playsta;		//����״̬
u32 radiofrq;	//����Ƶ��
u8 volume;		//����
u8 mutesta;		//����
u8 phonesta;	//ͨ��״̬
}__VOICE_Inf;
extern __VOICE_Inf	VOICE_Inf;

//�����豸��Ϣ
typedef struct{
u8 sta;			//����״̬
u8 name_len;	//�豸������
u8 time;		//���ͼ�ʱ��	
u8 name[32];	//�豸��
}__BLUE_Inf;
extern __BLUE_Inf	BLUE_Inf;

//ͨ����Ϣ
typedef struct{
u8 sta;			//״̬
u8 time;	
u8 name_len;	//�豸������
u8 num_len;	//�豸������
u8 name[33];	//�豸��
u8 number[33];	//�豸��
}__PHONE_Inf;
extern __PHONE_Inf	PHONE_Inf;

//����ģʽ��Ϣ
typedef struct{
u8 sta;			//״̬

}__FACTOR_Inf;
extern __FACTOR_Inf	FACTOR_Inf;

//����ԭ��
typedef enum{
ACCON = 0,			//ACC ON
OTRESET = 1,		//��ʱ��λ
STRESET = 2,		//���ø�λ
PREBOOT = 3,		//Ԥ����
STARTNOB = 4,		//��B+����
}__START_Reason;
extern __START_Reason	START_Reason;

extern u8 MCU_Awak;		//ϵͳ����״̬��0�����ߣ�1�����ѣ�
extern u8 ContTime;		//�Զ��ػ�����ʱ


extern const char *Version;		//�汾��
#define Version_SIZE strlen(Version)	//�汾�ų���











#endif







