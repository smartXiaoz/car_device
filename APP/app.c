#include "app.h"
#include "information.h"


extern void udsTASK_Create(u8 TASK_PRIO,u16 SIZE);
void APP_INT(void)
{
//	ledTASK_Create(9,256);
	
#ifdef	SKY121

#else
	CanTASK_Create(7,192);
	udsTASK_Create(8,128);
#endif
	wakeupTASK_Create(10,192);
	ProcessTASK_Create(6,192);
	InPutScanTASK_Create(6,192);
	uartTASK_Create(5,512);
	
}






