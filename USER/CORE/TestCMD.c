#include "TestCMD.h"
#include "usart.h"

#include "app.h"
#include "hardware.h"
#include "information.h"




uint8_t Test_mode  = 0;//0未进入测试模式，1进入测试模式
uint8_t Test_KEYmode  = 0;//0未进入测试模式，1进入测试模式


//接收相关
#define TestSYNC1		0x3A
#define TestSYNC2		0xA3
#define TestCMDBUFLEN	512
uint8_t Test_CMDBuf[TestCMDBUFLEN]={0};
uint8_t Test_CMDLen = 0;


//缓存操作
#define Test_CacheLen		256
uint16_t Test_WriteP = 0;
uint16_t Test_ReadP = 0;
uint8_t Test_CacheBuff[Test_CacheLen]={0};
void Test_wrie_buf(uint8_t dat)
{
	Test_CacheBuff[Test_WriteP] = dat;
	Test_WriteP++;
	if(Test_WriteP >= Test_CacheLen)Test_WriteP = 0;
	if(Test_WriteP == Test_ReadP)Test_ReadP++;
	if(Test_ReadP >= Test_CacheLen)Test_ReadP = 0;
}

uint8_t Test_read_buf(uint8_t *dat)
{
	if(Test_WriteP == Test_ReadP)return 0;
	*dat = Test_CacheBuff[Test_ReadP];
	Test_ReadP++;
	if(Test_ReadP >= Test_CacheLen)Test_ReadP = 0;
	return 1;
}

void Test_CacheClaen(void)
{
	Test_ReadP = Test_WriteP;
}
//end

//函数接口实现
void Test_Sendbyte(uint8_t byte)
{
	//发送实现
	while((USART1->SR&0X40)==0);//等待上一次串口数据发送完成  
	USART1->DR = (u8) byte;	//写DR,串口1将发送数据
}
//接收调用
void Test_Receivebyte(uint8_t byte)
{
	Test_wrie_buf(byte);
}
//end


//发送相关功能函数

_SendCache SendCache;


//uint8_t Test_TxCmdbuf[64]={0};
uint8_t Test_CmdSend(uint8_t msglen,u8 *dat)
{
	uint16_t sun = 0;
	uint8_t  checksun = 0;
	dat[0] = 0x3A;
	dat[1] = 0xA3;
	dat[2] = 0x00;
	dat[3] = msglen - 6;
	for(uint8_t i=2;i<(msglen-2);i++)	sun += dat[i];
	checksun = (uint8_t)(~sun + 1);
	dat[msglen-2] = 0;	
	dat[msglen-1] = (uint8_t)checksun;

	for(uint8_t i=0;i<msglen;i++)Test_Sendbyte(dat[i]);
	return 0;
}


void Write_sendcache(_cache_frame dat)
{
	SendCache.dat[SendCache.w_p] = dat;
	SendCache.w_p++;
	if(SendCache.w_p>=16)SendCache.w_p=0;
	if(SendCache.w_p == SendCache.r_p)SendCache.r_p++;
	if(SendCache.r_p>=16)SendCache.r_p = 0;
//	printf("write\r\n");
}
uint8_t Read_sendcache(_cache_frame *dat)
{
	if(SendCache.r_p == SendCache.w_p)return 0;
	*dat = SendCache.dat[SendCache.r_p];
	SendCache.r_p++;
	if(SendCache.r_p>=16)SendCache.r_p = 0;
//	printf("read\r\n");
	return 1;
}


void Test_respond(uint8_t len,uint8_t *data)
{
	_cache_frame temp;
	temp.len = len + 6;//加六
	for(u8 i=0;i<len;i++)
	{
		temp.dat[i+4] = data[i];
	} 
	Write_sendcache(temp);
}

uint8_t Test_sendProcess(void)
{
	_cache_frame temp;
	if(Read_sendcache(&temp))
	{
		Test_CmdSend(temp.len,temp.dat);
	}
	return 0;
}

