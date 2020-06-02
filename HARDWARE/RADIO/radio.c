/**
*file:radio.c
*date:2018.8.8
*author:GongYao
*
*The configuration and drive of the radio TEF6686 module.
*
*
**/
#include "radio.h"
#include "i2c.h"

//#include "patch.h"
//#include "patch2.h"	//旧版
#include "Tuner_Patch_Lithio_V101_p119.h"		//151版
//#include "Tuner_Patch_Lithio_V102_p224.h"		//
//#include "Tuner_Patch_Lithio_V205_p512.h"		//

#include "usart.h" 
#include "delay.h"

#include "information.h"

//#include "reportTASK.h"
//#include "radioTASK.h"

/**********************************************************************************/

#define TEF665x_SlaveAddr 0xC8
	 
	 
	 
u8 TEF6686_rdsSet(u8 mode); 
	 
	 
//	FM	6500 … 10800		65.00 … 108.00 MHz / 10 kHz step size
//	AM	LW 144 … 288		144 … 288 kHz / 1 kHz step size
//		MW 522 … 1710		522 … 1710 kHz / 1 kHz step size
//		SW 2300 … 27000	2.3 … 27 MHz / 1 kHz step	 
	 
	 
//配置指令数组
static const unsigned char devTEF665x_Patch_CmdTab1[] = {3,	0x1c,0x00,0x00};
static const unsigned char devTEF665x_Patch_CmdTab2[] = {3,	0x1c,0x00,0x74};
static const unsigned char devTEF665x_Patch_CmdTab3[] = {3,	0x1c,0x00,0x75};

#ifdef RDS_ENABLE
	const u16  Frequency[4]={8750,10800,522,1620};
#else
	const u16  Frequency[4]={8750,10800,531,1602};
#endif

/**********************************************************************************/
//return SUCCESS if i2c ok
static int devTEF665x_Write(unsigned char * buf,u32 len)
{
	return I2C2_WriteData(TEF665x_SlaveAddr,buf, len);
}
//END

/**
*-函数原型：
*-功能：写配置指令
*-
*-
**/
static int devTEF665x_WriteTab(const unsigned char * tab)
{
	unsigned char buf[32];
	int len;
	int i;

	len = tab[0];
	for(i=0;i<len;i++)	buf[i] = tab[i+1];
	return devTEF665x_Write(buf, len);
}
//END


#define TEF665X_SPLIT_SIZE		24
static int devTEF665x_Patch_Load(const unsigned char * pLutBytes, int16_t size)
{
	unsigned char buf[TEF665X_SPLIT_SIZE+1];
	int16_t i,len;
	int r;
	buf[0] = 0x1b;
	while(size)
	{
		len = (size>TEF665X_SPLIT_SIZE)? TEF665X_SPLIT_SIZE : size;
		size -= len;
		for(i=0;i<len;i++)	buf[1+i] = pLutBytes[i];
		pLutBytes+=len;
		if(SUCCESS != (r = devTEF665x_Write(buf, len+1)))	break;
	}

	return r;
}
//END


/**
*-函数原型：
*-功能：写配置
*-
**/
int devTEF665x_Patch_Init(void)
{
	int r;
//[ w 1C 0000 ]
	if(SUCCESS != (r = devTEF665x_WriteTab(devTEF665x_Patch_CmdTab1)))  return r;
//[ w 1C 0074 ]
	if(SUCCESS != (r = devTEF665x_WriteTab(devTEF665x_Patch_CmdTab2)))	return r;
//table1	
	if(SUCCESS != (r = devTEF665x_Patch_Load(pPatchBytes, PatchSize)))	return r;
//[ w 1C 0000 ]
	if(SUCCESS != (r = devTEF665x_WriteTab(devTEF665x_Patch_CmdTab1)))  return r;
//[ w 1C 0075 ]
	if(SUCCESS != (r = devTEF665x_WriteTab(devTEF665x_Patch_CmdTab3)))	return r;
//table2
	if(SUCCESS != (r = devTEF665x_Patch_Load(pLutBytes, LutSize)))    	return r;
//[ w 1C 0000 ]
	if(SUCCESS != (r = devTEF665x_WriteTab(devTEF665x_Patch_CmdTab1)))	return r;
	return r;	
}
//END


