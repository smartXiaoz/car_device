#include "can.h"
#include "delay.h"
#include "usart.h"
#include "information.h"

//CAN����RX0�ж�ʹ��
#define CAN_RX0_INT_ENABLE	1		//0,��ʹ��;1,ʹ��.
#define	CAN_DEBUG	0							//


#define Can_Msg		8		//ÿ֡���ݳ��ȣ�
#define Can_Ide		1		//1Ϊ��չ֡��0Ϊ��׼֡
#define Can_Rtr		0		//0Ϊ����֡��1ΪԶ��֡��ң��֡��




void CAN_SENGSTOP(void)
{
	CAN1->TSR |= 1<<23;
	CAN1->TSR |= 1<<15;
	CAN1->TSR |= 1<<7;
}


void DINT_CAN(void)
{
	RCC->APB1ENR&=~(1<<25);	//ʹ��CANʱ�� CANʹ�õ���APB1��ʱ��(max:36M)
	RCC->APB2ENR&=~(1<<2);    	//ʹ��PORTAʱ��	
	
	CAN1->MCR = 0x00010002;
	CAN1->MSR = 0x00000C02;
	CAN1->TSR = 0x1C000000;
	CAN1->RF0R = 0x00000000;
	CAN1->RF1R = 0x00000000;
	CAN1->IER = 0x00000000;
	CAN1->ESR = 0x00000000;
	CAN1->BTR = 0x01230000;
	
}





