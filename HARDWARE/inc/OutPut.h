#ifndef __OUTPUT_H
#define __OUTPUT_H	 
#include "sys.h"

#define	ON	1
#define	OFF	0


//#define CAN_ENpin	PCout(9)		//CANʹ������
#define CAN_ENpin	PCout(4)		//CANʹ������
#define BLACK_ENpin PBout(12)		//��Ļ����ʹ������
#define POWER_ENpin PCout(3)		//��Դʹ������
//#define MUTE_ENpin	PBout(3)		//���ž�������
#define MUTE_ENpin	PBout(8)		//���ž�������
#define PAMP_ENpin	PBout(4)		//����ʹ������
//#define Astern_OPutPin PCout(8)		//�������
#define Astern_OPutPin PCout(10)		//�������
#define MPU_ResetPin PCout(11)		//MPU��λ

void OutPut_Init(void);	//��ʼ��		
void OUT_DISABLE(void);
void OUT_ENABLE(void);

//�ߵ�ƽCAN�͹��ģ��͵�ƽ��������
#define CAN_Ctrl(Sta)		if(Sta)	CAN_ENpin=0;\
							else	CAN_ENpin=1;

//��Ļ������ƣ���ʹ��
#define Black_Ctrl(Sta)		if(Sta)	BLACK_ENpin=1;\
							else	BLACK_ENpin=0;

//��Դ���ƣ��ߵ�ƽ��Դ�����͵�ƽ��Դ��
#define Power_Ctrl(Sta)		if(Sta)	POWER_ENpin=1;\
							else	POWER_ENpin=0;

//���ž������ƣ��ߵ�ƽ�������͵�ƽMUTE
#define Mute_Ctrl(Sta)		if(Sta)	MUTE_ENpin=0;\
							else	MUTE_ENpin=1;

//�������߿��ƣ��ߵ�ƽ�������͵�ƽ����
#define PAMP_Ctrl(Sta)		if(Sta)	PAMP_ENpin=1;\
							else	PAMP_ENpin=0;

////��������źţ��ߵ�ƽ�������͵�ƽ����
//#define Astern_Ctrl(Sta)	if(Sta)	Astern_OPutPin=0;\
//							else	Astern_OPutPin=1;


//AC8255��λ�źţ��ߵ�ƽ��λ���͵�ƽ����
#define MPUReset_Ctrl(Sta)	if(Sta)	MPU_ResetPin=1;\
							else	MPU_ResetPin=0;



void Astern_Ctrl(u8 Sta);
















#endif

