extern u8 CMD_8670(u8 cmdlen,u8 *cd);
void Test_pc2mpu(void)
{
	u8 tempbuf[128];
	for(u8 i=0;i<(Test_CMDLen-6);i++)
		tempbuf[i] = Test_CMDBuf[i+4];
	CMD_8670(Test_CMDLen - 6,tempbuf);
}



uint8_t Test_CmdGet(void)
{
	static uint8_t syncsta = 0;
	static uint8_t rstatus = 0;
	static uint16_t msg_len = 0;
	
	uint8_t dattemp = 0;
	if(Test_read_buf(&dattemp))
	{
		if(1 == rstatus)
		{
			Test_CMDBuf[msg_len++] = dattemp;
			if(4 == msg_len) Test_CMDLen = dattemp + 6;
			if(msg_len >= TestCMDBUFLEN)
			{
				rstatus = 0;
				msg_len = 0;
				return 0;
			}
			if(msg_len == Test_CMDLen)
			{
				rstatus = 0;
				msg_len = 0;
				return 1;	
			}
		}
		else
		{
			if((TestSYNC2 == dattemp)&&(1 == syncsta))
			{
				rstatus = 1;
				msg_len = 2;
				Test_CMDBuf[0] = TestSYNC1;
				Test_CMDBuf[1] = TestSYNC2;
			}
			syncsta = 0;
			if(TestSYNC1 == dattemp)syncsta = 1;
		}
	}
	return 0;
}

uint8_t Test_SunCheck(void)
{
	uint16_t sun = 0;
	uint8_t  checksun = 0;
	for(uint8_t i=2;i<(Test_CMDLen-2);i++)	sun += Test_CMDBuf[i];
	checksun = (uint8_t)(~sun + 1);	
	
//	printf("checksun = 0x%x,sun = 0x%x\r\n",checksun,Test_CMDBuf[Test_CMDLen-1]);
	
	if(checksun == Test_CMDBuf[Test_CMDLen-1]) return 1;
	else return 0;
}


uint8_t Test_CMD00(uint8_t err);
uint8_t Test_CMD01(uint8_t err);
uint8_t Test_CMD02(uint8_t err);
uint8_t Test_CMD03(uint8_t err);
uint8_t Test_CMD04(uint8_t err);
uint8_t Test_CMD07(uint8_t err);
uint8_t Test_CMD08(uint8_t err);
uint8_t Test_CMD09(uint8_t err);
uint8_t Test_CMD0A(uint8_t err);
uint8_t Test_CMD0B(uint8_t err);
uint8_t Test_CMD0C(uint8_t err);
uint8_t Test_CMD0D(uint8_t err);
uint8_t Test_CMDFC(uint8_t err);

void Test_CMDProcess(void)
{
	uint8_t CMD_err = 0xFF;
	
	Test_sendProcess();
	
	if(Test_CmdGet())
	{
		if(Test_SunCheck())
		{
			
			if(Test_mode == 0)
			{
				if((Test_CMDBuf[4] != 0xFC))
				{
					if(Test_CMDBuf[4] !=0)return;
					if((Test_CMDBuf[5] !=0xFF)&&(Test_CMDBuf[5] !=0xFE))return;
				}
			}
			
//			printf("yes\r\n");
			
			switch(Test_CMDBuf[4])
			{
				case 0x00:CMD_err = Test_CMD00(0x00);break;//系统操作	4
				case 0x01:CMD_err = Test_CMD01(0x01);break;//EQ操作		1
				case 0x02:CMD_err = Test_CMD02(0x02);break;//收音功能	6
				case 0x03:CMD_err = Test_CMD03(0x03);break;//曲目控制
				case 0x04:CMD_err = Test_CMD04(0x04);break;//蓝牙功能
				case 0x05:break;//
				case 0x06:break;//
				case 0x07:CMD_err = Test_CMD07(0x07);break;//产品方控按键	11
				case 0x08:CMD_err = Test_CMD08(0x08);break;//产品信息写入	2
				case 0x09:CMD_err = Test_CMD09(0x09);break;//版本信息查询	1
				case 0x0A:CMD_err = Test_CMD0A(0x0A);break;//触摸功能
				case 0x0B:CMD_err = Test_CMD0B(0x0B);break;//WIFI功能			
				case 0x0C:CMD_err = Test_CMD0C(0x0C);break;//麦克风功能
				case 0x0D:CMD_err = Test_CMD0D(0x0D);break;//电源策略
				case 0xFC:CMD_err = Test_CMDFC(0xFF);break;				
				default:break;
			}
			if(CMD_err != 0xFF)
			{
				
			}
		}
		else
		{
//			printf("sun err\r\n");
		}
	}

}

