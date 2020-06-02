#ifndef __TESTCMD_H
#define __TESTCMD_H	  
#include "sys.h"  


typedef struct  
{
u8 len;
u8 dat[128];
}_cache_frame;

typedef struct  
{
u8 w_p;
u8 r_p;
_cache_frame dat[16];
}_SendCache;
extern _SendCache SendCache;










typedef struct {
uint8_t Light_value;
uint8_t Sys_source;
uint8_t Volume_value;
uint8_t Bass_value;
uint8_t Mid_value;
uint8_t Treb_value;
uint16_t FadeBalance_value;//高字节：前后；低字节：左右
}__Test_sysinf;
extern __Test_sysinf	Test_sysinf;


typedef struct {
uint16_t Frq_value;
uint8_t Band_value;
uint8_t Stereo_value;
uint16_t Track_num;//高字节：；低字节：；
uint8_t Play_ctrl;
}__Test_Voiceinf;
extern __Test_Voiceinf	Test_Voiceinf;

















void Test_CMDProcess(void);
void Write_sendcache(_cache_frame dat);
void Test_keysend(uint8_t key,uint8_t sta);







#endif





