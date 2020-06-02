
#include "hardware.h"
#include "LAroundExp.h"

//调试配置

#define LA_CheckSun	0		//打开校验和检查

///
#define LA_UART_SYNC	0xA55A			//同步头
#define LA_TX_oTime		100				//等待重发时间
#define LA_TX_Cnt		1				//发送次数
#define LA_DataSend		Uart2_TxBuff	//定义发送串口与函数

#define LA_Frame_Cap	3		//消息组
//帧编号
#define LA_CMD81	0	//心跳	
#define LA_CMD82	1	//设置
#define LA_CMD83	2	//设置


static u8 LA_ACK_Buff[7]={6,0xA5,0x5A,0,0,0,0};	//应答帧,第一个字节存该帧长度
static u8  LA_Frame_Buff[LA_Frame_Cap][10]={	//帧数据缓存,第一个字节存该帧长度，每一帧位置必须与帧编号对应
										{6, 0xA5,0x5A, 0x01, 1,0x81, 0,0},	//2+8	心跳	
										{7, 0xA5,0x5A, 0x01, 2,0x82, 0,0},	//2+8	设置	
										{7, 0xA5,0x5A, 0x01, 2,0x83, 0,0},	//2+5	截图、标定
										};

static u8 LA_ACK_Task = 0;		//ACK任务
								
typedef struct{
	u16 SYNC;		//同步头
	u8  ACK;		//应答
	u8  LEN;		//帧长度
	u8  CMD;		//命令
	u8  CHECKSUM;	//校验和
	u8  PARAM[2];	//命令数据缓存数组
}LA_UART_STA;
static LA_UART_STA	LA_UartRX;
								
