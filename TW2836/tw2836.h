#ifndef __TW2836_H
#define __TW2836_H	 
#include "sys.h"


#define MASTER	0x84	//address

#define	DVC_PG0		0
#define	DVC_PG1		1
#define	DVC_PG2		2


//IO方向设置
#define TW2836_SDA_IN()  {GPIOA->CRL&=0XFFFFF0FF;GPIOA->CRL|=0X00000800;}
#define TW2836_SDA_OUT() {GPIOA->CRL&=0XFFFFF0FF;GPIOA->CRL|=0X00000300;}

//IO操作函数	 
#define TW2836_IIC_SCL    PAout(3) 		//SCL
#define TW2836_IIC_SDA    PAout(2) 		//SDA	 
#define TW2836_READ_SDA   PAin(2) 	 		//输入SDA 



void TW2836_init(void);
void TW2836_set(u8 mode,u8 value);
u8 TW2836_Ctrl(u8 mode);


void TW2836_Process(void);
void TW2836_Switch(u8 value);
void TW2836_Adjust(u8 mode,u8 value);
	


#endif