//CAN��ʼ��
//tsjw:����ͬ����Ծʱ�䵥Ԫ.��Χ:1~3;
//tbs2:ʱ���2��ʱ�䵥Ԫ.��Χ:1~8;
//tbs1:ʱ���1��ʱ�䵥Ԫ.��Χ:1~16;
//brp :�����ʷ�Ƶ��.��Χ:1~1024;(ʵ��Ҫ��1,Ҳ����1~1024) tq=(brp)*tpclk1
//ע�����ϲ����κ�һ����������Ϊ0,�������.
//������=Fpclk1/((tbs1+tbs2+1)*brp);
//mode:0,��ͨģʽ;1,�ػ�ģʽ;
//Fpclk1��ʱ���ڳ�ʼ����ʱ������Ϊ36M,�������CAN_Normal_Init(1,8,9,4,1);
//������Ϊ:36M/((8+9+1)*4)=500Kbps
//CAN_Normal_Init(1,8,9,8,0);������250��������55.56
//CAN_Normal_Init(1,5,18,6,0);������250��������79.17
//CAN_Normal_Init(1,3,14,8,0);������250��������83.333
//CAN_Normal_Init(1,4,13,8,0);������250��������87.7777
//����ֵ:0,��ʼ��OK;
//    ����,��ʼ��ʧ��;
u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{
	u16 i=0;
	u32	FR = 0;
	
 	if(tsjw==0||tbs2==0||tbs1==0||brp==0)return 1;
	tsjw-=1;//�ȼ�ȥ1.����������
	tbs2-=1;
	tbs1-=1;
	brp-=1;
	
	DINT_CAN();

	
	RCC->APB2ENR|=1<<2;    	//ʹ��PORTAʱ��	 
	GPIOA->CRH&=0XFFF00FFF; 
	GPIOA->CRH|=0X000B8000;	//PA11 RX,PA12 TX�������   	 
    GPIOA->ODR|=3<<11;
					    
	RCC->APB1ENR|=1<<25;	//ʹ��CANʱ�� CANʹ�õ���APB1��ʱ��(max:36M)
	CAN1->MCR=0x0000;		//�˳�˯��ģʽ(ͬʱ��������λΪ0)
	CAN1->MCR|=1<<0;		//����CAN�����ʼ��ģʽ
	while((CAN1->MSR&1<<0)==0)
	{
		i++;
		if(i>100)return 2;	//�����ʼ��ģʽʧ��
	}
	CAN1->MCR|=0<<7;		//��ʱ�䴥��ͨ��ģʽ
#if (BUSSOFFTEST)
	CAN1->MCR|=0<<6;		//�Զ������������
#else
	CAN1->MCR|=1<<6;		//�Զ�����Ӳ������
#endif
	CAN1->MCR|=1<<5;		//˯��ģʽͨ���������(���CAN1->MCR��SLEEPλ)
	CAN1->MCR|=0<<4;		//�����Զ�����
	CAN1->MCR|=0<<3;		//���Ĳ�����,�µĸ��Ǿɵ�
	CAN1->MCR|=0<<2;		//���ȼ��ɱ��ı�ʶ������
	CAN1->BTR=0x00000000;	//���ԭ��������.
	CAN1->BTR|=mode<<30;	//ģʽ���� 0,��ͨģʽ;1,�ػ�ģʽ;
	CAN1->BTR|=tsjw<<24; 	//����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ
	CAN1->BTR|=tbs2<<20; 	//Tbs2=tbs2+1��ʱ�䵥λ
	CAN1->BTR|=tbs1<<16;	//Tbs1=tbs1+1��ʱ�䵥λ
	CAN1->BTR|=brp<<0;  	//��Ƶϵ��(Fdiv)Ϊbrp+1
							//������:Fpclk1/((Tbs1+Tbs2+1)*Fdiv)
	CAN1->MCR&=~(1<<0);		//����CAN�˳���ʼ��ģʽ
	while((CAN1->MSR&1<<0)==1)
	{
		i++;
		if(i>0XFFF0)return 3;//�˳���ʼ��ģʽʧ��
	}
	//��������ʼ��
	
	FR = (Can_Ide<<2)+(Can_Rtr<<1);
	
	CAN1->FMR|=1<<0;		//�������鹤���ڳ�ʼ��ģʽ
	
#ifdef	ES50I	
	CAN1->FA1R&=~(0x00003FFF);	//������������
	CAN1->FS1R|=0x00003FFF;	//������λ��Ϊ32λ.
	CAN1->FM1R|=0x00003FFF;	//�������������б�ģʽ����1Ϊ�б�ģʽ����0λ��ʶ������λģʽ��
#else
	CAN1->FA1R&=~(0x0000003F);	//������������
	CAN1->FS1R|=0x0000003F;	//������λ��Ϊ32λ.
	CAN1->FM1R|=0x0000003F;	//�������������б�ģʽ����1Ϊ�б�ģʽ����0λ��ʶ������λģʽ��
#endif
	
	CAN1->FFA1R|=0<<0;		//������������FIFO0
	
	
	
	//�����г���ID�Żᱻ���գ����౻�������˳�
	CAN1->sFilterRegister[0].FR1=(((u32)0x18FEBF0B<<3)+FR);//32λID
	CAN1->sFilterRegister[0].FR2=(((u32)0x10FF1021<<3)+FR);//32λID
	CAN1->sFilterRegister[1].FR1=(((u32)0x18FF6FEE<<3)+FR);//32λID
	CAN1->sFilterRegister[1].FR2=(((u32)0x08FF00DD<<3)+FR);//32λID
	CAN1->sFilterRegister[2].FR1=(((u32)0x10FF7521<<3)+FR);//32λID
	CAN1->sFilterRegister[2].FR2=(((u32)0x10FF4117<<3)+FR);//32λID	
	CAN1->sFilterRegister[3].FR1=(((u32)0x18A005E7<<3)+FR);//32λID
	CAN1->sFilterRegister[3].FR2=(((u32)0x1CA00735<<3)+FR);//32λID
	CAN1->sFilterRegister[4].FR1=(((u32)0x18F00503<<3)+FR);//32λID
	CAN1->sFilterRegister[4].FR2=(((u32)0x18DA76FA<<3)+FR);//32λID
	CAN1->sFilterRegister[5].FR1=(((u32)0x18DBFFFA<<3)+FR);//32λID
	CAN1->sFilterRegister[5].FR2=(((u32)0x18DBFFFA<<3)+FR);//32λID
	
#ifdef	ES50I
	
	CAN1->sFilterRegister[4].FR1=(((u32)0x18FE06E3<<3)+FR);//32λID
	CAN1->sFilterRegister[1].FR2=(((u32)0x18FE07E3<<3)+FR);//32λID
	
	CAN1->sFilterRegister[5].FR2=(((u32)0x00000285<<3)+FR);//32λID
	CAN1->sFilterRegister[6].FR1=(((u32)0x00000286<<3)+FR);//32λID
	CAN1->sFilterRegister[6].FR2=(((u32)0x00000287<<3)+FR);//32λID
	CAN1->sFilterRegister[7].FR1=(((u32)0x00000288<<3)+FR);//32λID
	CAN1->sFilterRegister[7].FR2=(((u32)0x00000185<<3)+FR);//32λID
	CAN1->sFilterRegister[8].FR1=(((u32)0x00000186<<3)+FR);//32λID
	CAN1->sFilterRegister[8].FR2=(((u32)0x00000187<<3)+FR);//32λID
	
	CAN1->sFilterRegister[9].FR1=(((u32)0x18FE01E3<<3)+FR);//32λID
	CAN1->sFilterRegister[9].FR2=(((u32)0x18FE02E3<<3)+FR);//32λID	
	CAN1->sFilterRegister[10].FR1=(((u32)0x18FE03E3<<3)+FR);//32λID
	CAN1->sFilterRegister[10].FR2=(((u32)0x18FE04E3<<3)+FR);//32λID	
	CAN1->sFilterRegister[11].FR1=(((u32)0x18FE05E3<<3)+FR);//32λID
	CAN1->sFilterRegister[11].FR2=(((u32)0x18FE08E3<<3)+FR);//32λID	
	CAN1->sFilterRegister[12].FR1=(((u32)0x18FE09E3<<3)+FR);//32λID
	CAN1->sFilterRegister[12].FR2=(((u32)0x18FE10E3<<3)+FR);//32λID	
	CAN1->sFilterRegister[13].FR1=(((u32)0x18FE11E3<<3)+FR);//32λID
	CAN1->sFilterRegister[13].FR2=(((u32)0x18FE12E3<<3)+FR);//32λID	
	
#endif	
	
#ifdef	ES50I
	CAN1->FA1R|=0x00003FFF;	//���������
#else
	CAN1->FA1R|=0x0000003F;	//���������
#endif
	
	
	CAN1->FMR&=0<<0;		//���������������ģʽ

#if CAN_RX0_INT_ENABLE
 	//ʹ���жϽ���
	CAN1->IER|=1<<1;		//FIFO0��Ϣ�Һ��ж�����.
//	CAN1->IER|=1<<8;		//���󾯸��ж�ʹ��.
//	CAN1->IER|=1<<9;		//���󱻶��ж�����.
	CAN1->IER|=1<<10;		//�����ж�����.
//	CAN1->IER|=1<<11;		//�ϴδ�����ж�ʹ��.	
	CAN1->IER|=1<<15;		//�����ж�����.	
	MY_NVIC_Init(8,0,CAN1_SCE_IRQn,4);//��4
	MY_NVIC_Init(8,0,USB_LP_CAN1_RX0_IRQn,4);//��4
#endif


//	FAULT_Inf.BUSOFF = 0;
//	printf("CAN Init\r\n");
	return 0;
}   

