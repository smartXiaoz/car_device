
#ifndef __SWITCH_H
#define __SWITCH_H	 
#include "sys.h"


typedef struct{
	u8 ctr;//控制指令
	u8 sta;//状态
}Switch_STA;

extern Switch_STA Switch;


#define Switch_S0_Pin PBout(14)	
#define Switch_S1_Pin PBout(15)

void Switch_Init(void);	//初始化		
u8 Switch_ctrl(void);	//开关切换
#endif




