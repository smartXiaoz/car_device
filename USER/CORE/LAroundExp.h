#ifndef __LAROUNDEXP_H
#define __LAROUNDEXP_H	
#include "sys.h"


u8 LA_TX_Process(void);					//发送任务处理，10ms调用一次
u8 LA_UART_explain(u16 Msg,u8 *Dat);	//协议解析函数，触发调用

u8 LA_CMD_82(u8 LAwark_Mod);			//视角切换命令
void LA_CMD_81(void);














#endif







