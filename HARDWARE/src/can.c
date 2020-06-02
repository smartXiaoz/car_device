#include "can.h"
#include "delay.h"
#include "usart.h"
#include "information.h"

//CAN接收RX0中断使能
#define CAN_RX0_INT_ENABLE	1		//0,不使能;1,使能.
#define	CAN_DEBUG	0							//


#define Can_Msg		8		//每帧数据长度，
#define Can_Ide		1		//1为扩展帧，0为标准帧
#define Can_Rtr		0		//0为数据帧，1为远程帧（遥控帧）




void CAN_SENGSTOP(void)
{
	CAN1->TSR |= 1<<23;
	CAN1->TSR |= 1<<15;
	CAN1->TSR |= 1<<7;
}


void DINT_CAN(void)
{
	RCC->APB1ENR&=~(1<<25);	//使能CAN时钟 CAN使用的是APB1的时钟(max:36M)
	RCC->APB2ENR&=~(1<<2);    	//使能PORTA时钟	
	
	CAN1->MCR = 0x00010002;
	CAN1->MSR = 0x00000C02;
	CAN1->TSR = 0x1C000000;
	CAN1->RF0R = 0x00000000;
	CAN1->RF1R = 0x00000000;
	CAN1->IER = 0x00000000;
	CAN1->ESR = 0x00000000;
	CAN1->BTR = 0x01230000;
	
}





