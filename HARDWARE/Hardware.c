#include "Hardware.h"
#include "delay.h"
//#include "information.c"

extern void DID_VersionSet(void);
void _HARDWARE_INT(void)
{
	u8 i;
	
	DID_VersionSet();

//	OutPut_Init();		  	//初始化与LED连接的硬件接口
	uart1_init(72,115200);
	
#ifdef TW2836
#else
	uart2_init(36,115200);
#endif
	
	
	uart3_init(36,115200);
	IIC1_Init();			//I2C接口初始化
	IIC2_Init();			//I2C接口初始化
	Adc_Init();
//	TIM2_Int_Init(10000,7199);
	
//	while(AT24CXX_Check())
//	{
//		printf("TRY %d, EEPROM ERROR!\r\n",i++);	
//		delay_ms(800); 
//	}
	
//	while(CAN_Mode_Init(1,8,9,8,0))//CAN初始化,波特率250Kbps，CAN工作模式;0,普通模式;1,环回模式
	
#ifdef	SKY121

#else
	while(CAN_Mode_Init(1,4,13,8,0))
	{
//		printf("TRY %d, CAN ERROR!\r\n",i++);	
		delay_ms(100);
	}
#endif
	
//	TIM4_Int_Init(499,719);//5ms
//	IWDG_Init(4,625);
//	printf("it is ok!\r\n");	
	
}