//����һ֡���ݣ��ڲ����ã�
//id:��׼ID(11λ)/��չID(11λ+18λ)	    
//ide:0,��׼֡;1,��չ֡
//rtr:0,����֡;1,Զ��֡
//len:Ҫ���͵����ݳ���(�̶�Ϊ8���ֽ�,��ʱ�䴥��ģʽ��,��Ч����Ϊ6���ֽ�)
//*dat:����ָ��.
//����ֵ:0~3,������.0XFF,����Ч����.
u8 CAN_Tx_Msg(u32 id,u8 ide,u8 rtr,u8 len,u8 *dat)
{	   
	u8 mbox;	  
	if(CAN1->TSR&(1<<26))mbox=0;		//����0Ϊ��
//	else if(CAN1->TSR&(1<<27))mbox=1;	//����1Ϊ��
//	else if(CAN1->TSR&(1<<28))mbox=2;	//����2Ϊ��
	else return 0XFF;					//�޿�����,�޷����� 
	CAN1->sTxMailBox[mbox].TIR=0;		//���֮ǰ������
	if(ide==0)	//��׼֡
	{
		id&=0x7ff;//ȡ��11λstdid
		id<<=21;		  
	}else		//��չ֡
	{
		id&=0X1FFFFFFF;//ȡ��32λextid
		id<<=3;									   
	}
	CAN1->sTxMailBox[mbox].TIR|=id;		 
	CAN1->sTxMailBox[mbox].TIR|=ide<<2;	  
	CAN1->sTxMailBox[mbox].TIR|=rtr<<1;
	len&=0X0F;//�õ�����λ
	CAN1->sTxMailBox[mbox].TDTR&=~(0X0000000F);
	CAN1->sTxMailBox[mbox].TDTR|=len;	//����DLC.
	//���������ݴ�������.
	CAN1->sTxMailBox[mbox].TDHR=(((u32)dat[7]<<24)|
								((u32)dat[6]<<16)|
 								((u32)dat[5]<<8)|
								((u32)dat[4]));
	CAN1->sTxMailBox[mbox].TDLR=(((u32)dat[3]<<24)|
								((u32)dat[2]<<16)|
 								((u32)dat[1]<<8)|
								((u32)dat[0]));
	CAN1->sTxMailBox[mbox].TIR|=1<<0; 	//��������������
	return mbox;
}
//��÷���״̬.
//mbox:������;
//����ֵ:����״̬. 0,����;0X05,����ʧ��;0X07,���ͳɹ�.
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
			sta=0X05;//����Ų���,�϶�ʧ��.
		break;
	}
	return sta;
} 
//�õ���FIFO0/FIFO1�н��յ��ı��ĸ���.
//fifox:0/1.FIFO���;
//����ֵ:FIFO0/FIFO1�еı��ĸ���.
u8 CAN_Msg_Pend(u8 fifox)
{
	if(fifox==0)return CAN1->RF0R&0x03; 
	else if(fifox==1)return CAN1->RF1R&0x03; 
	else return 0;
}
//��������
//fifox:�����
//id:��׼ID(11λ)/��չID(11λ+18λ)	    
//ide:0,��׼֡;1,��չ֡
//rtr:0,����֡;1,Զ��֡
//len:���յ������ݳ���(�̶�Ϊ8���ֽ�,��ʱ�䴥��ģʽ��,��Ч����Ϊ6���ֽ�)
//dat:���ݻ�����
void CAN_Rx_Msg(u8 fifox,u32 *id,u8 *ide,u8 *rtr,u8 *len,u8 *dat)
{	   
	*ide=CAN1->sFIFOMailBox[fifox].RIR&0x04;	//�õ���ʶ��ѡ��λ��ֵ  
 	if(*ide==0)//��׼��ʶ��
	{
		*id=CAN1->sFIFOMailBox[fifox].RIR>>21;
	}else	   //��չ��ʶ��
	{
		*id=CAN1->sFIFOMailBox[fifox].RIR>>3;
	}
	*rtr=CAN1->sFIFOMailBox[fifox].RIR&0x02;	//�õ�Զ�̷�������ֵ.
	*len=CAN1->sFIFOMailBox[fifox].RDTR&0x0F;	//�õ�DLC
 	//*fmi=(CAN1->sFIFOMailBox[FIFONumber].RDTR>>8)&0xFF;//�õ�FMI
	//��������
	dat[0]=CAN1->sFIFOMailBox[fifox].RDLR&0XFF;
	dat[1]=(CAN1->sFIFOMailBox[fifox].RDLR>>8)&0XFF;
	dat[2]=(CAN1->sFIFOMailBox[fifox].RDLR>>16)&0XFF;
	dat[3]=(CAN1->sFIFOMailBox[fifox].RDLR>>24)&0XFF;    
	dat[4]=CAN1->sFIFOMailBox[fifox].RDHR&0XFF;
	dat[5]=(CAN1->sFIFOMailBox[fifox].RDHR>>8)&0XFF;
	dat[6]=(CAN1->sFIFOMailBox[fifox].RDHR>>16)&0XFF;
	dat[7]=(CAN1->sFIFOMailBox[fifox].RDHR>>24)&0XFF;    
  	if(fifox==0)CAN1->RF0R|=0X20;//�ͷ�FIFO0����
	else if(fifox==1)CAN1->RF1R|=0X20;//�ͷ�FIFO1����	 
}