uint8_t Test_CMD00_source(void)
{
	switch(Test_CMDBuf[6])
	{
		case 0x00:;break;//收音机
		case 0x01:;break;//USB音乐
		case 0x02:;break;//USB视频
		case 0x03:;break;//USB图片
		case 0x04:;break;//蓝牙
		case 0x05:;break;//AUX
		case 0x06:;break;//NAVI
		case 0x07:;break;//Carlife
		case 0x08:;break;//保留
		case 0x09:;break;//保留
		case 0x0A:;break;//保留
		case 0x0B:;break;//保留
		
		default:break;
	}
	return 0;
}



uint8_t Test_CMD00(uint8_t err)
{
	switch(Test_CMDBuf[5])
	{
		case 0x00:Test_pc2mpu();Test_CMD00_source();break;//源操作
		case 0x01:Test_pc2mpu();break;//查询当前源
		case 0x02:;break;//保留
		case 0x03:MCU_Awak = 1;break;//自动开机
		case 0x04:Sys_Soft_Reset();break;//软件复位
		case 0x05:if(Test_CMDBuf[6] == 1)CAR_Inf.ReverseSta = 1;break;//视频开关
		case 0x06:Test_pc2mpu();break;//亮度设定
		case 0x07:Test_pc2mpu();break;//亮度查询
		case 0x08:MCU_Awak = 0;break;//快速ACC OFF
		case 0x09:Test_pc2mpu();break;//恢复出厂设置
		case 0x0A:Test_pc2mpu();break;//对比度设定
		case 0x0B:Test_pc2mpu();break;//对比度查询
		case 0x0C:Test_pc2mpu();break;//饱和度设定
		case 0x0D:Test_pc2mpu();break;//饱和度查询
		case 0x0E:Test_pc2mpu();break;//查询语音激活章台
		case 0xFE:Test_pc2mpu();break;//查询工厂模式状态
		case 0xFF:Test_pc2mpu();Test_mode = Test_CMDBuf[6];break;//进入/退出工厂模式

		default:Test_pc2mpu();break;
	}
	return 0;
}


uint8_t Test_CMD01(uint8_t err)
{
	uint8_t temp[16];
	switch(Test_CMDBuf[5])
	{
//		case 0x00:Test_pc2mpu();break;//音量设置
		case 0x01B:temp[0] = 0x81;temp[1] = 0x1B;temp[2] = SYS_Inf.MUTE_STA;Test_respond(3,temp);break;//MUTE查询
//		case 0x02:Test_pc2mpu();break;//BASS调整
//		case 0x03:Test_pc2mpu();break;//BASS查询
//		case 0x04:Test_pc2mpu();break;//MID调整
//		case 0x05:Test_pc2mpu();break;//MID查询
//		case 0x06:Test_pc2mpu();break;//TREB调整
//		case 0x07:Test_pc2mpu();break;//TREB查询
//		case 0x08:Test_pc2mpu();break;//关闭/开启LOUD功能
//		case 0x09:Test_pc2mpu();break;//关闭/开启MUTE功能
//		case 0x0A:;break;//关闭/开启Subwoofer功能
//		case 0x0B:;break;//设置Fade Balance
//		case 0x0C:;break;//EQ恢复初始值 OK
//		case 0x0D:;break;//查询Fade Balance
//		case 0x0E:;break;//设置Sub woofer filter
//		case 0x0F:;break;//设置Sub gain
//		case 0xFF:;break;//EQ相关值查询
//		case 0x10:;break;//设置EQ类型

		default:Test_pc2mpu();break;
	}
	return 0;
}

