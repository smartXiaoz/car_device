#ifndef __INFORMATION_H
#define __INFORMATION_H	
#include "sys.h"
#include "string.h"

/*
³ö¿ÚÒâ´óÀûÔÚES50I»ù´¡ÐÞ¸Ä£¬ÐèÒª±£ÁôCANID×ª·¢£¬
121-2ÔÚ32-1»ù´¡ÐÞ¸Ä£¬

121-2¼þºÅ	C00203820

 12VºóÊÓ	24VºóÊÓ		12V»·ÊÓ		24V»·ÊÓ
	15		16			17			18
	
*/
//SKY32-1 	:	SKY321	LOOK_AROUND	HIGHPOWER
//SKY121	:	SKY121
//ES50I		:	ES50I
//FKY322-5	:	ES50I	RDS_ENABLE
//FKY31		:	LOOK_AROUND FKY31	TW2836
//SKY322_3		:	SKY322_3	×óÓÒÊÓ

/*****************************************************************/

#define		SKY321
#define		LOOK_AROUND			//»·ÊÓ
//#define		HIGHPOWER

//#define		SKY121

//#define		ES50I	//ES50iÏîÄ¿
//#define		RDS_ENABLE

//#define	FKY31	//¾©»·
//#define	TW2836	//Ê¹ÓÃTW2836Ð¾Æ¬

//#define	SKY322_3


#define EMMCDEBUG	//ÊµÑé


/******************************************************************/


#define		DOORLOCK		//³µËø

#define		OFFTIME		10


extern u8 PowerNormal,PowerLow,PowerHigh;
extern  u16  Speed_flag;

extern u32 Ignition_cnt;//ACCÉÏµç´ÎÊý¼ÆÊý,´Ó³õÊ¼0¿ªÊ¼£¬µ¥Æ¬»ú¶ÏµçºóÇåÁã

//ÐÅºÅ¿ØÖÆ	0£ºÕý³£ÊÕ·¢£¬1£º½ûÖ¹ÊÕ·¢
typedef struct {
u8 send;
u8 receiver;
u8 power_dis;
}__Signal_Control;
extern __Signal_Control		Signal_Control;


//	//FICM
//u8 ID18FEE6EE;	//·¢ËÍ£¬ÖÜÆÚ£¬Ê±¼äÍ¬²½£¬
//u8 ID18FFB676;	//·¢ËÍ£¬ÖÜÆÚ£¬ÒôÏìÔ´
//u8 ID18FF6276;	//·¢ËÍ£¬ÖÜÆÚ£¬À¶ÑÀÐÅÏ¢
//u8 ID18FF6376;	//·¢ËÍ£¬ÊÂ¼þ£¬Í¨»°ºÅÂë
//u8 ID18FF6476;	//·¢ËÍ£¬ÊÂ¼þ£¬Í¨»°Ãû³Æ
//u8 ID18FFC676;	//·¢ËÍ£¬ÊÂ¼þ£¬ÉèÖÃ¿ª¹Ø
//	//BSM
//u8 ID18FEBF0B;	//½ÓÊÕ£¬ÖÜÆÚ£¬Ç°Öá×ªËÙ
//	//BCM
//u8 ID10FF1021;	//½ÓÊÕ£¬ÖÜÆÚ£¬×ªÏòµÆ
//u8 ID18FF6FEE;	//½ÓÊÕ£¬ÖÜÆÚ£¬Ñ°³µ·´À¡¡¢ÐÐ³µÂäËø¡¢Ï¨»ð½âËø¿ª¹Ø×´Ì¬
//u8 ID08FF00DD;	//½ÓÊÕ£¬ÖÜÆÚ£¬Ò£¿ØÆ÷×´Ì¬
//u8 ID10FF7521;	//½ÓÊÕ£¬ÖÜÆÚ£¬³µÃÅ¡¢Ô¿³××´Ì¬
//	//IC
//u8 ID10FF4117;	//½ÓÊÕ£¬ÖÜÆÚ£¬µ¹³µµÆ¡¢³¬ËÙ±¨¾¯¿ª¹Ø×´Ì¬
//	//DDSW
//u8 ID18A005E7;	//½ÓÊÕ£¬ÖÜÆÚ£¬Æ£ÀÍ¼ÝÊ»¿ª¹Ø×´Ì¬
//	//BSD
//u8 ID1CA00735;	//½ÓÊÕ£¬ÖÜÆÚ£¬BSD¿ª¹Ø×´Ì¬
//	//TCU
//u8 ID18f00503;	//½ÓÊÕ£¬ÖÜÆÚ£¬µµÎ»ÐÅºÅ