u8 TEF6686_Rest(void)
{
	u8 Buff[5]={0x1E,0x5A,0x01,0x5A,0x5A,};
	if(!I2C2_WriteData(TEF665x_SlaveAddr,Buff,5))
		if(!I2C2_WriteData(TEF665x_SlaveAddr,Buff,5))return 1;// 复位 
	return 0;
}
/**
*-函数原型：u8 TEF6686_Start(void)
*-功能：    配置 TEF6686 
*-输入：    无
*-返回：			结果：0：成功；
*-							其他：失败。
**/
u8 TEF6686_Start(void)
{
	u8 ErrTime = 0;
	u8 Buff[10]={0x1E,0x5A,0x01,0x5A,0x5A,0x18,0xAB,0x00,0x00,0x00};

	delay_xms(100);
	if(!I2C2_WriteData(TEF665x_SlaveAddr,Buff,5))
		if(!I2C2_WriteData(TEF665x_SlaveAddr,Buff,5))return 1;// 复位 
	delay_xms(1000);

	while((!devTEF665x_Patch_Init())&&(ErrTime<20))ErrTime++; // 配置
	if(ErrTime>=20) return 2;
	delay_xms(500);

	Buff[0] = 0x14;Buff[1] = 0x00;Buff[2] = 0x01;
	if(!I2C2_WriteData(TEF665x_SlaveAddr,Buff,3))return 3;// 启动  
	delay_xms(100);

	//Buff[0] = 0x40;Buff[1] = 0x04;Buff[2] = 0x01;Buff[3] = 0x02;Buff[4] = 0xDF;
	Buff[0] = 0x40;Buff[1] = 0x04;Buff[2] = 0x01;Buff[3] = 0x00;Buff[4] = 0x8C;Buff[5] = 0xA0;Buff[6] = 0x00;
	if(!I2C2_WriteData(TEF665x_SlaveAddr,Buff,9))return 4;// 配置时钟 
	delay_xms(100);

	Buff[0] = 0x40;Buff[1] = 0x05;Buff[2] = 0x01;Buff[3] = 0x00;Buff[4] = 0x01;
	if(!I2C2_WriteData(TEF665x_SlaveAddr,Buff,5))return 5;// 激活设备  
	delay_xms(100);

	Buff[0] = 0x30;Buff[1] = 0x0B;Buff[2] = 0x01;Buff[3] = 0x00;Buff[4] = 0x00;
	if(!I2C2_WriteData(TEF665x_SlaveAddr,Buff,5))return 6;// 关闭静音 
	delay_xms(100);

#ifdef RDS_ENABLE
	TEF6686_rdsSet(1);
#endif

//	Buff[1] = 0x01;Buff[2] = 0x01;Buff[3] = 0x00;Buff[4] = 0x01;
//	if(RADIO_Inf.MODE == 0)
//	{
//		Buff[0] = 0x20;
//		Buff[5] = (u8)(((RADIO_Inf.FM_FRQ*10)>>8)&0x00FF);
//		Buff[6] = (u8)((RADIO_Inf.FM_FRQ*10)&0x00FF);
//	}
//	else
//	{
//		Buff[0] = 0x21;
//		Buff[5] = (u8)((RADIO_Inf.AM_FRQ>>8)&0x00FF);
//		Buff[6] = (u8)(RADIO_Inf.AM_FRQ&0x00FF);	
//	}
//	Buff[0] = 0x20;Buff[1] = 0x01;Buff[2] = 0x01;Buff[3] = 0x00;Buff[4] = 0x01;Buff[5] = 0x22;Buff[6] = 0x2E;
//	if(!I2C2_WriteData(TEF665x_SlaveAddr,Buff,7))// 设置为 FM 87.5 MHz 
//	{
//		delay_xms(100);
//		if(!I2C2_WriteData(TEF665x_SlaveAddr,Buff,7))
//		return 7;
//	}// 设置为 FM 87.5 MHz
	TEF6686_TuneTo();
	RADIO_Inf.WORK_STA = 1;
	return 0;
}
//END

/**
*-函数原型：void TEF6686_Mute(u8 mute)
*-功能：    静音TEF6686
*-输入：    mute：0：不静音；1：静音
*-返回：    
**/
u8 TEF6686_Mute(u8 mute)
{
	u8 buff[5]={0x30,0x0B,0x01,0x00,0x00};

	if(mute == 1) buff[4] = 0x01;
	if(!I2C2_WriteData(TEF665x_SlaveAddr,buff,5))
		if(!I2C2_WriteData(TEF665x_SlaveAddr,buff,5))return 1;
	delay_ms(1);
	I2C2_WriteData(TEF665x_SlaveAddr,buff,5);
	return 0;
}
//END

/**
*-函数原型：u8 TEF6686_Low(u8 low)
*-功能：    低功耗开关
*-输入：    low：0：正常模式；1：低功耗
*-返回：    
**/
u8 TEF6686_Low(u8 low)
{
	u8 buff[5]={0x40,0x01,0x01,0x00,0x00};

	if(low == 1) buff[4] = 0x01;
	if(!I2C2_WriteData(TEF665x_SlaveAddr,buff,5))
		if(!I2C2_WriteData(TEF665x_SlaveAddr,buff,5))return 1;

	return 0;
}
//END

u8 TEF6686_rdsGet(u8 *data);
u16 Get_FMlevel(u16 frq)
{
	u8 buff[31]={0x20,0x01,0x01,0x00,0x01,0x00,0x00};
	u16 level;
//	TEF6686_Mute(1);//静音
	buff[5] = (u8)(frq>>8)&0xFF;buff[6] = (u8)(frq&0xFF);
	I2C2_WriteData(TEF665x_SlaveAddr,buff,7);
	delay_ms(50);
	//读取信号灵敏度信息
	buff[1] = 0x81;buff[2] = 0x01;
	I2C2_WriteData(TEF665x_SlaveAddr,buff,3);
	I2C2_ReadData(TEF665x_SlaveAddr+1,buff,31);
	level =	buff[2]<<8|buff[3];
//	TEF6686_Mute(0);//取消静音
	TEF6686_TuneTo();//回原来频点
	return level;
}







u8 Get_RadioQuality(void)
{
	u8 buff[31]={0x20,0x81,0x01,0x00,0x01,0x00,0x00};
	u8 Tbuff[31]={0x20,0x85,0x01};	
	
	u16 FMSM_TH = RADIO_Inf.FM_LEVEL;
	u16 level,usn,wam,offset,bandwidth,IFC;
	u8 Sta = 0;	
	u8 Stereo = 0;
	
	I2C2_WriteData(TEF665x_SlaveAddr,buff,3);
	I2C2_ReadData(TEF665x_SlaveAddr+1,buff,31);
	delay_ms(50);
	I2C2_WriteData(TEF665x_SlaveAddr,Tbuff,3);
	I2C2_ReadData(TEF665x_SlaveAddr+1,Tbuff,31);
	Stereo = Tbuff[0]==0x80?1:0;
	
	level		=	buff[2]<<8|buff[3];
	usn			=	buff[4]<<8|buff[5];
	wam			=	buff[6]<<8|buff[7];
	offset		=	buff[8]<<8|buff[9];
	bandwidth	=	buff[10]<<8|buff[11];
	if(offset<0x8000) IFC = offset;
	else IFC = 0x10000 - offset;
	
	if((Stereo == 1)&&(level >= 0xF0)&&(usn < 0xfa)&&(IFC < 0x30)) Sta = 1;
	if((bandwidth > 0x07D5)&&(level >= FMSM_TH)&&(usn < 0x70)&&(wam < 0x70)&&(IFC < 0x4A)) Sta = 1;
	
	return Sta;
}




