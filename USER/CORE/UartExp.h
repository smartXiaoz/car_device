#ifndef __UARTEXP_H
#define __UARTEXP_H	
#include "sys.h"

//升级信息
typedef struct{
	u8  STA;		//升级数据接收状态 0：不是升级模式；1：等待数据请求模式；2：数据传输过程中；3：升级传输成功；4：升级传输失败
	u32 EsDatLen;	//预计字节数
	u32 AcDatLen;	//实际字节数
	u16 MSG;		//总包数
	u16 L_FN;		//上一包序号
	u16 FN;			//包序号
	u32 Dat_write;	//包内指针
	u32 Save_AD;	//地址
	u32 Flag_AD;	//
	u16 Data[1024];	//数据缓存数组
	u16 ssst;
}UPGRADE;
extern UPGRADE UP_Data;


u8 TX_Process(void);				//发送任务处理，10ms调用一次
u8 UART_explain(u16 Msg,u8 *Dat);	//协议解析函数，触发调用


u8 Check_TXSTA(u8 CH);
void clean_TXtask(void);
void CMD_8003(void);//上报关机倒计时（计时开始后主动上报）
u8 CMD_8101(u8 mode);			//车速信息上报
u8 CMD_8102(u8 NUM,u8 STA);	//按键信息上报，按键触发
u8 CMD_8103(u8 NUM,u8 STA);	//车身信息上报，车门，背光，转向，触发调用
u8 CMD_8105(u8 STA);		//MUTE状态上报
u8 CMD_8106(u8 STA);		//屏幕失能控制
u8 CMD_8301(u8 Sta);		//收音机信息上报，收音机触发
void CMD_8501(void);		//开始升级命令
void CMD_8503(u8 result);	//发送升级结果，超时退出会使用
u8 CMD_8100(void);			//心跳


u8 CMD_8200(u8 *Dat);//配置
u8 CMD_8621(u8 mode,u32 ss);//停台灵敏度
u8 CMD_8622(u8 mode,u32 stnr);//信噪比
u8 CMD_8623(u8 mode,u32 fres);//频率响应
u8 CMD_8640(u8 mode,u8 result);//标定结果
u8 CMD_8680(void);
u8 CMD_810A(u8 sta);//影像状态

#endif