#include "radio.h"
void Test_sendfrqcover(void)
{
	uint8_t temp[8]={0x82,0x0A,0x00,0x00};
	uint16_t frq_l = 0;
	uint16_t frq_h = 0;
	
	if(RADIO_Inf.MODE == 0)
	{
		frq_l = Frequency[0];
		frq_h = Frequency[1];
	}
	else
	{
		frq_l = Frequency[2];
		frq_h = Frequency[3];
	}
	temp[2] = (uint8_t)((frq_l>>8)&0xFF);
	temp[3] = (uint8_t)((frq_l>>0)&0xFF);
	temp[4] = (uint8_t)((frq_h>>8)&0xFF);
	temp[5] = (uint8_t)((frq_h>>0)&0xFF);
	
	Test_respond(6,temp);
}

void Test_inquirtfrq(void)
{
	uint8_t temp[8]={0x82,0x01,0x00,0x00};
	uint16_t frq = 0;
	
	if(RADIO_Inf.MODE == 0)
	{
		frq = RADIO_Inf.FM_FRQ;
	}
	else
	{
		frq = RADIO_Inf.AM_FRQ;
	}
	temp[2] = (uint8_t)((frq>>8)&0xFF);
	temp[3] = (uint8_t)((frq>>0)&0xFF);
	
	Test_respond(4,temp);
}

//1:频率指定，2：切换波段，3：搜索电台
void Test_RadioCtrl(uint8_t cmd,uint16_t dat)
{
	if(RADIO_Inf.CMD != 0)//忙判断
	{
		if((cmd == 3)&&(dat == 2))
			RADIO_Inf.CMD = 6;//停止
		return;
	}
	if(cmd == 1)
	{
		if((dat>=(Frequency[0]))&&(dat<=(Frequency[1])))
		{
			RADIO_Inf.MODE = 0;
			RADIO_Inf.FM_FRQ = dat;
			RADIO_Inf.CMD = 2;
			return;
		}
		if((dat>=Frequency[2])&&(dat<=Frequency[3]))
		{
			RADIO_Inf.MODE = 1;
			RADIO_Inf.AM_FRQ = dat;
			RADIO_Inf.CMD = 2;
			return;
		}	
	}
	if(cmd == 2)
	{
		if(dat == 0)RADIO_Inf.MODE = 0;
		else if(dat == 1)RADIO_Inf.MODE = 1;
		RADIO_Inf.CMD = 1;
		return;
	}
	if(cmd == 3)
	{
		if(dat == 0)RADIO_Inf.CMD = 4;//向上搜
		else if(dat == 1)RADIO_Inf.CMD = 5;//向下搜
		else if(dat == 3){RADIO_Inf.Last_FRQ = RADIO_Inf.MODE?RADIO_Inf.AM_FRQ:RADIO_Inf.FM_FRQ;RADIO_Inf.CMD = 7;}//全局自动搜
		return;
	}
}