u8 TEF6686_rdsSet(u8 mode)
{
	u8 buff[9]={0x20,0x51,0x01,0x00,0x00,0x00,0x02,0x00,0x00};

	if(mode == 1) buff[4] = 0x01;
	if(!I2C2_WriteData(TEF665x_SlaveAddr,buff,9))
		if(!I2C2_WriteData(TEF665x_SlaveAddr,buff,9))return 1;
	return 0;
}

#define RDS_DEBUG	0
u8 TEF6686_rdsGet(u8 *data)
{
	u16 res = 0;
	u8 buff[31]={0x20,0x82,0x01,0x00,0x00};

	I2C2_WriteData(TEF665x_SlaveAddr,buff,3);
	delay_ms(2);
	I2C2_ReadData(TEF665x_SlaveAddr+1,buff,31);

	RADIO_Inf.RDS_BUF[0] = buff[0];
	RADIO_Inf.RDS_BUF[1] = buff[1];
	RADIO_Inf.RDS_BUF[2] = buff[2];
	
#if	1
	if(((buff[0] == 0x82)||(buff[0] == 0xC2))&&(buff[1] == 0x00))
#else
	if((buff[0] == 0x82)&&(buff[1] == 0x00))	
#endif
	{
		
#if (RDS_DEBUG)
		printf("RDS STA:0x%x 0x%x;",buff[0],buff[1]);
#endif
		delay_ms(100);
		buff[0] = 0x20;
		buff[1] = 0x83;
		buff[2] = 0x01;
		I2C2_WriteData(TEF665x_SlaveAddr,buff,3);
		delay_ms(2);
		I2C2_ReadData(TEF665x_SlaveAddr+1,buff,31);
#if (RDS_DEBUG)
		printf("Value:");
#endif
		for(u8 i=0;i<12;i++)
		{
			data[i] = buff[i];
#if (RDS_DEBUG)
			if(buff[i]>0xF)printf("0x%x ",buff[i]);
			else printf("0x0%x ",buff[i]);
#endif
		}
		res = 1;
#if (RDS_DEBUG)
		printf("\r\n");
#endif
	}
	else if((buff[0]&0x80) == 0x80)
	{
		buff[0] = 0x20;
		buff[1] = 0x83;
		buff[2] = 0x01;
		I2C2_WriteData(TEF665x_SlaveAddr,buff,3);
		delay_ms(2);
		I2C2_ReadData(TEF665x_SlaveAddr+1,buff,31);
	}
	
	return res;
}





struct{
u8 sta;
u8 num;
u8 sort_flag;
u8 af_level_cnt;
u8 af_cnt;
u8 lf_cnt;
u16 PI_Code;
u16 check_cycle_time;
u16 af_list[2][32];//list and level
u16 lf_list[2][32];
}RDS_Info;
void RDS_Clean(void)
{
	RDS_Info.sta = 0;
	RDS_Info.num = 0;
	RDS_Info.af_cnt = 0;
	RDS_Info.lf_cnt = 0;
	RDS_Info.sort_flag = 0;
	RDS_Info.check_cycle_time = 0;
//	printf("clean\r\n");
}
#define RDS_CHECKNUM	3
u16 Get_FMlevel2(u16 frq)
{
	u8 buff[31]={0x20,0x01,0x01,0x00,0x01,0x00,0x00};
	u16 level;
	
//	TEF6686_Mute(1);//静音
	
	buff[5] = (u8)(frq>>8)&0xFF;buff[6] = (u8)(frq&0xFF);
	I2C2_WriteData(TEF665x_SlaveAddr,buff,7);
	
	delay_ms(150);
	//读取信号灵敏度信息
	buff[1] = 0x81;buff[2] = 0x01;
	I2C2_WriteData(TEF665x_SlaveAddr,buff,3);
	I2C2_ReadData(TEF665x_SlaveAddr+1,buff,31);
	level =	buff[2]<<8|buff[3];
	
	delay_ms(20);
	u8 i =200;
	u16 last_code = 0xFFFF;
	u16 this_code = 0;
	while(i--)
	{
		if(TEF6686_rdsGet(buff))
		{
			this_code = (buff[2]<<8)|buff[3];
			if(this_code == last_code)
			{
				if(this_code == RDS_Info.PI_Code)
				{
					RDS_Info.af_list[0][0] = frq/10;
					RDS_Info.sta = 5;
				}
//				printf("% d have PI Code:%d  ..........................\r\n",frq,this_code);
				break;
			}
			last_code = this_code;
		}
	}
//	TEF6686_Mute(0);//取消静音
	
	TEF6686_TuneTo();//回原来频点
	TEF6686_rdsGet(buff);
	return level;
}

u8 TuneTo_Check(u16 frq)
{
	u8 re = 0;
	u8 buff[31]={0x20,0x01,0x01,0x00,0x01,0x00,0x00};
	buff[5] = (u8)(frq>>8)&0xFF;buff[6] = (u8)(frq&0xFF);
	I2C2_WriteData(TEF665x_SlaveAddr,buff,7);
	delay_ms(200);
	if(Get_RadioQuality())
	{
		re = 1;
	}
	else
	{
		if(Get_RadioQuality())re = 1;
		else re = 0;
	}
	
	TEF6686_TuneTo();//回原来频点
	return re;
}