//装载校验和
static void LA_CheckSun_load(u8 CH)
{
	u8 i = 0;
	u16 Temp = 0;
	//计算校验和
	for(i=3;i<(LA_Frame_Buff[CH][0]);i++) Temp += LA_Frame_Buff[CH][i];
	LA_Frame_Buff[CH][LA_Frame_Buff[CH][0]] = (u8)~(Temp);
}
//ACK装载
static void LA_ACK_Load(u8 ACK_STA,u8 Cmd)
{
	u16 temp = 0;
	u8 i = 0;
	LA_ACK_Buff[3] = ACK_STA|0x80;			//应答状态，正应答或负应答
	LA_ACK_Buff[4] = 0x01;				//LEN
	LA_ACK_Buff[5] = Cmd;				//CMD
	//计算校验和
	for(i=3;i<6;i++) temp += LA_ACK_Buff[i];
	LA_ACK_Buff[6] = (u8)(~temp);		//校验和
	//通知发送
	LA_ACK_Task = 1;
}
//
//发送处理函数
u8 LA_TX_Stabuff[LA_Frame_Cap][6]={0};//0：任务状态，1：超时计时器，2：超时时间，3：重发计数器，4：发送次数；5：重发达上限
/**
*--功能：添加一次发送任务
*--输入：CH ：帧编号
*--		Cnt：发送次数
*--		Time：超时时间
*--		Mode：发送模式，0，立即发送，1；延迟发送
*--返回：0：成功，1：失败
**/
u8 LA_TX_TaskADD(u8 CH,u8 Cnt,u8 Time,u8 Mode)
{
//	if(LAROUND_Inf.STA != 1) return 1;//休眠和预唤醒模式，拒绝加入任务（注释则缓存）
	LA_CheckSun_load(CH);
//	if(TX_Stabuff[CH][0] == 1) return 3;//任务进行中，添加失败（注释则覆盖原任务）
	if(Mode)LA_TX_Stabuff[CH][1] = 0;		//超时计数器	延迟发送
	else	LA_TX_Stabuff[CH][1] = Time;	//超时计数器	立即发送
	LA_TX_Stabuff[CH][2] = Time;			//超时时间
	LA_TX_Stabuff[CH][4] = Cnt;			//发送次数
	LA_TX_Stabuff[CH][3] = 0;
	LA_TX_Stabuff[CH][0] = 1;				//标记任务
	return 0;
}
//
/**
*--功能：注销一个进行中的任务
*--输入：CH ：帧编号
*--返回：0：成功，1：失败
**/
u8 LA_TX_TaskCancel(u8 CH)
{
	if(LA_TX_Stabuff[CH][0] == 0) return 1;//任务空，
	LA_TX_Stabuff[CH][0] = 0;				//标记任务	
	LA_TX_Stabuff[CH][1] = 0;				//超时计数清空
	LA_TX_Stabuff[CH][2] = 0;				//超时时间清空
	LA_TX_Stabuff[CH][3] = 0;				//重发计数器清空
//	TX_Stabuff[CH][5] = 0;
	return 0;
}
//
/**
*--功能：发送任务处理函数
*--输入：无
*--返回：0：无意义
*--					
**/
u8 LA_TX_Process(void)
{
	u8 i = 0;
	
	if(LAROUND_Inf.STA == 0) return 0;//环视离线，不进行发送任务
	
	if(LA_ACK_Task == 1)//应答发送
		{LA_DataSend(LA_ACK_Buff);LA_ACK_Task = 0;return 0;}
	
	for(i=0;i<LA_Frame_Cap;i++)
	{
		if(LA_TX_Stabuff[i][0] == 1)	//找出需要发送的任务
		{
			if(LA_TX_Stabuff[i][1]<LA_TX_Stabuff[i][2])LA_TX_Stabuff[i][1]++;	//超时计时
			else	//已超时
			{
				LA_TX_Stabuff[i][1] = 0;//重新计时
				if(LA_TX_Stabuff[i][3]<LA_TX_Stabuff[i][4])	//未达上线
				{
					//发送
					LA_DataSend(LA_Frame_Buff[i]);
					LA_TX_Stabuff[i][3]++;
					if(LA_TX_Stabuff[i][3]>=LA_TX_Stabuff[i][4])//已达上限
					{
						LA_TX_Stabuff[i][0] = 0;	//清除任务标记
						LA_TX_Stabuff[i][1] = 0;	//超时计数清空
						LA_TX_Stabuff[i][3] = 0;	//重发计数器清空
						LA_TX_Stabuff[i][4] = 0;
						LA_TX_Stabuff[i][5] = 1;	//重发达到上限
					}
					return 0;//直接退出此次循环，防止发送数据拼接，但会造成优先级存在
				}
			}
		}
	}
	return 0;
}
//


//接收数据处理

/**
*--功能：接收数据处理函数（协议解析）
*--输入：Msg：收到的数据长度
*--		 Dat：收到的数据内容
*--返回：错误类型
*--
**/
u8 LA_ACK_Process(u16 CMD,u8 ACK);
static u8 LA_CMD_00(u8 *Dat);
void cl_ladtc(void);
static u8 LA_CMD_03(u8 *Dat);
u8 LA_UART_explain(u16 Msg,u8 *Dat)
{
	//定义局部变量
	u16 temp = 0;	//检验和缓存变量
	u16 i = 0;		//
	u8 re = 0;		//错误类型标记
	
	//数据检出、计算
	LA_UartRX.SYNC = (Dat[0]<<8)+Dat[1];	//检出SYNC
	LA_UartRX.ACK = Dat[2];					//检出ACK	
	LA_UartRX.LEN = Dat[3];					//检出LEN
	LA_UartRX.CMD = Dat[4];					//检出CMD_H
	//计算检验和，限定32字节，防止数据错误进入死循环（有效数据不会超过200）
	for(i=0;(i<(LA_UartRX.LEN+2)&&(i<32));i++) temp += Dat[i+2];
	LA_UartRX.CHECKSUM = (u8)~(temp);	
	
	//数据校验
	if(LA_UartRX.SYNC != LA_UART_SYNC)	re = 1;	//帧同步头校验
	else if(LA_UartRX.LEN != (Msg-5))	re = 2;	//长度校验
#if	LA_CheckSun
	if(LA_UartRX.CHECKSUM != Dat[msg-1])	re = 3;	//校验和校验
#endif
	if(re != 0) return re;//校验不通过，错误帧
		
	cl_ladtc();
		
	if((((LA_UartRX.ACK&0x7F) == 0x02)||((LA_UartRX.ACK&0x7F) == 0x03))&&(LA_UartRX.CMD&0x80))//收到应答信号
	{
		re = LA_ACK_Process(LA_UartRX.CMD,(u8)(LA_UartRX.ACK&0x7F));//应答信号处理
		return re;
	}
	
	
	//缓存数据
	for(i=0;i<(LA_UartRX.LEN-1);i++) LA_UartRX.PARAM[i] = Dat[i+5];//缓存数据
	//命令处理
	switch(LA_UartRX.CMD)
	{
		case 0x00:re =  LA_CMD_00(LA_UartRX.PARAM);;break;//握手
		case 0x03:re =  LA_CMD_03(LA_UartRX.PARAM);break;//标定结果
		
		
		
		
		default:re = 12;break;
	}
	//应答发送（应答有最高优先级）
	if((LA_UartRX.ACK&0x7F) == 0x01)
	{
		if(re == 0)	LA_ACK_Load(0x02,LA_UartRX.CMD);	//正应答0x02
		else		LA_ACK_Load(0x03,LA_UartRX.CMD);	//负应答0x03
	}
	return re;
}
//