//ÅäÖÃÐÅÏ¢	0£ºÃ»ÓÐ£¬1£ºÓÐ
typedef struct {
u8 LOA;			//»·ÊÓ¡¢ºóÊÓ 0ÎªºóÊÓ£¬
u8 LOC;			//Ñ°³µ·´À¡
u8 DDWS;		//Æ£ÀÍÔ¤¾¯
u8 BSD;			//Ã¤Çø¼à²â 
}__SYS_Config;
extern __SYS_Config		SYS_Config;


//¹ÊÕÏÐÅÏ¢ 0£ºÕý³££¬1£º¹ÊÕÏ
typedef struct{
u8 BLTH;		//À¶ÑÀ	0/1 
u8 WIFI;		//WIFI	
u8 VOICE;		//ÓïÒô	
u8 LOA;			//»·ÊÓ	
u8 RADIO;		//ÊÕÒô»ú	
u8 MPUDIED;		//ËÀ»ú	
volatile u8 BUSOFF;		//×ÜÏß¹Ø±Õ£¬0£º×ÜÏßÕý³££¬1£º×ÜÏß¹Ø±Õ
u8 BUSOFF_Flag;
u8 CAN_OFF;
u8 MISSABS;		//miss ABS
u8 MISSIC;		//miss IC
u8 MISSBCM;		//miss BCM
	
//u8 OVERVOL;	//¸ßÑ¹
//u8 UNDERVOL;	//µÍÑ¹

}__FAULT_Inf;
extern __FAULT_Inf		FAULT_Inf;


//³µÉíÉèÖÃ¿ª¹Ø 0£º¹Ø£¬1£º¿ª
typedef struct{
u8 OVERSPEE;	//³¬ËÙ±¨¾¯	 
u8 CARSEARCH;	//Ñ°³µ·´À¡	
u8 DRIVLOCK;	//ÐÐ³µÂäËø	
u8 PARKULOCK;	//Í£³µ½âËø
u8 DDWS;		//Æ£ÀÍ¼ÝÊ»	
u8 BSD;			//Ã¤µã¼à²â	
}__CAR_Set;
extern __CAR_Set		CAR_Set;


//ÊÕÒô»ú×´Ì¬ÐÅÏ¢
typedef struct{
u8 WORK_STA;	//¹¤×÷×´Ì¬	¹Ø¡¢¿ª¡¢¹ÊÕÏ¡¢Æô¶¯ÖÐ	/0/1/2/3	
u8 MODE;		//¹¤×÷Ä£Ê½	AM/FM 1/0	
u16 Last_FRQ;	//ËÑÌ¨ÓÃ
u16 AM_FRQ;		//AMÆµµã	531  -- 1602
u16 FM_FRQ;		//FMÆµµã	8750 -- 10800
u8 SIGQUA;		//ÐÅºÅÖÊÁ¿	0£ºÎÞÐÅºÅ£¬1£ºÓÐÐÅºÅ
u8 CMD;			//¿ØÖÆÃüÁî	0£º¿ÕÏÐ£¬1£ºÄ£Ê½ÇÐ»»£¬2£ºÌø×ªÖÁÖ¸¶¨ÆµÂÊ£¬3£º²éÑ¯µ±Ç°ÆµÂÊ£¬4£ºµ±Ç°Î»ÖÃÏòÉÏËÑÌ¨£¬5£ºµ±Ç°Î»ÖÃÏòÏÂËÑÌ¨£¬6£ºÍ£Ö¹ËÑÌ¨£¬7£ºÈ«¾ÖËÑÌ¨,8:¹¤³§Ä£Ê½Ìø×ª
u16 FM_LEVEL;	//fmÍ£Ì¨ÁéÃô¶È
u16 AM_LEVEL;	//amÍ£Ì¨ÁéÃô¶È
u16 FM_STNR;	//fmÐÅÔë±È
u16 FM_FRES;	//fmÆµÂÊÏìÓ¦
u8 FA_Stereo;	//FMÁ¢ÌåÉù 1£ºÓÐ£¬0£ºÃ»ÓÐ
u8 RDS_OK;
u8 RDS_Sta;
u8 RDS_Block[12];
u8 RDS_BUF[10];
}__RADIO_Inf;
extern __RADIO_Inf		RADIO_Inf;