void My_Sort(u16 (*buf)[32],u8 len);
void RDS_Check(void)
{
	static u8 LAST_rds_sta;
	static u16 cnt = 0;
	static u8 times_cnt = 0;
	u8 block_buff[12];
	
	if(cnt < 6)cnt++;//60 ms 
	else cnt = 0;
	
	if(cnt == 1)//数据状态为0，需要做延时检测
	{
		RADIO_Inf.RDS_Sta = TEF6686_rdsGet(RADIO_Inf.RDS_Block);
		if(RADIO_Inf.RDS_Sta) times_cnt = 0,RADIO_Inf.RDS_OK = 1;
		else
		{
			if(times_cnt<6)times_cnt++;
			if(times_cnt == 5)
			{
				RADIO_Inf.RDS_OK = 1;
			}
		}
	}
	
	if(cnt == 2)
	{
		if(RADIO_Inf.RDS_Sta)
		{
			if(RDS_Info.sort_flag != 0)return;
			if((RADIO_Inf.RDS_Block[4]&0xF8)==0x00)
			{
				RDS_Info.PI_Code = (RADIO_Inf.RDS_Block[2]<<8)|RADIO_Inf.RDS_Block[3];
				if(((RDS_Info.af_cnt+RDS_Info.lf_cnt)>=RDS_Info.num)&&(RDS_Info.num != 0))return;
				if(RDS_Info.sta == 0)RDS_Info.sta = 1;
				if(RADIO_Inf.RDS_Block[6] == 250)//One LF/MF frequency follows
				{
					u8 i=0;
					u16 lf_temp = 0;
					lf_temp = (RADIO_Inf.RDS_Block[7]-16)*9+Frequency[2];
					if((lf_temp<Frequency[2])||(lf_temp>Frequency[3])) return;//超了
					
					for(i=0;i<RDS_Info.lf_cnt;i++)
					{
						if(lf_temp == RDS_Info.lf_list[0][i])break;
					}
					if(i == RDS_Info.lf_cnt)
					{
						RDS_Info.lf_list[0][RDS_Info.lf_cnt++] = lf_temp;
//						printf("get %d,num %d\r\n",lf_temp,RDS_Info.lf_cnt);
					}
				}
				else if((RADIO_Inf.RDS_Block[6] >= 225)&&(RADIO_Inf.RDS_Block[6] <= 249))//25 AFs follow
				{
					RDS_Info.num = RADIO_Inf.RDS_Block[6] - 224;
					if((RADIO_Inf.RDS_Block[7]<205)&&(RADIO_Inf.RDS_Block[7]!=0))
					{
						u8 i = 0;
						u16 af_temp = 0;
						af_temp = (RADIO_Inf.RDS_Block[7] + 875);
						for(i=0;i<RDS_Info.af_cnt;i++)
						{
							if(af_temp == RDS_Info.af_list[0][i])break;
						}
						if(i == RDS_Info.af_cnt) 
						{
							RDS_Info.af_list[0][RDS_Info.af_cnt++] = af_temp;
//							printf("get %d,num %d\r\n",af_temp,RDS_Info.af_cnt);
						}
					}
				}
				else if((RADIO_Inf.RDS_Block[6]<205)&&(RADIO_Inf.RDS_Block[6]!=0))
				{
						u8 i = 0;
						u16 af_temp = 0;
						af_temp = (RADIO_Inf.RDS_Block[6] + 875);
					
						for(i=0;i<RDS_Info.af_cnt;i++)
						{
							if(af_temp == RDS_Info.af_list[0][i])break;
						}
						if(i == RDS_Info.af_cnt) 
						{
							RDS_Info.af_list[0][RDS_Info.af_cnt++] = af_temp;
//							printf("get %d,num %d\r\n",af_temp,RDS_Info.af_cnt);
						}
						if((RADIO_Inf.RDS_Block[7]<205)&&(RADIO_Inf.RDS_Block[7]!=0))
						{
							af_temp = (RADIO_Inf.RDS_Block[7] + 875);
							for(i=0;i<RDS_Info.af_cnt;i++)
							{
								if(af_temp == RDS_Info.af_list[0][i])break;
							}
							if(i == RDS_Info.af_cnt) 
							{
								RDS_Info.af_list[0][RDS_Info.af_cnt++] = af_temp;
//								printf("get %d,num %d\r\n",af_temp,RDS_Info.af_cnt);
							}
						}
				}
//				printf("RDS 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\r\n",RADIO_Inf.RDS_Block[2],RADIO_Inf.RDS_Block[3],RADIO_Inf.RDS_Block[4],RADIO_Inf.RDS_Block[5],RADIO_Inf.RDS_Block[6],RADIO_Inf.RDS_Block[7],RADIO_Inf.RDS_Block[8],RADIO_Inf.RDS_Block[9]);
//				printf("b0:%d ",(RADIO_Inf.RDS_Block[4]>>3)&0x01);
//				printf("tp:%d ",(RADIO_Inf.RDS_Block[4]>>2)&0x01);
//				printf("pty:%x ",(((RADIO_Inf.RDS_Block[4]<<8)+RADIO_Inf.RDS_Block[5])>>5)&0x1F);
//				printf("ab:%d ",(RADIO_Inf.RDS_Block[5]>>4)&0x01);
//				printf("ad:%d \r\n",(RADIO_Inf.RDS_Block[5]>>0)&0x0f);	
			}
		}
	}
	if(cnt == 3)//检查信号,信号不好的时候去检索列表，找出信号最好的，跳过去
	{
		static u8 Quality_cnt = 0;
		static u8 Quality_times = 0;
		if(RDS_Info.sta == 1)//检查
		{
			if(Quality_cnt<10)Quality_cnt++;
			else
			{
				Quality_cnt = 0;
				if(0 == Get_RadioQuality()) Quality_times++;
				else						Quality_times = 0;
				if(Quality_times >= 5)
				{
					Quality_times = 0;
					RDS_Info.sta = 2;
				}
			}
		}
		else if(RDS_Info.sta == 2)//第一遍遍历
		{
			if((RDS_Info.num ==0 )||(RDS_Info.num > 25))
			{
				RDS_Clean();
				RDS_Info.sta = 0;
				return;
			}
			if(RDS_Info.af_level_cnt<RDS_Info.af_cnt)
			{
				RDS_Info.af_list[1][RDS_Info.af_level_cnt]=Get_FMlevel(RDS_Info.af_list[0][RDS_Info.af_level_cnt]*10);
				RDS_Info.af_level_cnt++;
				cnt = 2;
			}
			else 
			{
				RDS_Info.sta = 3;
				RDS_Info.af_level_cnt = 0;
				cnt = 2;
			}
		}
		else if(RDS_Info.sta == 3)//第一次排序
		{
			if(RDS_Info.af_cnt > 1)My_Sort(RDS_Info.af_list,RDS_Info.af_cnt);
//			printf("af_list:");
//			for(u8 i=0;i<RDS_Info.af_cnt;i++)printf(" %d",RDS_Info.af_list[0][i]);
//			printf(";\r\n");
//			printf("lv_list:");
//			for(u8 i=0;i<RDS_Info.af_cnt;i++)printf(" %d",RDS_Info.af_list[1][i]);
//			printf(";\r\n");
			RDS_Info.sta = 4;
			cnt = 2;
		}
		else if(RDS_Info.sta == 4)//第二次遍历
		{
			if(RDS_Info.af_cnt>RDS_CHECKNUM)
			{
				if(RDS_Info.af_level_cnt<RDS_CHECKNUM)
				{
					
					RDS_Info.af_list[1][RDS_Info.af_level_cnt]=Get_FMlevel2(RDS_Info.af_list[0][RDS_Info.af_level_cnt]*10);
					RDS_Info.af_level_cnt++;
					cnt = 2;
				}
				else 
				{
					RDS_Info.sta = 5;
					cnt = 2;
				}	
			}
			else
			{
				if(RDS_Info.af_level_cnt<RDS_Info.af_cnt)
				{
					RDS_Info.af_list[1][RDS_Info.af_level_cnt]=Get_FMlevel2(RDS_Info.af_list[0][RDS_Info.af_level_cnt]*10);
					RDS_Info.af_level_cnt++;
					cnt = 2;
				}
				else 
				{
					RDS_Info.sta = 5;
					RDS_Info.af_level_cnt = 0;
					cnt = 2;
				}
			}
		}
		else if(RDS_Info.sta == 5)//可以跳了
		{
			if(RDS_Info.af_cnt > 1)
			{
				if(RDS_Info.af_cnt>RDS_CHECKNUM)My_Sort(RDS_Info.af_list,RDS_CHECKNUM);
				else	My_Sort(RDS_Info.af_list,RDS_Info.af_cnt);
			}
//			printf("af_list:");
//			if(RDS_Info.af_cnt>RDS_CHECKNUM)	for(u8 i=0;i<RDS_CHECKNUM;i++)printf(" %d",RDS_Info.af_list[0][i]);
//			else for(u8 i=0;i<RDS_Info.af_cnt;i++)printf(" %d",RDS_Info.af_list[0][i]);
//			printf(";\r\n");
//			
//			printf("lv_list:");
//			if(RDS_Info.af_cnt>RDS_CHECKNUM)	for(u8 i=0;i<RDS_CHECKNUM;i++)printf(" %d",RDS_Info.af_list[1][i]);
//			else for(u8 i=0;i<RDS_Info.af_cnt;i++)printf(" %d",RDS_Info.af_list[1][i]);
//			printf(";\r\n");
			
			if(TuneTo_Check(RDS_Info.af_list[0][0]*10))//信号足够好才去
			{
				RADIO_Inf.FM_FRQ = RDS_Info.af_list[0][0]*10;
				RDS_Info.sta = 0;
				RADIO_Inf.CMD = 2;//正式跳过去
			}
			else
			{
				RDS_Info.check_cycle_time = 0;
				RDS_Info.sta = 6;
			}
//			printf("RDS_Info.num:%d,af_cnt:%d;lf_cnt:%d\r\n",RDS_Info.num,RDS_Info.af_cnt,RDS_Info.lf_cnt);
		}
		else if(RDS_Info.sta == 6)//信号太差，等会儿再检查
		{
			static u8 Quality_times = 0;
			
			if((RDS_Info.check_cycle_time>=10000)||(Quality_times>=5))//100为6s
			{
				Quality_times = 0;
				RDS_Info.sta = 1;
			}
			else RDS_Info.check_cycle_time++;

			if((RDS_Info.check_cycle_time%10) == 0)
			{
				if(Get_RadioQuality())Quality_times++;
				else Quality_times = 0;
			}
			
		}
	}
	if(cnt == 4)
	{
//		printf("RDS_Info.sta : %d \r\n",RDS_Info.sta);
	}
}


