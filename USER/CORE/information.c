#include "information.h"


//121-2件号	C00203820
//出口意大利件号EKY322-5		C00061025	12月25号收到消息，更改为 C00247452 
//京环件号	FKY31	C00250123


#ifdef	SKY121		//121
const char *Version = "NV1.20_20050920";//版本号
const char *Version2 = "NV1.20";//释放版本号
//const char *Version2 = "Amplifier_test_6v1";//测试版本号
#else
	#ifdef	ES50I
		#ifdef RDS_ENABLE	//意大利
		const char *Version = "NV1.20_20033152";//版本号
		const char *Version2 = "NV1.20";//释放版本号
		#else				//50i
		const char *Version = "NV1.19_19112819";//版本号
		const char *Version2 = "NV1.19";//释放版本号
		#endif	
	#else	//SKY32-1
		#ifdef FKY31		//京环
		const char *Version = "NV1.20_20050723";//版本号
		const char *Version2 = "NV1.20";//释放版本号
//		const char *Version2 = "FKY31";//测试版本号
		#else				
			#ifdef SKY322_3	//322_3
			const char *Version = "NV1.20_20052221";//版本号 //临时21
			const char *Version2 = "NV1.20";//释放版本号
			#else	//32-1
			const char *Version = "NV1.20_20051717";//版本号
			const char *Version2 = "NV1.20";//释放版本号
//		const char *Version2 = "TESTKEY_60ms";//测试版本号
			#endif
		#endif
	#endif
	
#endif



char radio_version[30];

__SYS_Config	SYS_Config;
__FAULT_Inf		FAULT_Inf;
__CAR_Set		CAR_Set={1,1,1,1,1,1};
__CAR_Inf		CAR_Inf={1600};
__SYS_Inf		SYS_Inf={1,0,0,0,0,0,0,0,0,0};
__KEY_Inf		KEY_Inf;
__LAROUND_Inf	LAROUND_Inf;
__START_Reason	START_Reason;
__VOICE_Inf		VOICE_Inf={0,0,87500,0,0,3};
__BLUE_Inf		BLUE_Inf={0,0,50,0};
__PHONE_Inf		PHONE_Inf={0,2};
__FACTOR_Inf	FACTOR_Inf;
__Signal_Control	Signal_Control;

#ifdef RDS_ENABLE
__RADIO_Inf		RADIO_Inf={0,0,0,522,8750,0,0,0x1f0,0x2f0,0,0};
#else
__RADIO_Inf		RADIO_Inf={0,0,0,531,8750,0,0,0x1f0,0x2f0,0,0};
#endif


u8 ContTime = OFFTIME;
u8 MCU_Awak = 0;
u32 Ignition_cnt = 0;


u16  Speed_flag;

u8 PowerNormal,PowerLow,PowerHigh;

u8 Video_sta = 0;




 