//³µÉí×´Ì¬ÐÅÏ¢
typedef struct{
//u8 NUM;			//ÐÅÏ¢±àºÅ
//u8 STA;			//×´Ì¬
u16 Batter_Value;//µç³ØµçÑ¹	
u8 NIGHTLIGHT;	//Ò¹µÆ¿ª£¬µ÷°µ±³¹â	0£º°×Ìì£¬1£ºÒ¹¼ä
u8 ReverseSta;	//µ¹³µ×´Ì¬			0£ºÕý³£ÐÐ³µ£¬1£ºµ¹³µ
u8 DIVEDOOR;	//¼ÝÊ»ÊÒÃÅ			0£º¹Ø±Õ£¬1£º´ò¿ª
u8 PSNGDOOR;	//¸±¼ÝÊ»ÃÅ			0£º¹Ø±Õ£¬1£º´ò¿ª
u8 ACC;			//ACC×´Ì¬			0£ºÏÂµç£¬1£ºÉÏµç
u8 ACCDe;		//µÍÑ¹ÑÓÊ±µÄACCÐÅºÅ	0£ºÏÂµç£¬1£ºÉÏµç
u8 POWER;		//Õû³µµçÔ´×´Ì¬		0£ºOFF£¬1£ºACC£¬2£ºON
u8 KEY_STS;		//Ô¿³×Î»ÖÃ			1£ºStop and Key absent£¬2£ºReserved£¬4£ºON£¬6£ºStop and Key In£¬C£ºReserved£¬E£ºError£¬F£ºNot Avaliable£¬ÆäËû£º±£Áô
u8 LTurnSts;	//×ó×ªÏòµÆ			0£º¹Ø£¬1£º¿ª
u8 RTurnSts;	//ÓÒ×ªÏòµÆ			0£º¹Ø£¬1£º¿ª
u16 Car_Speed;	//Ç°ÖáÆ½¾ùËÙ¶È
u8	Lock;		//³µËø				0£º±£Áô£¬1£ºÃÅ½âËø£¬2£ºÃÅÉÏËø
u8 Door_lock;
}__CAR_Inf;
extern __CAR_Inf		CAR_Inf;


//ÏµÍ³×´Ì¬ÐÅÏ¢
typedef struct{
u8 ACC_AWAKE;
u8 STA;			//ÏµÍ³×´Ì¬	0£ºÐÝÃß£»1£º»½ÐÑ¹ý³Ì£¬2£ºÕý³£¹¤×÷ÖÐ£¬3£ºÉý¼¶Êý¾Ý½ÓÊÕ¹ý³ÌÖÐ£»
u8 MPU_STA;		//MPU×´Ì¬   0£ºµÈ´ýÁ¬Ïß£»1£ºÕý³£ÔÚÏß£¬2£ºMPUÇëÇó¹Ø»ú£»3£ºMPUÉý¼¶ÀëÏß£»4£º¹Ø»ú×´Ì¬£»5£º¹ÊÕÏ
u8 BATTER_VERR;	//µçÑ¹±£»¤±êÖ¾
u8 MUTE_STA;
u8 MUTE_OK;		//¸Õ¿ª»úÐèÒªÖ±½Ó¾²Òô
u8 OFF_TIME;
u8 START_STA;
u8 KEY_ENABLE;	//·½ÏòÅÌ°´¼üÊ¹ÄÜ Îª0£¬°´¼ü¿ÉÓÃ£¬Îª1£¬ÆÁ±Î°´¼ü
u8 Batter_sta;	//0:Õý³££¬1£ºµÍÓÚ13V£¬2£ºµÍÓÚ12V£
}__SYS_Inf;
extern __SYS_Inf		SYS_Inf;