uint8_t Test_CMD02(uint8_t err)
{
	uint8_t tempbuf[5];
	switch(Test_CMDBuf[5])
	{
		case 0x00:Test_RadioCtrl(1,(uint16_t)((Test_CMDBuf[6]<<8)+Test_CMDBuf[7]));break;//频率指定
		case 0x01:Test_inquirtfrq();;break;//频率查询
		case 0x02:Test_RadioCtrl(2,(uint16_t)Test_CMDBuf[6]);break;//切换波段
		case 0x03:tempbuf[0]=0x82;tempbuf[1]=0x03;tempbuf[2]=RADIO_Inf.MODE;Test_respond(3,tempbuf);break;//查询波段
		case 0x04:Test_RadioCtrl(3,(uint16_t)Test_CMDBuf[6]);break;//搜索电台
		case 0x05:;break;//
		case 0x06:;break;//
		case 0x07:;break;//
		case 0x08:;break;//保留
		case 0x09:;break;//保留
		case 0x0A:Test_sendfrqcover();break;//查询频率范围
		case 0x0B:;break;//保留
		case 0x0C:;break;//保留
		case 0x0D:tempbuf[0]=0x82;tempbuf[1]=0x0D;tempbuf[2]=RADIO_Inf.FA_Stereo;Test_respond(3,tempbuf);break;//查询Stereo状态

		default:break;
	}
	return 0;
}

uint8_t Test_CMD03(uint8_t err)
{
	switch(Test_CMDBuf[5])
	{
		case 0x00:Test_pc2mpu();break;//跳转指定
		case 0x01:Test_pc2mpu();;break;//播放/暂停
		case 0x02:Test_pc2mpu();break;//
		case 0x03:Test_pc2mpu();;break;//查询曲号
		case 0x04:Test_pc2mpu();break;//
		case 0x05:Test_pc2mpu();break;//

		default:Test_pc2mpu();break;
	}
	return 0;
}


uint8_t Test_CMD04(uint8_t err)
{
	switch(Test_CMDBuf[5])
	{
		case 0x00:	//break;//关闭/打开蓝牙
		case 0x01:	//break;//查询蓝牙开关状态
		case 0x02:	//break;//
		case 0x03:	//break;//进入蓝牙音乐
		case 0x04:	//break;//蓝牙音乐播放/暂停
		case 0x05:	//break;//
		case 0x06:	//break;//蓝牙连接状态查询
		case 0x07:	//break;//查询蓝牙MAC地址
		case 0x08:	//break;//蓝牙通话切换
		case 0x09:Test_pc2mpu();break;//清除蓝牙连接记录
		case 0x0A:Test_pc2mpu();break;//
		case 0x0B:Test_pc2mpu();break;//
		case 0x0C:Test_pc2mpu();break;//
		default:Test_pc2mpu();break;
	}
	return 0;
}


void Test_keysend(uint8_t key,uint8_t sta)
{
	uint8_t keyindex[10]={0x05,0x04,0x08,0x07,0x03,0x06,0x09,0x0B,0x0C,0x0D};
	uint8_t temp[4]={0x87,0x00,0x00,0x00};
	
	if(Test_KEYmode == 0)return;
	if(sta == 0)return;
	
	temp[1] = keyindex[key];
	Test_respond(2,temp);
}

uint8_t Test_CMD07(uint8_t err)
{
	uint8_t temp[4] = {0x87,0x01,0x00,0x00};
	switch(Test_CMDBuf[5])
	{
		case 0x00:Test_KEYmode = Test_CMDBuf[6];break;//进入方控按键测试模式
		case 0x01:temp[2] = Test_KEYmode;Test_respond(3,temp);break;//查询按键测试模式状态
		case 0x02:;break;//
		case 0x03:;break;//

		default:break;
	}
	return 0;
}


uint8_t EEPROM_Check(uint8_t add,uint8_t dat)
{
	if(AT24CXX_ReadOneByte(add)!=dat)//错误
	{
		if(AT24CXX_ReadOneByte(add)!=dat)return 0;//再试一次，也错
		else return 1;
	}
	else return 1;	
	
}


uint8_t Test_CMD08(uint8_t err)
{
	static uint8_t eep_buf[4]={0x88,0x05,0x00,0x00};
	
	switch(Test_CMDBuf[5])
	{
//		case 0x00:;break;//产品条码写入
//		case 0x01:;break;//产品条码读取
//		case 0x02:;break;//UUID写入
		case 0x03:Test_pc2mpu();break;//UUID读取
		case 0x04:eep_buf[2] = EEPROM_Check(255,EEPROM_FLAG);;break;//EEPROM写入
		case 0x05:Test_respond(3,eep_buf);;break;//EEPROM读取
		case 0x06:;break;//
		case 0x07:;break;//

		default:Test_pc2mpu();break;
	}
	return 0;
}


