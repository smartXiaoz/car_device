#ifndef __INFORMATION_H
#define __INFORMATION_H	
#include "sys.h"
#include "string.h"

#define		LOOK_AROUND

extern u8 PowerNormal,PowerLow,PowerHigh;

extern u32 Ignition_cnt;//ACC上电次数计数,从初始0开始，单片机断电后清零

//信号控制	0：正常收发，1：禁止收发
typedef struct {
u8 send;
u8 receiver;
}__Signal_Control;
extern __Signal_Control		Signal_Control;


//	//FICM
//u8 ID18FEE6EE;	//发送，周期，时间同步，
//u8 ID18FFB676;	//发送，周期，音响源
//u8 ID18FF6276;	//发送，周期，蓝牙信息
//u8 ID18FF6376;	//发送，事件，通话号码
//u8 ID18FF6476;	//发送，事件，通话名称
//u8 ID18FFC676;	//发送，事件，设置开关
//	//BSM
//u8 ID18FEBF0B;	//接收，周期，前轴转速
//	//BCM
//u8 ID10FF1021;	//接收，周期，转向灯
//u8 ID18FF6FEE;	//接收，周期，寻车反馈、行车落锁、熄火解锁开关状态
//u8 ID08FF00DD;	//接收，周期，遥控器状态
//u8 ID10FF7521;	//接收，周期，车门、钥匙状态
//	//IC
//u8 ID10FF4117;	//接收，周期，倒车灯、超速报警开关状态
//	//DDSW
//u8 ID18A005E7;	//接收，周期，疲劳驾驶开关状态
//	//BSD
//u8 ID1CA00701;	//接收，周期，BSD开关状态
//	//TCU
//u8 ID18f00503;	//接收，周期，档位信号



//配置信息	0：没有，1：有
typedef struct {
u8 LOA;			//环视、后视 0为后视，
u8 LOC;			//寻车反馈
u8 DDWS;		//疲劳预警
u8 BSD;			//盲区监测 
}__SYS_Config;
extern __SYS_Config		SYS_Config;


//故障信息 0：正常，1：故障
typedef struct{
u8 BLTH;		//蓝牙	0/1 
u8 WIFI;		//WIFI	
u8 VOICE;		//语音	
u8 LOA;			//环视	
u8 RADIO;		//收音机	
u8 MPUDIED;		//死机	
u8 BUSOFF;		//总线关闭，0：总线正常，1：总线关闭
u8 MISSABS;		//miss ABS
u8 MISSIC;		//miss IC
u8 MISSBCM;		//miss BCM
//u8 OVERVOL;		//高压
//u8 UNDERVOL;	//低压

}__FAULT_Inf;
extern __FAULT_Inf		FAULT_Inf;


//车身设置开关 0：关，1：开
typedef struct{
u8 OVERSPEE;	//超速报警	 
u8 CARSEARCH;	//寻车反馈	
u8 DRIVLOCK;	//行车落锁	
u8 PARKULOCK;	//停车解锁
u8 DDWS;		//疲劳驾驶	
u8 BSD;			//盲点监测	
}__CAR_Set;
extern __CAR_Set		CAR_Set;


//收音机状态信息
typedef struct{
u8 WORK_STA;	//工作状态	关、开、故障、启动中	/0/1/2/3	
u8 MODE;		//工作模式	AM/FM 1/0	
u16 Last_FRQ;	//搜台用
u16 AM_FRQ;		//AM频点	531  -- 1602
u16 FM_FRQ;		//FM频点	875 -- 1080
u8 SIGQUA;		//信号质量	0：无信号，1：有信号
u8 CMD;			//控制命令	0：空闲，1：模式切换，2：跳转至指定频率，3：查询当前频率，4：当前位置向上搜台，5：当前位置向下搜台，6：停止搜台，7：全局搜台,8:工厂模式跳转
u16 FM_LEVEL;	//fm停台灵敏度
u16 AM_LEVEL;	//am停台灵敏度
u16 FM_STNR;	//fm信噪比
u16 FM_FRES;	//fm频率响应
}__RADIO_Inf;
extern __RADIO_Inf		RADIO_Inf;