//CAN初始化
//tsjw:重新同步跳跃时间单元.范围:1~3;
//tbs2:时间段2的时间单元.范围:1~8;
//tbs1:时间段1的时间单元.范围:1~16;
//brp :波特率分频器.范围:1~1024;(实际要加1,也就是1~1024) tq=(brp)*tpclk1
//注意以上参数任何一个都不能设为0,否则会乱.
//波特率=Fpclk1/((tbs1+tbs2+1)*brp);
//mode:0,普通模式;1,回环模式;
//Fpclk1的时钟在初始化的时候设置为36M,如果设置CAN_Normal_Init(1,8,9,4,1);
//则波特率为:36M/((8+9+1)*4)=500Kbps
//CAN_Normal_Init(1,8,9,8,0);波特率250；采样点55.56
//CAN_Normal_Init(1,5,18,6,0);波特率250；采样点79.17
//CAN_Normal_Init(1,3,14,8,0);波特率250；采样点83.333
//CAN_Normal_Init(1,4,13,8,0);波特率250；采样点87.7777
//返回值:0,初始化OK;
//    其他,初始化失败;
u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{
	u16 i=0;
	u32	FR = 0;
	
 	if(tsjw==0||tbs2==0||tbs1==0||brp==0)return 1;
	tsjw-=1;//先减去1.再用于设置
	tbs2-=1;
	tbs1-=1;
	brp-=1;
	
	DINT_CAN();

	
	RCC->APB2ENR|=1<<2;    	//使能PORTA时钟	 
	GPIOA->CRH&=0XFFF00FFF; 
	GPIOA->CRH|=0X000B8000;	//PA11 RX,PA12 TX推挽输出   	 
    GPIOA->ODR|=3<<11;
					    
	RCC->APB1ENR|=1<<25;	//使能CAN时钟 CAN使用的是APB1的时钟(max:36M)
	CAN1->MCR=0x0000;		//退出睡眠模式(同时设置所有位为0)
	CAN1->MCR|=1<<0;		//请求CAN进入初始化模式
	while((CAN1->MSR&1<<0)==0)
	{
		i++;
		if(i>100)return 2;	//进入初始化模式失败
	}
	CAN1->MCR|=0<<7;		//非时间触发通信模式
#if (BUSSOFFTEST)
	CAN1->MCR|=0<<6;		//自动离线软件管理
#else
	CAN1->MCR|=1<<6;		//自动离线硬件管理
#endif
	CAN1->MCR|=1<<5;		//睡眠模式通过软件唤醒(清除CAN1->MCR的SLEEP位)
	CAN1->MCR|=0<<4;		//报文自动传送
	CAN1->MCR|=0<<3;		//报文不锁定,新的覆盖旧的
	CAN1->MCR|=0<<2;		//优先级由报文标识符决定
	CAN1->BTR=0x00000000;	//清除原来的设置.
	CAN1->BTR|=mode<<30;	//模式设置 0,普通模式;1,回环模式;
	CAN1->BTR|=tsjw<<24; 	//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位
	CAN1->BTR|=tbs2<<20; 	//Tbs2=tbs2+1个时间单位
	CAN1->BTR|=tbs1<<16;	//Tbs1=tbs1+1个时间单位
	CAN1->BTR|=brp<<0;  	//分频系数(Fdiv)为brp+1
							//波特率:Fpclk1/((Tbs1+Tbs2+1)*Fdiv)
	CAN1->MCR&=~(1<<0);		//请求CAN退出初始化模式
	while((CAN1->MSR&1<<0)==1)
	{
		i++;
		if(i>0XFFF0)return 3;//退出初始化模式失败
	}
	//过滤器初始化
	
	FR = (Can_Ide<<2)+(Can_Rtr<<1);
	
	CAN1->FMR|=1<<0;		//过滤器组工作在初始化模式
	
#ifdef	ES50I	
	CAN1->FA1R&=~(0x00003FFF);	//过滤器不激活
	CAN1->FS1R|=0x00003FFF;	//过滤器位宽为32位.
	CAN1->FM1R|=0x00003FFF;	//过滤器工作在列表模式（置1为列表模式，清0位标识符屏蔽位模式）
#else
	CAN1->FA1R&=~(0x0000003F);	//过滤器不激活
	CAN1->FS1R|=0x0000003F;	//过滤器位宽为32位.
	CAN1->FM1R|=0x0000003F;	//过滤器工作在列表模式（置1为列表模式，清0位标识符屏蔽位模式）
#endif
	
	CAN1->FFA1R|=0<<0;		//过滤器关联到FIFO0
	
	
	
	//以下列出的ID才会被接收，其余被过滤器滤除
	CAN1->sFilterRegister[0].FR1=(((u32)0x18FEBF0B<<3)+FR);//32位ID
	CAN1->sFilterRegister[0].FR2=(((u32)0x10FF1021<<3)+FR);//32位ID
	CAN1->sFilterRegister[1].FR1=(((u32)0x18FF6FEE<<3)+FR);//32位ID
	CAN1->sFilterRegister[1].FR2=(((u32)0x08FF00DD<<3)+FR);//32位ID
	CAN1->sFilterRegister[2].FR1=(((u32)0x10FF7521<<3)+FR);//32位ID
	CAN1->sFilterRegister[2].FR2=(((u32)0x10FF4117<<3)+FR);//32位ID	
	CAN1->sFilterRegister[3].FR1=(((u32)0x18A005E7<<3)+FR);//32位ID
	CAN1->sFilterRegister[3].FR2=(((u32)0x1CA00735<<3)+FR);//32位ID
	CAN1->sFilterRegister[4].FR1=(((u32)0x18F00503<<3)+FR);//32位ID
	CAN1->sFilterRegister[4].FR2=(((u32)0x18DA76FA<<3)+FR);//32位ID
	CAN1->sFilterRegister[5].FR1=(((u32)0x18DBFFFA<<3)+FR);//32位ID
	CAN1->sFilterRegister[5].FR2=(((u32)0x18DBFFFA<<3)+FR);//32位ID
	
#ifdef	ES50I
	
	CAN1->sFilterRegister[4].FR1=(((u32)0x18FE06E3<<3)+FR);//32位ID
	CAN1->sFilterRegister[1].FR2=(((u32)0x18FE07E3<<3)+FR);//32位ID
	
	CAN1->sFilterRegister[5].FR2=(((u32)0x00000285<<3)+FR);//32位ID
	CAN1->sFilterRegister[6].FR1=(((u32)0x00000286<<3)+FR);//32位ID
	CAN1->sFilterRegister[6].FR2=(((u32)0x00000287<<3)+FR);//32位ID
	CAN1->sFilterRegister[7].FR1=(((u32)0x00000288<<3)+FR);//32位ID
	CAN1->sFilterRegister[7].FR2=(((u32)0x00000185<<3)+FR);//32位ID
	CAN1->sFilterRegister[8].FR1=(((u32)0x00000186<<3)+FR);//32位ID
	CAN1->sFilterRegister[8].FR2=(((u32)0x00000187<<3)+FR);//32位ID
	
	CAN1->sFilterRegister[9].FR1=(((u32)0x18FE01E3<<3)+FR);//32位ID
	CAN1->sFilterRegister[9].FR2=(((u32)0x18FE02E3<<3)+FR);//32位ID	
	CAN1->sFilterRegister[10].FR1=(((u32)0x18FE03E3<<3)+FR);//32位ID
	CAN1->sFilterRegister[10].FR2=(((u32)0x18FE04E3<<3)+FR);//32位ID	
	CAN1->sFilterRegister[11].FR1=(((u32)0x18FE05E3<<3)+FR);//32位ID
	CAN1->sFilterRegister[11].FR2=(((u32)0x18FE08E3<<3)+FR);//32位ID	
	CAN1->sFilterRegister[12].FR1=(((u32)0x18FE09E3<<3)+FR);//32位ID
	CAN1->sFilterRegister[12].FR2=(((u32)0x18FE10E3<<3)+FR);//32位ID	
	CAN1->sFilterRegister[13].FR1=(((u32)0x18FE11E3<<3)+FR);//32位ID
	CAN1->sFilterRegister[13].FR2=(((u32)0x18FE12E3<<3)+FR);//32位ID	
	
#endif	
	
#ifdef	ES50I
	CAN1->FA1R|=0x00003FFF;	//激活过滤器
#else
	CAN1->FA1R|=0x0000003F;	//激活过滤器
#endif
	
	
	CAN1->FMR&=0<<0;		//过滤器组进入正常模式

#if CAN_RX0_INT_ENABLE
 	//使用中断接收
	CAN1->IER|=1<<1;		//FIFO0消息挂号中断允许.
//	CAN1->IER|=1<<8;		//错误警告中断使能.
//	CAN1->IER|=1<<9;		//错误被动中断允许.
	CAN1->IER|=1<<10;		//离线中断允许.
//	CAN1->IER|=1<<11;		//上次错误号中断使能.	
	CAN1->IER|=1<<15;		//错误中断允许.	
	MY_NVIC_Init(8,0,CAN1_SCE_IRQn,4);//组4
	MY_NVIC_Init(8,0,USB_LP_CAN1_RX0_IRQn,4);//组4
#endif


//	FAULT_Inf.BUSOFF = 0;
//	printf("CAN Init\r\n");
	return 0;
}   

