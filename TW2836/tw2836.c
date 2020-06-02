#include "tw2836.h"
#include "tw2836_tbl.c"
#include "delay.h"
#include "usart.h"


void TW2836_IIC_Init(void)
{					     
 	RCC->APB2ENR|=1<<2;		//先使能外设IO PORTA时钟
	GPIOA->ODR|=3<<2;     	//PA2,3 输出高
	GPIOA->CRL&=0XFFFF00FF;	//PB2/3 推挽输出
	GPIOA->CRL|=0X00003300;	   
}
//产生IIC起始信号
void TW2836_IIC_Start(void)
{
	TW2836_SDA_OUT();     //sda线输出
	TW2836_IIC_SDA=1;	  	  
	TW2836_IIC_SCL=1;
	delay_us(4);
 	TW2836_IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	TW2836_IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void TW2836_IIC_Stop(void)
{
	TW2836_SDA_OUT();//sda线输出
	TW2836_IIC_SCL=0;
	TW2836_IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	TW2836_IIC_SCL=1; 
	TW2836_IIC_SDA=1;//发送I2C总线结束信号
	delay_us(4);							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 TW2836_IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	TW2836_SDA_IN();      //SDA设置为输入  
	TW2836_IIC_SDA=1;delay_us(1);	   
	TW2836_IIC_SCL=1;delay_us(1);	 
	while(TW2836_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>150)
		{
//			TW2836_IIC_Stop();
//			return 1;
			break;
		}
	}
	TW2836_IIC_SCL=0;//时钟输出0 	   
	return 0;  
}
//产生ACK应答 647.28 1852.72

