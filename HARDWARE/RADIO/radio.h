#ifndef __RADIO_H
#define __RADIO_H	 
#include "sys.h" 

extern const u16  Frequency[4];

u8 TEF6686_Rest(void);
u8 TEF6686_Start(void);	 
u8 TEF6686_Mute(u8 mute);
u8 TEF6686_Low(u8 low);
u16 TEF6886_GetSta(u8 mode);
u8 TEF6686_TuneTo(void);

u8 Radio_Process(void);

u8 TEF6686_rdsGet(u8 *data);

void get_version(void);

#endif