//发送一帧数据（内部调用）
//id:标准ID(11位)/扩展ID(11位+18位)	    
//ide:0,标准帧;1,扩展帧
//rtr:0,数据帧;1,远程帧
//len:要发送的数据长度(固定为8个字节,在时间触发模式下,有效数据为6个字节)
//*dat:数据指针.
//返回值:0~3,邮箱编号.0XFF,无有效邮箱.
u8 CAN_Tx_Msg(u32 id,u8 ide,u8 rtr,u8 len,u8 *dat)
{	   
	u8 mbox;	  
	if(CAN1->TSR&(1<<26))mbox=0;		//邮箱0为空
//	else if(CAN1->TSR&(1<<27))mbox=1;	//邮箱1为空
//	else if(CAN1->TSR&(1<<28))mbox=2;	//邮箱2为空
	else return 0XFF;					//无空邮箱,无法发送 
	CAN1->sTxMailBox[mbox].TIR=0;		//清除之前的设置
	if(ide==0)	//标准帧
	{
		id&=0x7ff;//取低11位stdid
		id<<=21;		  
	}else		//扩展帧
	{
		id&=0X1FFFFFFF;//取低32位extid
		id<<=3;									   
	}
	CAN1->sTxMailBox[mbox].TIR|=id;		 
	CAN1->sTxMailBox[mbox].TIR|=ide<<2;	  
	CAN1->sTxMailBox[mbox].TIR|=rtr<<1;
	len&=0X0F;//得到低四位
	CAN1->sTxMailBox[mbox].TDTR&=~(0X0000000F);
	CAN1->sTxMailBox[mbox].TDTR|=len;	//设置DLC.
	//待发送数据存入邮箱.
	CAN1->sTxMailBox[mbox].TDHR=(((u32)dat[7]<<24)|
								((u32)dat[6]<<16)|
 								((u32)dat[5]<<8)|
								((u32)dat[4]));
	CAN1->sTxMailBox[mbox].TDLR=(((u32)dat[3]<<24)|
								((u32)dat[2]<<16)|
 								((u32)dat[1]<<8)|
								((u32)dat[0]));
	CAN1->sTxMailBox[mbox].TIR|=1<<0; 	//请求发送邮箱数据
	return mbox;
}
//获得发送状态.
//mbox:邮箱编号;
//返回值:发送状态. 0,挂起;0X05,发送失败;0X07,发送成功.
u8 CAN_Tx_Staus(u8 mbox)
{	
	u8 sta=0;					    
	switch (mbox)
	{
		case 0: 
			sta |= CAN1->TSR&(1<<0);		//RQCP0
			sta |= CAN1->TSR&(1<<1);		//TXOK0
			sta |=((CAN1->TSR&(1<<26))>>24);//TME0
			break;
		case 1: 
			sta |= CAN1->TSR&(1<<8)>>8;		//RQCP1
			sta |= CAN1->TSR&(1<<9)>>8;		//TXOK1
			sta |=((CAN1->TSR&(1<<27))>>25);//TME1	   
			break;
		case 2: 
			sta |= CAN1->TSR&(1<<16)>>16;	//RQCP2
			sta |= CAN1->TSR&(1<<17)>>16;	//TXOK2
			sta |=((CAN1->TSR&(1<<28))>>26);//TME2
			break;
		default:
			sta=0X05;//邮箱号不对,肯定失败.
		break;
	}
	return sta;
} 
//得到在FIFO0/FIFO1中接收到的报文个数.
//fifox:0/1.FIFO编号;
//返回值:FIFO0/FIFO1中的报文个数.
u8 CAN_Msg_Pend(u8 fifox)
{
	if(fifox==0)return CAN1->RF0R&0x03; 
	else if(fifox==1)return CAN1->RF1R&0x03; 
	else return 0;
}
//接收数据
//fifox:邮箱号
//id:标准ID(11位)/扩展ID(11位+18位)	    
//ide:0,标准帧;1,扩展帧
//rtr:0,数据帧;1,远程帧
//len:接收到的数据长度(固定为8个字节,在时间触发模式下,有效数据为6个字节)
//dat:数据缓存区
void CAN_Rx_Msg(u8 fifox,u32 *id,u8 *ide,u8 *rtr,u8 *len,u8 *dat)
{	   
	*ide=CAN1->sFIFOMailBox[fifox].RIR&0x04;	//得到标识符选择位的值  
 	if(*ide==0)//标准标识符
	{
		*id=CAN1->sFIFOMailBox[fifox].RIR>>21;
	}else	   //扩展标识符
	{
		*id=CAN1->sFIFOMailBox[fifox].RIR>>3;
	}
	*rtr=CAN1->sFIFOMailBox[fifox].RIR&0x02;	//得到远程发送请求值.
	*len=CAN1->sFIFOMailBox[fifox].RDTR&0x0F;	//得到DLC
 	//*fmi=(CAN1->sFIFOMailBox[FIFONumber].RDTR>>8)&0xFF;//得到FMI
	//接收数据
	dat[0]=CAN1->sFIFOMailBox[fifox].RDLR&0XFF;
	dat[1]=(CAN1->sFIFOMailBox[fifox].RDLR>>8)&0XFF;
	dat[2]=(CAN1->sFIFOMailBox[fifox].RDLR>>16)&0XFF;
	dat[3]=(CAN1->sFIFOMailBox[fifox].RDLR>>24)&0XFF;    
	dat[4]=CAN1->sFIFOMailBox[fifox].RDHR&0XFF;
	dat[5]=(CAN1->sFIFOMailBox[fifox].RDHR>>8)&0XFF;
	dat[6]=(CAN1->sFIFOMailBox[fifox].RDHR>>16)&0XFF;
	dat[7]=(CAN1->sFIFOMailBox[fifox].RDHR>>24)&0XFF;    
  	if(fifox==0)CAN1->RF0R|=0X20;//释放FIFO0邮箱
	else if(fifox==1)CAN1->RF1R|=0X20;//释放FIFO1邮箱	 
}