void TW2836_IIC_Ack(void)
{
	TW2836_IIC_SCL=0;
	TW2836_SDA_OUT();
	TW2836_IIC_SDA=0;
	delay_us(2);
	TW2836_IIC_SCL=1;
	delay_us(2);
	TW2836_IIC_SCL=0;
}
//不产生ACK应答		    
void TW2836_IIC_NAck(void)
{
	TW2836_IIC_SCL=0;
	TW2836_SDA_OUT();
	TW2836_IIC_SDA=1;
	delay_us(2);
	TW2836_IIC_SCL=1;
	delay_us(2);
	TW2836_IIC_SCL=0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void TW2836_IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	TW2836_SDA_OUT(); 	    
    TW2836_IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        TW2836_IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		TW2836_IIC_SCL=1;
		delay_us(2); 
		TW2836_IIC_SCL=0;	
		delay_us(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 TW2836_IIC_Read_Byte(unsigned char ack)
{
	unsigned char i=0,receive=0;
	TW2836_IIC_SCL=0; 
	TW2836_SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        TW2836_IIC_SCL=0; 
        delay_us(2);
		TW2836_IIC_SCL=1;
		delay_us(2);
        receive<<=1;
        if(TW2836_READ_SDA)receive++;   
		delay_us(2); 
    }					 
    if (!ack)
        TW2836_IIC_NAck();//发送nACK
    else
        TW2836_IIC_Ack(); //发送ACK   
    return receive;
}


u8 TW2836_WriteByte(u8 slave,u8 page ,u8 inde,u8 data)
{
	TW2836_IIC_Start();
	TW2836_IIC_Send_Byte(slave+0);
	TW2836_IIC_Wait_Ack();
	TW2836_IIC_Send_Byte(page);
	TW2836_IIC_Wait_Ack();
	TW2836_IIC_Send_Byte(inde);
	TW2836_IIC_Wait_Ack();
	TW2836_IIC_Send_Byte(data);
	TW2836_IIC_Wait_Ack();
	TW2836_IIC_Stop();
	return 0;
}

u8 TW2836_ReadByte(u8 slave,u8 page ,u8 inde,u8 *data)
{
	TW2836_IIC_Start();
	TW2836_IIC_Send_Byte(slave);
	TW2836_IIC_Wait_Ack();
	TW2836_IIC_Send_Byte(page);
	TW2836_IIC_Wait_Ack();
	TW2836_IIC_Send_Byte(inde);
	TW2836_IIC_Wait_Ack();
	TW2836_IIC_Stop();
	TW2836_IIC_Start();
	TW2836_IIC_Send_Byte(slave+1);
	TW2836_IIC_Wait_Ack();
	*data = TW2836_IIC_Read_Byte(1);
	TW2836_IIC_Stop();
	return 0;
}

//==================================================================================
void WriteAsicByte(u8 _dvc, u8 _pg, u8 _addr, u8 _wdat)
{
	TW2836_WriteByte(_dvc,_pg,_addr,_wdat);
}

u8 ReadAsicByte(u8 _dvc, u8 _pg, u8 _addr)
{
	u8 temp;
	if(0 == TW2836_ReadByte(_dvc,_pg,_addr,&temp))
		return temp;
	else return 1;
}



//==================================================================================
void WriteAsicTable(u8 _dvc, u8 _pg, u8 _addr,const u8 *_tbl_ptr, u8 _tbl_cnt)
{
	u8 i = 0;
	TW2836_IIC_Start();
	TW2836_IIC_Send_Byte(_dvc+0);
	TW2836_IIC_Wait_Ack();
	TW2836_IIC_Send_Byte(_pg);
	TW2836_IIC_Wait_Ack();
	TW2836_IIC_Send_Byte(_addr);
	TW2836_IIC_Wait_Ack();
	
	for(i=0;i<_tbl_cnt;i++)
	{
		TW2836_IIC_Send_Byte(_tbl_ptr[i]);
		TW2836_IIC_Wait_Ack();
	}
	TW2836_IIC_Stop();
}

//==================================================================================
void InitPg0(void)
{
	u8 _t1_;
	for(_t1_=0;_t1_<4;_t1_++)
	{
		WriteAsicTable(MASTER,DVC_PG0,0x00+0x10*_t1_,tbl_ntsc_pg0_cmn,15);//P120
		WriteAsicByte(MASTER,DVC_PG0,0x0c+0x10*_t1_,(_t1_<<6)|0x06);			//... x path ANA_CH mux			//
		WriteAsicTable(MASTER,DVC_PG0,0x80+0x10*_t1_,tbl_ntsc_pg0_scl,16);
		WriteAsicByte(MASTER,DVC_PG0,0x80+0x10*_t1_,(_t1_<<6)|0x05);			//... x path scale filter : quad
//		WriteAsicByte(MASTER,DVC_PG0,0x80+0x10*_t1_,(_t1_<<6)|0x06);			//... x path scale filter : 1/3 size
		WriteAsicByte(MASTER,DVC_PG0,0x8a+0x10*_t1_,(_t1_<<6)|0x31);			//... y path scale filter : quad
	}
	
//	WriteAsicByte(MASTER,DVC_PG0,0x80+0x10*_t1_,(_t1_<<6)|0x05);	//输入通道选择
	
	
	WriteAsicTable(MASTER,DVC_PG0,0x40,tbl_ntsc_pg0_sfr1,21);
	WriteAsicTable(MASTER,DVC_PG0,0x60,tbl_ntsc_pg0_sfr2,21);
	WriteAsicTable(MASTER,DVC_PG0,0xc0,tbl_ntsc_pg0_sfr3,11);
}

//==================================================================================
void InitPg1(void)
{
	u8 _t1_;//, _t2_=0;

	WriteAsicTable(MASTER,DVC_PG1,0x01,tbl_pg1_x_cmn,47);
	WriteAsicTable(MASTER,DVC_PG1,0x50,tbl_pg1_y_cmn,80);
	
	WriteAsicTable(MASTER,DVC_PG1,0x30,tbl_ntsc_pg1_pic_qd,16);		//... normal quad
//	WriteAsicTable(MASTER,DVC_PG1,0x30,tbl_ntsc_pg1_pic_9_lt,16);		//... non-realtime
	WriteAsicTable(MASTER,DVC_PG1,0x40,tbl_ntsc_pg1_pic_9_rb,16);
	WriteAsicTable(MASTER,DVC_PG1,0xa0,tbl_ntsc_pg1_enc,16);	
	WriteAsicByte(MASTER,DVC_PG1,0x00,0x00);								//... NTSC

	//... queue setting
	for(_t1_=0;_t1_<16;_t1_++)
	{
		//... mux queue
		WriteAsicByte(MASTER,DVC_PG1,0x59,_t1_);			//... queue data
		WriteAsicByte(MASTER,DVC_PG1,0x5a,0x80|_t1_);		//... queue addr
	}
	WriteAsicByte(MASTER,DVC_PG1,0x73,0x01);	//... queue data
	WriteAsicByte(MASTER,DVC_PG1,0x74,0x23);	//... queue data
	WriteAsicByte(MASTER,DVC_PG1,0x75,0x80);	//... queue addr
	WriteAsicByte(MASTER,DVC_PG1,0x73,0x12);	//... queue data
	WriteAsicByte(MASTER,DVC_PG1,0x74,0x30);	//... queue data
	WriteAsicByte(MASTER,DVC_PG1,0x75,0x81);	//... queue addr
	WriteAsicByte(MASTER,DVC_PG1,0x73,0x23);	//... queue data
	WriteAsicByte(MASTER,DVC_PG1,0x74,0x01);	//... queue data
	WriteAsicByte(MASTER,DVC_PG1,0x75,0x82);	//... queue addr
	WriteAsicByte(MASTER,DVC_PG1,0x73,0x30);	//... queue data
	WriteAsicByte(MASTER,DVC_PG1,0x74,0x12);	//... queue data
	WriteAsicByte(MASTER,DVC_PG1,0x75,0x83);	//... queue addr
//	WriteAsicByte(MASTER,DVC_PG1,0x80,0xff);	// Enable the Auto channel ID	
	WriteAsicByte(MASTER,DVC_PG1,0x56,0x40);	//... start internal trigger for mux queue
}


void TW2836_init(void)
{
	TW2836_IIC_Init();
	InitPg0();
	InitPg1();
	

//	printf("0x00:0x%x\r\n",ReadAsicByte(MASTER,DVC_PG0,0));
//	printf("0x01:0x%x\r\n",ReadAsicByte(MASTER,DVC_PG0,1));
//	printf("0x02:0x%x\r\n",ReadAsicByte(MASTER,DVC_PG0,2));
//	printf("0x03:0x%x\r\n",ReadAsicByte(MASTER,DVC_PG0,3));
	
//	TW2836_set(0,0x80);
//	TW2836_set(1,0x40);
//	TW2836_set(2,0x80);
//	TW2836_set(3,0x30);
	
}

//mode:  0:色调；1:亮度；2:饱和度；3:对比度；
//0:0x80	TW2836_set(0,0x80);
//1:0x40	TW2836_set(1,0x40);
//2:0x80	TW2836_set(2,0x80);
//3:0x30	TW2836_set(3,0x30);
void TW2836_set(u8 mode,u8 value)
{
	u8 index = 0;
	if(mode>3)return;
	switch(mode)
	{
		case 0:index = 0x07;break;
		case 1:index = 0x0A;break;
		case 2:index = 0x08;break;
		case 3:index = 0x09;break;
		default:break;
	}
	
	TW2836_WriteByte(MASTER,DVC_PG0,0x00+index,value);
	TW2836_WriteByte(MASTER,DVC_PG0,0x10+index,value);
	TW2836_WriteByte(MASTER,DVC_PG0,0x20+index,value);
	TW2836_WriteByte(MASTER,DVC_PG0,0x30+index,value);
}

u8 TW2836_Ctrl(u8 mode)
{
	u8 temp;
	u8 buff[4];
	if(mode >4)return 0;
	
	InitPg0();
	InitPg1();
	
	if(mode == 4)return 0;
	
	buff[0] = 0x00;
	buff[1] = 0xB4;//B4
	buff[2] = 0x00;
	buff[3] = 0x78;//78
	WriteAsicTable(MASTER,DVC_PG1,mode*4+0x30,buff,4);

	buff[0] = 0xFf;										//Scaling
	buff[1] = 0xff;
	buff[2] = 0xFf;
	buff[3] = 0xff;
	WriteAsicTable(MASTER,DVC_PG0,mode*0x10+0x81,buff,4);
	temp = ReadAsicByte(MASTER,DVC_PG1,mode*8+0x10);
	TW2836_WriteByte(MASTER,DVC_PG1,mode*8+0x10,temp|(1<<6));//set FUNC_MODE = 1
	return 1;
}

u8 TW2836_Getver(void)
{
	u8 temp = 0;
	temp = ReadAsicByte(MASTER,DVC_PG0,0xFE);
	temp = temp>>3;
	
	if(0x05 == temp)return 0;
	else return 1;
}


#include "information.h"
u8 TW2836_Mode = 4;
u8 TW2836_CMD = 0;
u8 TW2836_Set_mode = 0;
u8 TW2836_Set_value = 0;

void TW2836_Switch(u8 value)
{
	if((value>4)||(value==3))return;
	TW2836_Mode = value;
//	printf("get switch value:%d\r\n",value);
}
void TW2836_Adjust(u8 mode,u8 value)
{
	if(mode>3)return;
	TW2836_Set_mode = mode;
	TW2836_Set_value = value;
	TW2836_CMD = 1;
}

void TW2836_Process(void)
{
	static u8 last_power = 0;
	static u8 last_mode;
	static u8 check = 0;
	static u8 cnt = 10;
	if((cnt++)>=10)
	{
		cnt = 0;
		TW2836_IIC_Init();
		if(TW2836_Getver())
		{
			check ++;
			if(check >= 5)
			{
				TW2836_init();
				last_mode = 4;
				check = 0;
			}
//			printf("TW2836 ID ERR\r\n");
		}
		else check = 0;
	}
	else
	{
		if(CAR_Inf.ACC != last_power)
		{
			last_power = CAR_Inf.ACC;
			if(last_power == 1)
			{
				TW2836_init();
				last_mode = 4;
			}
		}
		
		
		if(TW2836_Mode != last_mode)
		{
			last_mode = TW2836_Mode;
			TW2836_Ctrl(TW2836_Mode);
//			printf("set switch value:%d\r\n",TW2836_Mode);
		}
		if(TW2836_CMD == 1)
		{
			TW2836_CMD = 0;
			TW2836_set(TW2836_Set_mode,TW2836_Set_value);
		}
	}
}