/**
*-函数原型：u16 TEF6886_GetSta(u8 mode)
*-功能：			获得当前频道信息
*-输入：			模式
*-返回：			频道值
**/
u16 TEF6886_GetSta(u8 mode)
{
	u16 Frq = 0;
	u8 buff[31]={0x20,0x87,0x01,0x00,0x00};
	
	if(mode) buff[0] = 0x21;

	I2C2_WriteData(TEF665x_SlaveAddr,buff,3);
	I2C2_ReadData(TEF665x_SlaveAddr+1,buff,31);

	Frq = (buff[2]<<8)|buff[3];
	
	return Frq;
}
//END
//以下为功能函数

/**
*-函数原型：u8 TEF6686_FMSeek(u8 *buff,u16 Freq)
*-功能：			检查信号质量(注意不要直接调用，由 TEF6686_TuneTo（）调用)
*-输入：			buff：质量信息，Freq：频率
*-输出：			0；无信号，1：好台
**/
u8 TEF6686_FMSeek(u8 *buff,u16 Freq)
{
	u16 FMSM_TH = RADIO_Inf.FM_LEVEL;
	u16 level,usn,wam,offset,bandwidth,IFC;
	u8 Tbuff[31]={0x20,0x85,0x01};
	u8 Sta = 0;
	I2C2_WriteData(TEF665x_SlaveAddr,Tbuff,3);
	I2C2_ReadData(TEF665x_SlaveAddr+1,Tbuff,31);
	RADIO_Inf.FA_Stereo = Tbuff[0]==0x80?1:0;
	
	level		=	buff[2]<<8|buff[3];
	usn			=	buff[4]<<8|buff[5];
	wam			=	buff[6]<<8|buff[7];
	offset		=	buff[8]<<8|buff[9];
	bandwidth	=	buff[10]<<8|buff[11];
	if(offset<0x8000) IFC = offset;
	else IFC = 0x10000 - offset;
	
//	if((stereo == 1)&&(level >= 0x90)&&(IFC < 0xA4)) Sta = 1;
//	if((level >= FMSM_TH)&&(usn < 0x80)&&(wam < 0xAF)&&(bandwidth > 0x07C0)&&(IFC < 0x8A)) Sta = 1;
	
	if((RADIO_Inf.FA_Stereo == 1)&&(level >= 0xF0)&&(usn < 0xfa)&&(IFC < 0x30)) Sta = 1;
	if((bandwidth > 0x07D5)&&(level >= FMSM_TH)&&(usn < 0x70)&&(wam < 0x70)&&(IFC < 0x4A)) Sta = 1;
	
	RADIO_Inf.FM_STNR = usn;//信噪比
	RADIO_Inf.FM_FRES = IFC;//频率响应 
	
//	else if((Freq == 9910)&&(bandwidth > 0x0500)&&(level >= FMSM_TH)&&(usn < 0x9A)&&(wam < 0x9A)&&(IFC < 0x6A)) Sta = 1;
//	else if((Freq == 9420)&&(level >= FMSM_TH)&&(usn < 0x80)&&(wam < 0x80)&&(IFC < 0x6A)) Sta = 1;	
	
//	printf("level=0x%x,usn=0x%x,wam=0x%x,offset=0x%x,bandwidth=0x%x,IFC=0x%x,stereo=0x%x;\r\n",level,usn,wam,offset,bandwidth,IFC,stereo);
//	printf("Sta=%d;\r\n",Sta);


//	TEF6686_rdsGet();
	
	return Sta;
}
//END