#if CAN_RX0_INT_ENABLE	//使能RX0中断
//中断服务函数	
void Can_Write_RXMsg(Can_SF RX_Msg);//向缓冲区添加一帧报文

//CAN中断处理函数，
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	u8 ide,rtr,len;  
	Can_SF rx_data;
	
	CAN_Rx_Msg(0,&rx_data.id,&ide,&rtr,&len,rx_data.data);
	
	//调用外部接口，接收数据，注意：此处为中断内，禁延时、等待循环	
	Can_Write_RXMsg(rx_data);//将收到的数据放入缓存区

	CAN1->RF0R|=0X20;//释放FIFO0邮箱
	CAN1->RF1R|=0X20;//释放FIFO1邮箱	
	
#if	CAN_DEBUG	//调试时可选择从串口打印出收到的报文

#endif
}

void CAN1_SCE_IRQHandler(void)
{
	
//	printf("CAN1->ESR =0x%x\r\n",CAN1->ESR);
//	printf("CAN1->MSR =0x%x\r\n",CAN1->MSR);
//	printf("CAN1->MCR =0x%x\r\n",CAN1->MCR);
	
//	if((CAN1->ESR&(1<<2))||(CAN1->ESR&(1<<1)))
	if(CAN1->ESR&(1<<2))
	{
		//离线
		FAULT_Inf.BUSOFF = 1;//bus-off
		FAULT_Inf.CAN_OFF = 1;		
	}

	
	CAN1->MSR&=~(1<<2);
	CAN1->MSR&=~(1<<3);
	CAN1->MSR&=~(1<<4);
	CAN1->ESR&=~(0x50);
	CAN1->MCR&=~(1<<1);
}

