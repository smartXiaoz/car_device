#include "information.h"

const char *Version = "NV1.09";//°æ±¾ºÅ

__SYS_Config	SYS_Config;
__FAULT_Inf		FAULT_Inf;
__CAR_Set		CAR_Set;
__RADIO_Inf		RADIO_Inf={0,0,0,531,875,0,0,0x1f0,0x2f0,0,0};
__CAR_Inf		CAR_Inf={1600};
__SYS_Inf		SYS_Inf;
__KEY_Inf		KEY_Inf;
__LAROUND_Inf	LAROUND_Inf;
__START_Reason	START_Reason;
__VOICE_Inf		VOICE_Inf={0,0,87500,16,3};
__BLUE_Inf		BLUE_Inf={0,0,50,0};
__PHONE_Inf		PHONE_Inf={0,2};
__FACTOR_Inf	FACTOR_Inf;
__Signal_Control	Signal_Control;

u8 ContTime = 30;
u8 MCU_Awak = 0;
u32 Ignition_cnt = 0;

u8 PowerNormal,PowerLow,PowerHigh;






 











