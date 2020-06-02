#include "i2c.h"
#include "delay.h"



//初始化IIC
void IIC1_Init(void)
{
 	RCC->APB2ENR|=1<<3;		//先使能外设IO PORTB时钟 							 
	GPIOB->CRL&=0XFFFFFF00;	//PB0/1 推挽输出
	GPIOB->CRL|=0X00000033;	   
//	GPIOB->ODR|=3<<0;     	//PB0,1 输出高
}
//产生IIC起始信号
void IIC1_Start(void)
{
	IIC1_SDA_OUT();     //sda线输出
	IIC1_SDA=1;	  	  
	IIC1_SCL=1;
	delay_us(4);
 	IIC1_SDA=0;//START:when CLK is high,DATA change form high to low  
	delay_us(4);
	IIC1_SCL=0;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC1_Stop(void)
{
	IIC1_SDA_OUT();//sda线输出
	IIC1_SCL=0;
	IIC1_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC1_SCL=1; 
	IIC1_SDA=1;//发送I2C总线结束信号
	delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC1_Wait_Ack(void)
{
	u8 ucErrTime=0;
	IIC1_SDA_IN();      //SDA设置为输入  
	IIC1_SDA=1;delay_us(1);	   
	IIC1_SCL=1;delay_us(1);	 
	while(IIC1_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC1_Stop();
			return 1;
		}
	}
	IIC1_SCL=0;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void IIC1_Ack(void)
{
	IIC1_SCL=0;
	IIC1_SDA_OUT();
	IIC1_SDA=0;
	delay_us(2);
	IIC1_SCL=1;
	delay_us(2);
	IIC1_SCL=0;
}
//不产生ACK应答		    
void IIC1_NAck(void)
{
	IIC1_SCL=0;
	IIC1_SDA_OUT();
	IIC1_SDA=1;
	delay_us(2);
	IIC1_SCL=1;
	delay_us(2);
	IIC1_SCL=0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC1_Send_Byte(u8 txd)
{                        
    u8 t;   
	IIC1_SDA_OUT(); 	    
    IIC1_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        IIC1_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		IIC1_SCL=1;
		delay_us(2); 
		IIC1_SCL=0;	
		delay_us(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC1_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	IIC1_SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC1_SCL=0; 
        delay_us(2);
		IIC1_SCL=1;
        receive<<=1;
        if(IIC1_READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC1_NAck();//发送nACK
    else
        IIC1_Ack(); //发送ACK   
    return receive;
}


//初始化IIC
void IIC2_Init(void)
{					     
 	RCC->APB2ENR|=1<<3;		//先使能外设IO PORTB时钟 							 
	GPIOB->CRL&=0X00FFFFFF;	//PB6/7 推挽输出
	GPIOB->CRL|=0X33000000;	   
	GPIOB->ODR|=3<<6;     	//PB6,7 输出高
}
//产生IIC起始信号
void IIC2_Start(void)
{
	IIC2_SDA_OUT();     //sda线输出
	IIC2_SDA=1;	  	  
	IIC2_SCL=1;
	delay_us(4);
 	IIC2_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC2_SCL=0;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC2_Stop(void)
{
	IIC2_SDA_OUT();//sda线输出
	IIC2_SCL=0;
	IIC2_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC2_SCL=1; 
	IIC2_SDA=1;//发送I2C总线结束信号
	delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC2_Wait_Ack(void)
{
	u8 ucErrTime=0;
	IIC2_SDA_IN();      //SDA设置为输入  
	IIC2_SDA=1;delay_us(1);	   
	IIC2_SCL=1;delay_us(1);	 
	while(IIC2_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC2_Stop();
			return 1;
		}
	}
	IIC2_SCL=0;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void IIC2_Ack(void)
{
	IIC2_SCL=0;
	IIC2_SDA_OUT();
	IIC2_SDA=0;
	delay_us(2);
	IIC2_SCL=1;
	delay_us(2);
	IIC2_SCL=0;
}
//不产生ACK应答		    
void IIC2_NAck(void)
{
	IIC2_SCL=0;
	IIC2_SDA_OUT();
	IIC2_SDA=1;
	delay_us(2);
	IIC2_SCL=1;
	delay_us(2);
	IIC2_SCL=0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC2_Send_Byte(u8 txd)
{                        
    u8 t;   
	IIC2_SDA_OUT(); 	    
    IIC2_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        IIC2_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		IIC2_SCL=1;
		delay_us(2); 
		IIC2_SCL=0;	
		delay_us(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC2_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	IIC2_SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC2_SCL=0; 
        delay_us(2);
		IIC2_SCL=1;
        receive<<=1;
        if(IIC2_READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC2_NAck();//发送nACK
    else
        IIC2_Ack(); //发送ACK   
    return receive;
}

int I2C2_WriteData(u8 ADD,u8 *buff,u32 len)
{
	u32 i = 0;
	IIC2_Start();
	IIC2_Send_Byte(ADD);	
	if(IIC2_Wait_Ack()) return ERROR;
	for(i=0;i<len;i++)
	{
		IIC2_Send_Byte(buff[i]);
		if(IIC2_Wait_Ack())
		return ERROR;
	}
	IIC2_Stop();
	return SUCCESS;
}

int I2C2_ReadData(u8 ADD,u8 *buff,u32 len)
{
	u32 i = 0;
	IIC2_Start();
	IIC2_Send_Byte(ADD);	
	if(IIC2_Wait_Ack())	return ERROR;
//	delay_ms(1);
	for(i=0;i<len;i++)	buff[i] = IIC2_Read_Byte(1);
	IIC2_Stop();	
	return SUCCESS;
}

