//°´¼ü×´Ì¬ÐÅÏ¢
typedef struct{
u8 NUM;			//°´¼ü±àºÅ	8¸ö°´¼ü
u8 STA;			//°´¼ü×´Ì¬	0£ºÊÍ·Å£¬1£º¶Ì°´£¬2£º³¤°´	
}__KEY_Inf;
extern __KEY_Inf		KEY_Inf;


//»·ÊÓÊÓ½Ç×´Ì¬
//typedef enum{
//FRONT=1,		//Ç°
//AFFTER,			//ºó
//LEFT,			//×ó
//RIGHT,			//ÓÒ
//}__LAROUND_Sta;
//extern __LAROUND_Sta	LAROUND_Sta;

//»·ÊÓÊÓ½Ç×´Ì¬
typedef struct{
u8 STA;			//»·ÊÓ×´Ì¬	0£ºµÈ´ýÁ¬Ïß£¬1£ºÕý³£¹¤×÷£»2£º¹ÊÕÏ
u8 VIS;			//ÊÓ½Ç		0£ºÍ£Ö¹Í¼ÏñÊä³ö£»1£ºÇ°£»2£ººó£»3£º×ó£»4£ºÓÒ£¬
}__LAROUND_Inf;
extern __LAROUND_Inf	LAROUND_Inf;

//ÒôÏìÔ´ÐÅÏ¢
typedef struct{
u8 source;		//Ô´
u8 playsta;		//²¥·Å×´Ì¬
u32 radiofrq;	//ÊÕÒôÆµÂÊ
u8 volume;		//ÒôÁ¿
u8 mutesta;		//½ûÒô
u8 phonesta;	//Í¨»°×´Ì¬
}__VOICE_Inf;
extern __VOICE_Inf	VOICE_Inf;

//À¶ÑÀÉè±¸ÐÅÏ¢
typedef struct{
u8 sta;			//Á¬½Ó×´Ì¬
u8 name_len;	//Éè±¸Ãû³¤¶È
u8 time;		//·¢ËÍ¼ÆÊ±ÓÃ	
u8 name[36];	//Éè±¸Ãû
}__BLUE_Inf;
extern __BLUE_Inf	BLUE_Inf;

//Í¨»°ÐÅÏ¢
typedef struct{
u8 sta;			//×´Ì¬
u8 time;	
u8 name_len;	//Éè±¸Ãû³¤¶È
u8 num_len;	//Éè±¸Ãû³¤¶È
u8 name[36];	//Éè±¸Ãû
u8 number[36];	//Éè±¸Ãû
}__PHONE_Inf;
extern __PHONE_Inf	PHONE_Inf;

//¹¤³§Ä£Ê½ÐÅÏ¢
typedef struct{
u8 sta;			//×´Ì¬

}__FACTOR_Inf;
extern __FACTOR_Inf	FACTOR_Inf;

//Æô¶¯Ô­Òò
typedef enum{
ACCON = 0,			//ACC ON
OTRESET = 1,		//³¬Ê±¸´Î»
STRESET = 2,		//ÉèÖÃ¸´Î»
PREBOOT = 3,		//Ô¤Æô¶¯
STARTNOB = 4,		//¶ÏB+Æô¶¯
}__START_Reason;
extern __START_Reason	START_Reason;

extern u8 MCU_Awak;		//ÏµÍ³»½ÐÑ×´Ì¬£¬0£ºÐÝÃß£¬1£º»½ÐÑ£¬
extern u8 ContTime;		//×Ô¶¯¹Ø»úµ¹¼ÆÊ±

extern char radio_version[30];//ÊÕÒô»ú°æ±¾ºÅ
#define RVersion_SIZE  strlen(radio_version)	//radio°æ±¾ºÅ³¤¶È

extern const char *Version;		//°æ±¾ºÅ
extern const char *Version2;		//°æ±¾ºÅ
#define Version_SIZE strlen(Version)	//°æ±¾ºÅ³¤¶È
#define Version2_SIZE strlen(Version2)	//°æ±¾ºÅ³¤¶È

extern u8 Video_sta;








#endif