u8 TEF6686_AMSeek(u8 *buff,u16 Freq)
{
	u16 AMSM_TH = RADIO_Inf.AM_LEVEL;
	u16 level,offset,IFC;
	u16 bandwidth,usn,co_channel;
	u8 Sta = 0;
	
	level		= buff[2]<<8|buff[3];
	usn		    = buff[4]<<8|buff[5];
	co_channel	= buff[6]<<8|buff[7];
	offset		= buff[8]<<8|buff[9];
	bandwidth	= buff[10]<<8|buff[11];
	
	if(level&0x8000)level =  (~level)+1;
	
	if(offset&0x8000) IFC = (~offset)+1;
	else IFC = offset;
	
	if((level >= 0xFF00)||(IFC >= 0x10)) {;}
	else if((level >= AMSM_TH)&&(level < 0xFF00)&&(IFC < 0x08)) Sta = 1;
	
//	printf("level=0x%x,usn=0x%x,offset=0x%x,bandwidth=0x%x,IFC=0x%x,co_channel=0x%x;\r\n",level,usn,offset,bandwidth,IFC,co_channel);
//	printf("Sta=%d;\r\n",Sta);
		
		
//	TEF6686_rdsGet();
		
	return Sta;
}
//END


u8 TEF6686_FRQSet(u8 mode,u16 frq)
{
	u8 buff[31]={0x20,0x01,0x01,0x00,0x01,0x00,0x00};
	
	if(mode)	buff[0] = 0x21;
	else		buff[0] = 0x20;
	
	buff[5] = (u8)(frq>>8)&0xFF;buff[6] = (u8)(frq&0xFF);
	I2C2_WriteData(TEF665x_SlaveAddr,buff,7);
	
	delay_ms(200);
	//读取信号灵敏度信息
	
	buff[1] = 0x81;buff[2] = 0x01;
	I2C2_WriteData(TEF665x_SlaveAddr,buff,3);
	I2C2_ReadData(TEF665x_SlaveAddr+1,buff,31);
	
	if(mode)	return TEF6686_AMSeek(buff,frq);
	else      	return TEF6686_FMSeek(buff,frq);
}
//


/**
*-函数原型：u8 TEF6686_TuneTo(void)
*-功能：    跳转到指定模式，指定频率
*-输入：    mode： 模式：AM/FM
*-返回：    质量，0：无信号，1：好台 
**/
u8 TEF6686_TuneTo(void)
{
	if(RADIO_Inf.MODE)	return TEF6686_FRQSet(1,RADIO_Inf.AM_FRQ);
	else				return TEF6686_FRQSet(0,RADIO_Inf.FM_FRQ);
	
}
//

