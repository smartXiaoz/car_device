#ifndef __UARTEXP_H
#define __UARTEXP_H	
#include "sys.h"

//������Ϣ
typedef struct{
	u8  STA;		//�������ݽ���״̬ 0����������ģʽ��1���ȴ���������ģʽ��2�����ݴ�������У�3����������ɹ���4����������ʧ��
	u32 EsDatLen;	//Ԥ���ֽ���
	u32 AcDatLen;	//ʵ���ֽ���
	u16 MSG;		//�ܰ���
	u16 L_FN;		//��һ�����
	u16 FN;			//�����
	u32 Dat_write;	//����ָ��
	u32 Save_AD;	//��ַ
	u32 Flag_AD;	//
	u16 Data[1024];	//���ݻ�������
	u16 ssst;
}UPGRADE;
extern UPGRADE UP_Data;


u8 TX_Process(void);				//����������10ms����һ��
u8 UART_explain(u16 Msg,u8 *Dat);	//Э�������������������


u8 Check_TXSTA(u8 CH);
void clean_TXtask(void);
void CMD_8003(void);//�ϱ��ػ�����ʱ����ʱ��ʼ�������ϱ���
u8 CMD_8101(u8 mode);			//������Ϣ�ϱ�
u8 CMD_8102(u8 NUM,u8 STA);	//������Ϣ�ϱ�����������
u8 CMD_8103(u8 NUM,u8 STA);	//������Ϣ�ϱ������ţ����⣬ת�򣬴�������
u8 CMD_8105(u8 STA);		//MUTE״̬�ϱ�
u8 CMD_8106(u8 STA);		//��Ļʧ�ܿ���
u8 CMD_8301(u8 Sta);		//��������Ϣ�ϱ�������������
void CMD_8501(void);		//��ʼ��������
void CMD_8503(u8 result);	//���������������ʱ�˳���ʹ��
u8 CMD_8100(void);			//����


u8 CMD_8200(u8 *Dat);//����
u8 CMD_8621(u8 mode,u32 ss);//̨ͣ������
u8 CMD_8622(u8 mode,u32 stnr);//�����
u8 CMD_8623(u8 mode,u32 fres);//Ƶ����Ӧ
u8 CMD_8640(u8 mode,u8 result);//�궨���
u8 CMD_8680(void);
u8 CMD_810A(u8 sta);//Ӱ��״̬

#endif