#if CAN_RX0_INT_ENABLE	//ʹ��RX0�ж�
//�жϷ�����	
void Can_Write_RXMsg(Can_SF RX_Msg);//�򻺳������һ֡����

//CAN�жϴ�������
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	u8 ide,rtr,len;  
	Can_SF rx_data;
	
	CAN_Rx_Msg(0,&rx_data.id,&ide,&rtr,&len,rx_data.data);
	
	//�����ⲿ�ӿڣ��������ݣ�ע�⣺�˴�Ϊ�ж��ڣ�����ʱ���ȴ�ѭ��	
	Can_Write_RXMsg(rx_data);//���յ������ݷ��뻺����

	CAN1->RF0R|=0X20;//�ͷ�FIFO0����
	CAN1->RF1R|=0X20;//�ͷ�FIFO1����	
	
#if	CAN_DEBUG	//����ʱ��ѡ��Ӵ��ڴ�ӡ���յ��ı���

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
		//����
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


//can����һ������
//����ֵ:0,�ɹ�;
//		 ����,ʧ��;
static u16 send_err_cnt = 0;
u8 CAN_Send_Msg(Can_SF TX_Msg)
{	
	u8 mbox;
	u16 i=0;	  	 						       
	  mbox=CAN_Tx_Msg(TX_Msg.id,Can_Ide,Can_Rtr,Can_Msg,TX_Msg.data);
	while((CAN_Tx_Staus(mbox)!=0X07)&&(i<0XFFF))i++;//�ȴ����ͽ���
	if(i>=0XFFF)//����ʧ��?
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
	FAULT_Inf.BUSOFF = 0;		//���ͳɹ������߻ָ�
	FAULT_Inf.BUSOFF_Flag = 0;
//	FAULT_Inf.CAN_OFF = 0;
	return 0;					//���ͳɹ�;
}
//can�ڽ������ݲ�ѯ
//buf:���ݻ�����;	 
//����ֵ:0,�����ݱ��յ�;
//		 ����,���յ����ݳ���;
u8 CAN_Receive_Msg(u8 *buf)
{		   		   
	u32 id;
	u8 ide,rtr,len; 
	if(CAN_Msg_Pend(0)==0)return 0;			//û�н��յ�����,ֱ���˳� 	 
  	CAN_Rx_Msg(0,&id,&ide,&rtr,&len,buf); 	//��ȡ����
    if(id!=0x12||ide!=0||rtr!=0)len=0;		//���մ���	   
	return len;	
}



CAN_CACHE CAN_RX;

//�򻺴������һ֡����
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

//�ӻ�����ȡ��һ֡���ݣ�
//����ֵ��	0��û������
//			1���õ�һ֡����
u8 Can_Get_RXMsg(Can_SF *RX_Msg)
{
	if(CAN_RX.writpointer==CAN_RX.readpointer) return 0;//�������գ�û������
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