/**
*--功能：应答处理函数
*--输入：CMD：收到的数据CMD
*--		 ACK：收到的ACK
*--返回：错误类型
**/
u8 LA_ACK_Process(u16 CMD,u8 ACK)
{
	u8 i = 0;
	switch(CMD)//检出CMD对应的帧编号，用于注销重发索引
	{
		case 0x81:i=0;break;
		case 0x82:i=1;break;

		default:i=255;break;
	}
	if(i==255)return 4;//CMD错误，无法解析
	if(ACK == 0X02)	//正应答
	{
		LA_TX_TaskCancel(i);	//注销重发
	}
	else if(ACK == 0X03)//负应答
	{
		LA_TX_TaskCancel(i);	//注销重发
	}
	else //错误应答
	{
		return 4;
	}
	return 0;
}
//


//握手
//CMD_00
static u8 LA_CMD_00(u8 *Dat)
{
	//标记环视上线
	LAROUND_Inf.STA = 1;
	
	LA_Frame_Buff[LA_CMD82][6] = LAROUND_Inf.VIS;	//模式
	LA_TX_TaskADD(LA_CMD82,LA_TX_Cnt,LA_TX_oTime,1);//延迟发送当前模式
	
	return 0;
}


//标定结果
extern u8 CMD_8640(u8 mode,u8 result);//标定结果
static u8 LA_CMD_03(u8 *Dat)
{
	CMD_8640(Dat[0],Dat[1]);
}


u8 LAHAERT_Cnt = 0;
void cl_ladtc(void)//MPU故障清除
{
	LAHAERT_Cnt = 0;
	FAULT_Inf.LOA = 0;
//	if(LAROUND_Inf.STA == 2)
		LAROUND_Inf.STA=1;
}
void LA_CMD_81(void)
{
	if(CAR_Inf.ACCDe == 0)//环视没上电
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
		FAULT_Inf.LOA = 1;//标记DTC
	}
#endif
#endif	
}
//视角切换命令
//CMD_82
u8 LA_CMD_82(u8 LAwark_Mod)
{
	LA_Frame_Buff[LA_CMD82][6] = LAwark_Mod;	//模式

	//通知发送
	LA_TX_TaskADD(LA_CMD82,LA_TX_Cnt,LA_TX_oTime,0);
	return 0;
}

//截图、标定命令
//CMD_83
u8 LA_CMD_83(u8 cmds)
{
	LA_Frame_Buff[LA_CMD83][6] = cmds;	//模式

	//通知发送
	LA_TX_TaskADD(LA_CMD83,LA_TX_Cnt,LA_TX_oTime,0);
	return 0;
}