//车身状态信息
typedef struct{
//u8 NUM;			//信息编号
//u8 STA;			//状态
u16 Batter_Value;//电池电压	
u8 NIGHTLIGHT;	//夜灯开，调暗背光	0：白天，1：夜间
u8 ReverseSta;	//倒车状态			0：正常行车，1：倒车
u8 DIVEDOOR;	//驾驶室门			0：关闭，1：打开
u8 PSNGDOOR;	//副驾驶门			0：关闭，1：打开
u8 ACC;			//ACC状态			0：下电，1：上电
u8 POWER;		//整车电源状态		0：OFF，1：ACC，2：ON
u8 KEY_STS;		//钥匙位置			1：Stop and Key absent，2：Reserved，4：ON，6：Stop and Key In，C：Reserved，E：Error，F：Not Avaliable，其他：保留
u8 LTurnSts;	//左转向灯			0：关，1：开
u8 RTurnSts;	//右转向灯			0：关，1：开
u16 Car_Speed;	//前轴平均速度
u8	Lock;		//车锁				0：保留，1：门解锁，2：门上锁
}__CAR_Inf;
extern __CAR_Inf		CAR_Inf;


//系统状态信息
typedef struct{
u8 STA;			//系统状态	0：休眠；1：唤醒过程，2：正常工作中，3：升级数据接收过程中；
u8 MPU_STA;		//MPU状态   0：等待连线；1：正常在线，2：MPU请求关机；3：MPU升级离线；4：关机状态；5：故障
u8 BATTER_VERR;	//电压保护标志
u8 MUTE_STA;
}__SYS_Inf;
extern __SYS_Inf		SYS_Inf;


//按键状态信息
typedef struct{
u8 NUM;			//按键编号	8个按键
u8 STA;			//按键状态	0：释放，1：短按，2：长按	
}__KEY_Inf;
extern __KEY_Inf		KEY_Inf;


//环视视角状态
//typedef enum{
//FRONT=1,		//前
//AFFTER,			//后
//LEFT,			//左
//RIGHT,			//右
//}__LAROUND_Sta;
//extern __LAROUND_Sta	LAROUND_Sta;

//环视视角状态
typedef struct{
u8 STA;			//环视状态	0：等待连线，1：正常工作；2：故障
u8 VIS;			//视角		0：停止图像输出；1：前；2：后；3：左；4：右，
}__LAROUND_Inf;
extern __LAROUND_Inf	LAROUND_Inf;

//音响源信息
typedef struct{
u8 source;		//源
u8 playsta;		//播放状态
u32 radiofrq;	//收音频率
u8 volume;		//音量
u8 mutesta;		//禁音
u8 phonesta;	//通话状态
}__VOICE_Inf;
extern __VOICE_Inf	VOICE_Inf;

//蓝牙设备信息
typedef struct{
u8 sta;			//连接状态
u8 name_len;	//设备名长度
u8 time;		//发送计时用	
u8 name[32];	//设备名
}__BLUE_Inf;
extern __BLUE_Inf	BLUE_Inf;

//通话信息
typedef struct{
u8 sta;			//状态
u8 time;	
u8 name_len;	//设备名长度
u8 num_len;	//设备名长度
u8 name[33];	//设备名
u8 number[33];	//设备名
}__PHONE_Inf;
extern __PHONE_Inf	PHONE_Inf;

//工厂模式信息
typedef struct{
u8 sta;			//状态

}__FACTOR_Inf;
extern __FACTOR_Inf	FACTOR_Inf;

//启动原因
typedef enum{
ACCON = 0,			//ACC ON
OTRESET = 1,		//超时复位
STRESET = 2,		//设置复位
PREBOOT = 3,		//预启动
STARTNOB = 4,		//断B+启动
}__START_Reason;
extern __START_Reason	START_Reason;

extern u8 MCU_Awak;		//系统唤醒状态，0：休眠，1：唤醒，
extern u8 ContTime;		//自动关机倒计时


extern const char *Version;		//版本号
#define Version_SIZE strlen(Version)	//版本号长度











#endif