uint8_t Test_CMD09(uint8_t err)
{
	uint8_t temp[64];
	
	temp[0] = 0x89;
	temp[1] = 0x00;
	for(u8 i=0;i<Version_SIZE;i++)
		temp[2+i] = (u8)(*(Version+i));
	
	switch(Test_CMDBuf[5])
	{
		case 0x00:Test_respond(Version_SIZE+2,temp);break;//MCU版本号查询
		case 0x01:Test_pc2mpu();break;//OS版本号查询
		case 0x02:Test_pc2mpu();break;//固件版本号查询
		case 0x03:;break;//
		case 0x04:;break;//蓝牙版本查询
		case 0x05:;break;//机型版本查询
		case 0x06:;break;//
		case 0x07:;break;//

		default:Test_pc2mpu();break;
	}
	return 0;
}


uint8_t Test_CMD0A(uint8_t err)
{
	switch(Test_CMDBuf[5])
	{
		case 0x00:Test_pc2mpu();break;//进入触摸测试模式
		case 0x01:Test_pc2mpu();break;//查询触摸测试模式
		case 0x02:Test_pc2mpu();break;//查询触摸测试结果
		case 0x03:;break;//
		case 0x04:;break;//

		default:Test_pc2mpu();break;
	}
	return 0;
}

uint8_t Test_CMD0B(uint8_t err)
{
	switch(Test_CMDBuf[5])
	{
		case 0x00:Test_pc2mpu();break;//关闭/打开WIFI
		case 0x01:Test_pc2mpu();break;//查询WIFI开关状态
		case 0x02:Test_pc2mpu();break;//WIFI连接
		case 0x03:Test_pc2mpu();break;//查询WIFI连接状态
		case 0x04:Test_pc2mpu();break;//WIFI信号强度查询
		case 0x05:Test_pc2mpu();break;//WIFI PING网
		case 0x06:;break;//
		case 0x07:;break;//

		default:Test_pc2mpu();break;
	}
	return 0;
}


uint8_t Test_CMD0C(uint8_t err)
{
	switch(Test_CMDBuf[5])
	{
		case 0x00:Test_pc2mpu();break;//进入麦克风测试模式
		case 0x01:Test_pc2mpu();break;//查询麦克风测试结果
		case 0x02:;break;//
		case 0x03:;break;//

		default:Test_pc2mpu();break;
	}
	return 0;
}


void power_check(u8 mode)
{
	u8 temp[5] = {0x8D,0x00,0x00,0x00,0x00};
	u8 result = 0;
	
	switch(mode)
	{
		case 0:if(SYS_Inf.Batter_sta == 2) result=1;break;//12V以下
		case 1:if(SYS_Inf.Batter_sta == 1) result=1;break;//12-13
		case 2:if(SYS_Inf.Batter_sta == 0) result=1;break;//13以上
		
		default: result = 0;break;
	}
	temp[1] = mode;
	temp[2] = result;
	Test_respond(3,temp);
}



uint8_t Test_CMD0D(uint8_t err)
{
	switch(Test_CMDBuf[5])
	{
		case 0x00:power_check(0);break;//<12V
		case 0x01:power_check(1);break;//12-13V
		case 0x02:power_check(2);break;//>13V

		default:Test_pc2mpu();break;
	}
	return 0;
}



extern void DEBUG_Printf(void);
uint8_t Test_CMDFC(uint8_t err)
{
	
	switch(Test_CMDBuf[5])
	{
		case 0x00:DEBUG_Printf();;break;//
		case 0x01:;break;//
		case 0x02:;break;//
		case 0x03:;break;//

		default:Test_pc2mpu();break;
	}
	
	return 0;
}