#include "string.h"
// RDS version:0x7 0x2 0x2 0x2 0x5 0x0 0x0 0x0 0x0 0x0 0x0 0x5d 0x0 0x1 0x0 0x0 0x0 0x0 0xff 0xff 0x0 0x0 0x0 0x0 0x0 0x1 0x0 0x0 0x0 0x0
//	   version:0x7 0x5 0x2 0x2 0x5 0x0 0x0 0x0 0x0 0x0 0x0 0x5d 0x0 0x1 0x0 0x0 0x0 0x0 0xff 0xff 0x0 0x0 0x0 0x0 0x0 0x1 0x0 0x0 0x0 0x0
void get_version(void)
{
	u8 buff[31]={0x40,0x82,0x01,0x00,0x00};
	
	u16 radio_v;
	u8 radio_index;
	
	I2C2_WriteData(TEF665x_SlaveAddr,buff,3);
	I2C2_ReadData(TEF665x_SlaveAddr+1,buff,31);

	if(buff[0] == 7)
	{
		switch(buff[1])
		{
			case 7:radio_index = 0;break;
			case 6:radio_index = 3;break;
			case 5:radio_index = 1;break;
			case 4:radio_index = 2;break;
			case 2:radio_index = 6;break;
		}
		sprintf((char*)radio_version,"TEA685%dA_HW%d.%d_SW%d.%d",radio_index,buff[2],buff[3],buff[4],buff[5]);
	}
	else if(buff[0] == 9)
	{
		switch(buff[1])
		{
			case 14:radio_index = 6;break;
			case 1:radio_index = 7;break;
			case 9:radio_index = 8;break;
			case 3:radio_index = 9;break;
		}
		sprintf((char*)radio_version,"TEF668%d_HW%d.%d_SW%d.%d",radio_index,buff[2],buff[3],buff[4],buff[5]);
	}
	else
	{
		sprintf((char*)radio_version,"Unknown Radio");
	}
	
	
//	printf("radio_version:%s ",radio_version);
//	printf("len = %d",strlen(radio_version));

//	printf("\r\n");
}






//u8 TEF6686_TuneTo(void)
//{
//	u8 buff[31]={0x20,0x01,0x01,0x00,0x01,0x00,0x00};
//	u8 sta = 0;
//	u16 Hz = 0;
//	
//	if(RADIO_Inf.MODE)	Hz = RADIO_Inf.AM_FRQ,buff[0] = 0x21;
//	else				Hz = RADIO_Inf.FM_FRQ*10,buff[0] = 0x20;
//	
//	buff[5] = Hz>>8;buff[6] = Hz&0xff;
//	if(RADIO_Inf.MODE)buff[0] = 0x21;
//	I2C2_WriteData(TEF665x_SlaveAddr,buff,7);
//	delay_ms(150);
//	//读取信号灵敏度信息
//	
//	buff[1] = 0x81;buff[2] = 0x01;
//	I2C2_WriteData(TEF665x_SlaveAddr,buff,3);
//	I2C2_ReadData(TEF665x_SlaveAddr+1,buff,31);
//	
//	if(RADIO_Inf.MODE)	{sta = TEF6686_AMSeek(buff,Hz);}
//	else      {sta = TEF6686_FMSeek(buff,Hz);}
//	return sta;
//}
//END



#include "FreeRTOS.h"
#include "task.h"
extern u8 CMD_8301(u8 Sta);
extern u8 CMD_8621(u8 mode,u32 ss);//停台灵敏度
extern u8 CMD_8622(u8 mode,u32 stnr);//信噪比
extern u8 CMD_8623(u8 mode,u32 fres);//频率响应

u8 TEF6686_Scan(u8 mode,u8 all);
static u8 Cnt_Err = 0;
static u8 Cnt_Time = 0;
static u8 radio_mute = 0;
u8 Radio_Process(void)
{
	u16 Temp = 0;

	if(RADIO_Inf.WORK_STA == 3)//收音机启动阶段
	{
		if(!TEF6686_Start())//启动成功？
		{
			Cnt_Err = 0;
			RADIO_Inf.WORK_STA = 1;
			FAULT_Inf.RADIO = 0;
			RDS_Clean();
		}
		else//不成功
		{
			Cnt_Err++;
			if(Cnt_Err>=10)//尝试次数过多，标记故障吧
			{
				RADIO_Inf.WORK_STA = 2;
				FAULT_Inf.RADIO = 1;
				Cnt_Err = 0;
			}
			return 0;
		}
		return 0;
	}

	if((RADIO_Inf.WORK_STA != 1)&&(RADIO_Inf.WORK_STA != 2))	//收音机还没启动
	{
		return 0;
	}

	//收音机故障检查
	if(Cnt_Time<50)Cnt_Time++;
	else
	{
		Cnt_Time = 0;
		Temp = TEF6886_GetSta(RADIO_Inf.MODE);
		get_version();
		if((0x100 == Temp)||(0 == Temp))	//总线故障为 0x100，意外复位为 0；
		{
			if(Cnt_Err<6) 
			{
				Cnt_Err++;		//故障计次
				if(Temp == 0)//意外复位，尝试重启收音机
				{
					TEF6686_Start();
					TEF6686_TuneTo();
				}
			}
			else 
			{
				Cnt_Err = 0;
				RADIO_Inf.WORK_STA = 2;		//标记收音机故障
				FAULT_Inf.RADIO = 1;
				//故障处理
				if(Temp == 0)//意外复位，尝试重启收音机
				{
					TEF6686_Start();
					TEF6686_TuneTo();
				}
				return 0;
			}
		}
		else 
		{
			Cnt_Err = 0;
			RADIO_Inf.WORK_STA = 1;		//收音机正常工作模式
			FAULT_Inf.RADIO = 0;
			
			//get version
//			get_version();
		}
	}
	
	if(RADIO_Inf.WORK_STA == 2)
	{
		RADIO_Inf.CMD = 0,CMD_8301(0);//收音机故障
		return 0;
	}
	
	if(RADIO_Inf.CMD == 0)
	{
		if(radio_mute == 1)
		{
			radio_mute = 0;
			TEF6686_Mute(0);
		}
#ifdef RDS_ENABLE
		RDS_Check();
#endif
		return 0;	//没有任务，返回
	}	
	else
	{
		if(radio_mute == 0)//非空闲禁音控制
		{
			radio_mute = 1;
			TEF6686_Mute(1);
		}
	}
	
#ifdef RDS_ENABLE
	switch(RADIO_Inf.CMD)
	{
		case 1:
		case 2:
		case 7:
		case 8:RDS_Clean();break;
		default:break;
	}
#endif
	
	switch(RADIO_Inf.CMD)
	{
		case 0:break;
		case 1:RADIO_Inf.SIGQUA = TEF6686_TuneTo();RADIO_Inf.CMD = 0;CMD_8301(0);break;//模式切换
		case 2:RADIO_Inf.SIGQUA = TEF6686_TuneTo();RADIO_Inf.CMD = 0;CMD_8301(0);break;//跳转至指定频率		
		case 3:RADIO_Inf.CMD = 0;CMD_8301(0);break;//查询频率
		case 4:TEF6686_Scan(1,0);break;//向上搜台
		case 5:TEF6686_Scan(0,0);break;//向下搜台
		case 6:RADIO_Inf.CMD = 0;CMD_8301(5);break;//打断搜台
		case 7:TEF6686_Scan(1,1);break;//全局搜台
		case 8:
			RADIO_Inf.SIGQUA = TEF6686_TuneTo();
			if(RADIO_Inf.MODE)CMD_8621(RADIO_Inf.MODE,RADIO_Inf.AM_LEVEL/10);
			else
			{
				CMD_8621(RADIO_Inf.MODE,RADIO_Inf.FM_LEVEL/10);
				CMD_8622(RADIO_Inf.MODE,RADIO_Inf.FM_STNR/10);
				CMD_8623(RADIO_Inf.MODE,RADIO_Inf.FM_FRES/10);
			}
			RADIO_Inf.CMD = 0;
		break;
		default:break;
	}
	return 0;
}

