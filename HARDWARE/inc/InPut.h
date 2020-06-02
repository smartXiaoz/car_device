#ifndef __INPUT_H
#define __INPUT_H	 
#include "sys.h"


#define ACC_IN		PAin(5) //ACC检测
#define Blight_IN	PAin(1)	//背光调节
#define BACK_IN		PCin(2)	//倒车


void InPut_Init(void);	//IO初始化
u8 InPut_Scan(void);  	//扫描函数					    
#endif
