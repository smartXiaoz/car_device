#include "24cxx.h" 
#include "delay.h" 										 
#include "usart.h" 



u8 TEST_DID_F190[17],TEST_DID_F190_Enable;
u8 TEST_DID_F1A3[14],TEST_DID_F1A3_Enable;
u8 TEST_DID_3700[4],TEST_DID_3700_Enable;
/*******************************************************************************
*  Call back functions for the VIN configuration write                  *
********************************************************************************/
void CAL_F190_CONF_WRITE(void) 
{
    TEST_DID_F190_Enable=1;
}
/*******************************************************************************
*  Call back functions for the CalibrationDataIdentifier configuration write                  *
********************************************************************************/
void CAL_F1A3_CONF_WRITE(void) 
{
    TEST_DID_F1A3_Enable=1;
}
/*******************************************************************************
*  Call back functions for the GeneralConfiguration configuration write                  *
********************************************************************************/
void CAL_3700_CONF_WRITE(void) 
{
    TEST_DID_3700_Enable=1;
}


void Data_Save(void)
{
	u8 i = 0;
	
	if(TEST_DID_F190_Enable==1)
	{
		for(i=0;i<17;i++)AT24CXX_WriteOneByte(EEPROM_ADD+i,(u8)(TEST_DID_F190[i]));
		TEST_DID_F190_Enable = 0;
	}
	if(TEST_DID_F1A3_Enable==1)
	{
		for(i=0;i<14;i++)AT24CXX_WriteOneByte(EEPROM_ADD+17+i,(u8)(TEST_DID_F1A3[i]));
		TEST_DID_F1A3_Enable = 0;
	}
	if(TEST_DID_3700_Enable==1)
	{
		for(i=0;i<4;i++)AT24CXX_WriteOneByte(EEPROM_ADD+17+14+i,(u8)(TEST_DID_3700[i]));
		TEST_DID_3700_Enable = 0;
	}
}

void Data_read(void)
{
	u8 i = 0;
	for(i=0;i<17;i++)TEST_DID_F190[i] = AT24CXX_ReadOneByte(EEPROM_ADD+i);
	for(i=0;i<14;i++)TEST_DID_F1A3[i] = AT24CXX_ReadOneByte(EEPROM_ADD+17+i);
	for(i=0;i<4;i++)TEST_DID_3700[i] = AT24CXX_ReadOneByte(EEPROM_ADD+17+14+i);
}


//初始化IIC接口
void AT24CXX_Init(void)
{
	IIC1_Init();
}
//在AT24CXX指定地址读出一个数据
//ReadAddr:开始读数的地址  
//返回值  :读到的数据
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{			  
	u8 temp=0;		  	    																 
    IIC1_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC1_Send_Byte(EE_ADDR);	   //发送写命令
		IIC1_Wait_Ack();
		IIC1_Send_Byte(ReadAddr>>8);//发送高地址	    
	}else IIC1_Send_Byte(EE_ADDR+((ReadAddr/256)<<1));   //发送器件地址0XA0,写数据， (ReadAddr/256)<<1)为页地址，视情况占用A0 - A2
	IIC1_Wait_Ack(); 
    IIC1_Send_Byte(ReadAddr%256);   //发送低地址 
	IIC1_Wait_Ack();	    
	IIC1_Start();  	 	   
	IIC1_Send_Byte(EE_ADDR|0x01);           //进入接收模式			   
	IIC1_Wait_Ack();	 
    temp=IIC1_Read_Byte(0);		   
    IIC1_Stop();//产生一个停止条件	    
	return temp;
}
//在AT24CXX指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
    IIC1_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC1_Send_Byte(EE_ADDR);	    //发送写命令
		IIC1_Wait_Ack();
		IIC1_Send_Byte(WriteAddr>>8);//发送高地址	  
	}else IIC1_Send_Byte(EE_ADDR+((WriteAddr/256)<<1));   //发送器件地址0XA0,写数据 	 
	IIC1_Wait_Ack();	   
    IIC1_Send_Byte(WriteAddr%256);   //发送低地址
	IIC1_Wait_Ack();
	IIC1_Send_Byte(DataToWrite);     //发送字节							   
	IIC1_Wait_Ack();
    IIC1_Stop();						//产生一个停止条件 
	delay_xms(100);	 				//EEPROM的写入速度比较慢，加入延迟
}

//返回1:检测失败
//返回0:检测成功
u8 AT24CXX_Check(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(255);//避免每次开机都写AT24CXX
	if(temp==EEPROM_FLAG)return 0;
	else//排除第一次初始化的情况
	{
		temp=AT24CXX_ReadOneByte(255);//再试一次
		if(temp==EEPROM_FLAG)return 0;
//		printf("it is new IC! \r\n");
		AT24CXX_WriteOneByte(255,EEPROM_FLAG);
		AT24CXX_WriteOneByte(EEPROM_ADD+17+14,0X04);
//		AT24CXX_WriteOneByte(EEPROM_ADD+17+14+1,0X01);
	    temp=AT24CXX_ReadOneByte(255);
		if(temp==EEPROM_FLAG)return 0;
	}
	return 1;
}

//在AT24CXX里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  
//在AT24CXX里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对24c02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}