u8 TEF6686_Scan(u8 mode,u8 all)
{
	static u8 cnt = 0; 
	if(RADIO_Inf.MODE)	
	{
		cnt++;
		RADIO_Inf.AM_FRQ = mode?RADIO_Inf.AM_FRQ+9:RADIO_Inf.AM_FRQ-9;
		if(RADIO_Inf.AM_FRQ>Frequency[3])
		{
			if((RADIO_Inf.AM_FRQ-Frequency[3])==9)RADIO_Inf.AM_FRQ = Frequency[2];
			else RADIO_Inf.AM_FRQ = (RADIO_Inf.AM_FRQ-Frequency[3]+Frequency[2]);
		}
		if(RADIO_Inf.AM_FRQ<Frequency[2])
		{
			if((Frequency[2]-RADIO_Inf.AM_FRQ)==9)RADIO_Inf.AM_FRQ = Frequency[3];
			else RADIO_Inf.AM_FRQ = (Frequency[3]-(Frequency[2]-RADIO_Inf.AM_FRQ));
		}
	}
	else 				
	{
		cnt++;
		
#ifdef RDS_ENABLE
		RADIO_Inf.FM_FRQ = mode?(RADIO_Inf.FM_FRQ+5):(RADIO_Inf.FM_FRQ-5);
		if(RADIO_Inf.FM_FRQ>Frequency[1])RADIO_Inf.FM_FRQ = Frequency[0];
		if(RADIO_Inf.FM_FRQ<Frequency[0])RADIO_Inf.FM_FRQ = Frequency[1];
#else	
		RADIO_Inf.FM_FRQ = mode?(RADIO_Inf.FM_FRQ+10):(RADIO_Inf.FM_FRQ-10);
		if(RADIO_Inf.FM_FRQ>Frequency[1])RADIO_Inf.FM_FRQ = Frequency[0];
		if(RADIO_Inf.FM_FRQ<Frequency[0])RADIO_Inf.FM_FRQ = Frequency[1];
#endif		
		
	}
	RADIO_Inf.SIGQUA = TEF6686_TuneTo();
#ifdef RDS_ENABLE
//	if(RADIO_Inf.SIGQUA == 0)RADIO_Inf.SIGQUA = TEF6686_TuneTo();
#endif
	if(RADIO_Inf.SIGQUA)//有信号，
	{
		if(all == 0)//搜到停止
		{
			RADIO_Inf.CMD = 0;
			cnt=0;
			CMD_8301(0);
			return 0;	
		}
		else CMD_8301(3);
	}
	
	//按步进距离报告位置
	if(RADIO_Inf.MODE)
	{
		if(RADIO_Inf.AM_FRQ == RADIO_Inf.Last_FRQ)//到头了
		{
			if(all==0)	CMD_8301(0);
			else		CMD_8301(5);
//			else		CMD_8301(2);
			cnt=0;
			RADIO_Inf.CMD = 0;
		}
		else //按步进距离报告位置
		{
			if(cnt >= 10)
			{
				cnt=0;
				if(all==1)	CMD_8301(4);
				else		CMD_8301(0);
//				CMD_8301(1);
			}
		}
	}
	else
	{
		if(RADIO_Inf.FM_FRQ == RADIO_Inf.Last_FRQ)//到头了
		{
			if(all==0)	CMD_8301(0);
			else		CMD_8301(5);
//			else		CMD_8301(2);
			cnt=0;
			RADIO_Inf.CMD = 0;
		}
		else //按步进距离报告位置
		{
			if(cnt >= 10)
			{
				cnt=0;
				if(all==1)	CMD_8301(4);
				else		CMD_8301(0);
//				else		CMD_8301(1);
			}
		}
	}
	return 0;
}

//paixu
void My_Sort(u16 (*buf)[32],u8 len)
{
	u16 temp;
	u8 flag = 1;
	
	for(u8 i=len;(i>0)&&flag == 1;i--)
	{
		flag = 0;
		for(u8 j=1;j<i;j++)
		{
			if(buf[1][j-1]<buf[1][j])
			{
				temp = buf[0][j-1];
				buf[0][j-1] = buf[0][j];
				buf[0][j] = temp;
				
				temp = buf[1][j-1];
				buf[1][j-1] = buf[1][j];
				buf[1][j] = temp;

				flag = 1;
			}		
		}
	}
}