#endif


//can发送一组数据
//返回值:0,成功;
//		 其他,失败;
static u16 send_err_cnt = 0;
u8 CAN_Send_Msg(Can_SF TX_Msg)
{	
	u8 mbox;
	u16 i=0;	  	 						       
	  mbox=CAN_Tx_Msg(TX_Msg.id,Can_Ide,Can_Rtr,Can_Msg,TX_Msg.data);
	while((CAN_Tx_Staus(mbox)!=0X07)&&(i<0XFFF))i++;//等待发送结束
	if(i>=0XFFF)//发送失败?
	{
		if(send_err_cnt < 20)send_err_cnt++;
		if(((CAN1->ESR&0xFF000000) == 0xFF000000)||(send_err_cnt > 20))
		{
			send_err_cnt = 0;
			FAULT_Inf.BUSOFF = 1;
			FAULT_Inf.CAN_OFF = 1;
//			printf("send err;\r\n");
		}
		
		return 1;
	}
//	printf("send ok;\r\n");
	send_err_cnt = 0;
	FAULT_Inf.BUSOFF = 0;		//发送成功，总线恢复
	FAULT_Inf.BUSOFF_Flag = 0;
//	FAULT_Inf.CAN_OFF = 0;
	return 0;					//发送成功;
}
//can口接收数据查询
//buf:数据缓存区;	 
//返回值:0,无数据被收到;
//		 其他,接收的数据长度;
u8 CAN_Receive_Msg(u8 *buf)
{		   		   
	u32 id;
	u8 ide,rtr,len; 
	if(CAN_Msg_Pend(0)==0)return 0;			//没有接收到数据,直接退出 	 
  	CAN_Rx_Msg(0,&id,&ide,&rtr,&len,buf); 	//读取数据
    if(id!=0x12||ide!=0||rtr!=0)len=0;		//接收错误	   
	return len;	
}



CAN_CACHE CAN_RX;

//向缓存区添加一帧数据
void Can_Write_RXMsg(Can_SF RX_Msg)
{
	CAN_RX.frame[CAN_RX.writpointer++] = RX_Msg;

	if(CAN_RX.writpointer>=CAN_LEN) CAN_RX.writpointer = 0;
	if(CAN_RX.writpointer==CAN_RX.readpointer)
	{
		CAN_RX.readpointer++;
		if(CAN_RX.readpointer>=CAN_LEN)CAN_RX.readpointer=0;
	}
}

//从缓存区取出一帧数据，
//返回值：	0，没有数据
//			1，拿到一帧数据
u8 Can_Get_RXMsg(Can_SF *RX_Msg)
{
	if(CAN_RX.writpointer==CAN_RX.readpointer) return 0;//缓存区空，没有数据
	else 
	{
		*RX_Msg = CAN_RX.frame[CAN_RX.readpointer++];
		if(CAN_RX.readpointer>=CAN_LEN)CAN_RX.readpointer=0;
		return 1;
	}
}

void CAN_RXclean(void)
{
	CAN_RX.readpointer = CAN_RX.writpointer;
}


CAN_CACHE CAN_TX0;
CAN_CACHE CAN_TX1;

void Can_Write_TXMsg(Can_SF TX_Msg,u8 Priority)
{
	if(Priority == 0)
	{
		CAN_TX0.frame[CAN_TX0.writpointer++] = (TX_Msg);
		if(CAN_TX0.writpointer>=CAN_LEN)CAN_TX0.writpointer=0;
	}
	else if(Priority == 1)
	{
		CAN_TX1.frame[CAN_TX1.writpointer++] = (TX_Msg);
		if(CAN_TX1.writpointer>=CAN_LEN)CAN_TX1.writpointer=0;
	}
}


u8 Can_Get_TXMsg(Can_SF *TX_Msg) 
{

	if(CAN_TX0.readpointer != CAN_TX0.writpointer)
	{
		(*TX_Msg) = CAN_TX0.frame[CAN_TX0.readpointer++];
		if(CAN_TX0.readpointer>=CAN_LEN)CAN_TX0.readpointer=0;
		return 1;
	}
	else if(CAN_TX1.readpointer != CAN_TX1.writpointer)
	{
		(*TX_Msg) = CAN_TX1.frame[CAN_TX1.readpointer++];
		if(CAN_TX1.readpointer>=CAN_LEN)CAN_TX1.readpointer=0;
		return 2;
	}	
	
	else return 0;
}





