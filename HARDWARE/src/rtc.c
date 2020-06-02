#include "delay.h"
#include "usart.h"
#include "rtc.h" 		    

#define	LSI_clksrc	1	//�ڲ�����ʱ��
#define	LSE_clksrc	2	//�ⲿ����ʱ��

#define	RTC_clksrc	LSI_clksrc


_calendar_obj calendar={8,0,0,2018,8,8,3,3,0};//ʱ�ӽṹ�� 	
static u32	timecount=1533686400;
volatile u8	RTC_IRQflag = 0;

//ʵʱʱ������
//��ʼ��RTCʱ��,ͬʱ���ʱ���Ƿ�������
//BKP->DR1���ڱ����Ƿ��һ�����õ�����
//����0:����
//����:�������
u8 get_sta = 0;


//��Ƭ���������ʼ��RTC
u8 RTC_Init4(void)
{
	static u8 ss = 0;
	u8 temp = 0;
	u8 mode = 0;
	
//	INTX_DISABLE();
	
	if(ss == 0)
	{
		RCC->APB1ENR|=1<<28;     //ʹ�ܵ�Դʱ��
		RCC->APB1ENR|=1<<27;     //ʹ�ܱ���ʱ��
		PWR->CR|=1<<8;           //ȡ��������д����
		RCC->BDCR|=1<<16;        //����������λ
		RCC->BDCR&=~(1<<16);     //����������λ����
		RCC->BDCR|=1<<0;         //�����ⲿ��������
		
		ss = 1;
	}
	while((!(RCC->BDCR&0X02))&&temp<250)//�ȴ��ⲿʱ�Ӿ���	 
	{
		temp++;
//		delay_ms(10);
	}
	
	if(temp>=250)return 1;//��ʼ��ʱ��ʧ��,����������
	
//	if(temp>=250)//��ʼ��ʱ��ʧ��,����������
//	{
//		temp = 0;
//		RCC->CSR|=1<<0;         //�����ڲ��������� 
//		while((!(RCC->CSR&0X02))&&temp<250)//�ȴ��ڲ�ʱ�Ӿ���	 
//		{
//			temp++;
//			delay_ms(10);
//		};
//		if(temp>=250)return 1;	//��ʼ��ʱ��ʧ��,
//		mode = 1;
//	}
	
	if(0 == mode)	RCC->BDCR|=1<<8;	//LSE��ΪRTCʱ��	
	else			RCC->BDCR|=2<<8;	//LSI��ΪRTCʱ��

	RCC->BDCR|=1<<15;		//RTCʱ��ʹ��	  
	while(!(RTC->CRL&(1<<5)));//�ȴ�RTC�Ĵ����������	 
	while(!(RTC->CRL&(1<<3)));//�ȴ�RTC�Ĵ���ͬ��  
	RTC->CRH|=0X01;  		  //�������ж�
//	RTC->CRH|=0X02;  		  //���������ж�
	while(!(RTC->CRL&(1<<5)));//�ȴ�RTC�Ĵ����������	 
	RTC->CRL|=1<<4;           //��������	  
	RTC->PRLH=0X0000;
	
	if(0 == mode)	RTC->PRLL=32767;			//ʱ����������(�д��۲�,���Ƿ�������?)����ֵ��32767	
	else			RTC->PRLL=40893;			//ʱ����������(�д��۲�,���Ƿ�������?)����ֵ��39999	
	
	RTC_Set(2018,8,8,9,0,0);	//����ʱ��	��Ŀǰʹ�ã�
//	RTC_Set((u16)YEAR,(u8)MONTH,(u8)DAY,(u8)HOUR,(u8)MINUTE,(u8)SECOND);	//����ʱ�䣨�������ʱ�䣩
	
	RTC->CRL&=~(1<<4);				//���ø���
	while(!(RTC->CRL&(1<<5)));		//�ȴ�RTC�Ĵ����������	
	
	MY_NVIC_Init(0,0,RTC_IRQn,4);	//���ȼ�����   

//	printf("RTC mode : %d\r\n",mode);
	get_sta = 1;
//	INTX_ENABLE();
	RTC_Set(2018,8,8,8,0,0);	//����ʱ��	��Ŀǰʹ�ã�
	return 0; //ok
}




u8 RTC_Init3(void)
{
	//����ǲ��ǵ�һ������ʱ��
	u8 temp = 0;
	u8 mode = 0;
	
	if(BKP->DR1==0X5050)//�ⲿ�������
	{
    	while(!(RTC->CRL&(1<<3)));	//�ȴ�RTC�Ĵ���ͬ��  
    	RTC->CRH|=0X01;				//�������ж�
    	while(!(RTC->CRL&(1<<5)));	//�ȴ�RTC�Ĵ����������
//		printf("clk=%d\r\n",0);
	}
	else if(BKP->DR1==0X5051)//�ڲ�����ʱ�Ӽ�������
	{
	  	RCC->APB1ENR|=1<<28;     //ʹ�ܵ�Դʱ��	    
		RCC->APB1ENR|=1<<27;     //ʹ�ܱ���ʱ��
		PWR->CR|=1<<8;           //ȡ��������д����
	    RCC->CSR|=1<<0;         //�����ڲ��������� 
	    while((!(RCC->CSR&0X02))&&temp<250)//�ȴ��ڲ�ʱ�Ӿ���	 
		{
			temp++;
			delay_ms(10);
		};
		if(temp>=250)return 1;	//��ʼ��ʱ��ʧ��,����������	
		RCC->BDCR|=2<<8;		//LSI��ΪRTCʱ��
		RCC->BDCR|=1<<15;		//RTCʱ��ʹ��	  
		while(!(RTC->CRL&(1<<5)));//�ȴ�RTC�Ĵ����������	 
		while(!(RTC->CRL&(1<<3)));//�ȴ�RTC�Ĵ���ͬ��  
		RTC->CRH|=0X01;  		  //�������ж�
		while(!(RTC->CRL&(1<<5)));//�ȴ�RTC�Ĵ����������
//		printf("clk=%d\r\n",1);
	}
	else //�����磬��������RTC
	{
	  	RCC->APB1ENR|=1<<28;     //ʹ�ܵ�Դʱ��	    
		RCC->APB1ENR|=1<<27;     //ʹ�ܱ���ʱ��	    
		PWR->CR|=1<<8;           //ȡ��������д����
		RCC->BDCR|=1<<16;        //����������λ	   
		RCC->BDCR&=~(1<<16);     //����������λ����
		
	    RCC->BDCR|=1<<0;         //�����ⲿ�������� 
	    while((!(RCC->BDCR&0X02))&&temp<250)//�ȴ��ⲿʱ�Ӿ���	 
		{
			temp++;
			delay_ms(10);
		};
//		if(temp>=250)return 1;//��ʼ��ʱ��ʧ��,����������
		if(temp>=250)//��ʼ��ʱ��ʧ��,����������
		{
			temp = 0;
			RCC->CSR|=1<<0;         //�����ڲ��������� 
			while((!(RCC->CSR&0X02))&&temp<250)//�ȴ��ڲ�ʱ�Ӿ���	 
			{
				temp++;
				delay_ms(10);
			};
			if(temp>=250)return 1;	//��ʼ��ʱ��ʧ��,
			mode = 1;
		}
		
	
		if(0 == mode)	RCC->BDCR|=1<<8;	//LSE��ΪRTCʱ��	
		else			RCC->BDCR|=2<<8;	//LSI��ΪRTCʱ��
		
		
		RCC->BDCR|=1<<15;		//RTCʱ��ʹ��	  
		while(!(RTC->CRL&(1<<5)));//�ȴ�RTC�Ĵ����������	 
		while(!(RTC->CRL&(1<<3)));//�ȴ�RTC�Ĵ���ͬ��  
		RTC->CRH|=0X01;  		  //�������ж�
//		RTC->CRH|=0X02;  		  //���������ж�
		while(!(RTC->CRL&(1<<5)));//�ȴ�RTC�Ĵ����������	 
		RTC->CRL|=1<<4;           //��������	  
		RTC->PRLH=0X0000;
		
		if(0 == mode)	RTC->PRLL=32767;			//ʱ����������(�д��۲�,���Ƿ�������?)����ֵ��32767	
		else			RTC->PRLL=40893;			//ʱ����������(�д��۲�,���Ƿ�������?)����ֵ��39999	

//		RTC_Set(2019,1,29,9,30,00);	//����ʱ��	��Ŀǰʹ�ã�
		RTC_Set(YEAR,MONTH,DAY,HOUR,MINUTE,SECOND);	//����ʱ�䣨�������ʱ�䣩
		
		RTC->CRL&=~(1<<4);				//���ø���
		while(!(RTC->CRL&(1<<5)));		//�ȴ�RTC�Ĵ����������	
		
		if(0 == mode)	BKP->DR1=0X5050;  
		else			BKP->DR1=0X5051;
//		printf("first time ; clk=%d\r\n",mode);
	}

	MY_NVIC_Init(0,0,RTC_IRQn,4);	//���ȼ�����    
//	RTC_Get();//����ʱ�� 
	return 0; //ok
}




u8 RTC_Init(void)
{
	//����ǲ��ǵ�һ������ʱ��
	u8 temp=0;
	
#if (RTC_clksrc==LSE_clksrc)	
	if(BKP->DR1!=0X5050)//��һ������
	{
#endif
	  	RCC->APB1ENR|=1<<28;     //ʹ�ܵ�Դʱ��	    
		RCC->APB1ENR|=1<<27;     //ʹ�ܱ���ʱ��	    
		PWR->CR|=1<<8;           //ȡ��������д����
		RCC->BDCR|=1<<16;        //����������λ	   
		RCC->BDCR&=~(1<<16);     //����������λ����	 
	
#if (RTC_clksrc==LSE_clksrc)		
	    RCC->BDCR|=1<<0;         //�����ⲿ�������� 
	    while((!(RCC->BDCR&0X02))&&temp<250)//�ȴ��ⲿʱ�Ӿ���	 
		{
			temp++;
			delay_ms(10);
		};
		if(temp>=250)return 1;	//��ʼ��ʱ��ʧ��,����������	   
		RCC->BDCR|=1<<8;		//LSE��ΪRTCʱ��
#endif	
		
#if (RTC_clksrc==LSI_clksrc)	
	    RCC->CSR|=1<<0;         //�����ڲ��������� 
	    while((!(RCC->CSR&0X02))&&temp<250)//�ȴ��ڲ�ʱ�Ӿ���	 
		{
			temp++;
			delay_ms(10);
		};
		if(temp>=250)return 1;	//��ʼ��ʱ��ʧ��,����������
		RCC->BDCR|=2<<8;		//LSI��ΪRTCʱ��		
#endif
		
		RCC->BDCR|=1<<15;		//RTCʱ��ʹ��	  
		while(!(RTC->CRL&(1<<5)));//�ȴ�RTC�Ĵ����������	 
		while(!(RTC->CRL&(1<<3)));//�ȴ�RTC�Ĵ���ͬ��  
		RTC->CRH|=0X01;  		  //�������ж�
//		RTC->CRH|=0X02;  		  //���������ж�
		while(!(RTC->CRL&(1<<5)));//�ȴ�RTC�Ĵ����������	 
		RTC->CRL|=1<<4;           //��������	 
		RTC->PRLH=0X0000;
#if (RTC_clksrc==LSE_clksrc)	
		RTC->PRLL=32767;			//ʱ����������(�д��۲�,���Ƿ�������?)����ֵ��32767	
#endif
#if (RTC_clksrc==LSI_clksrc)		
		RTC->PRLL=40893;			//ʱ����������(�д��۲�,���Ƿ�������?)����ֵ��39999	
#endif	
		RTC_Set(2019,1,8,17,35,00);	//����ʱ��	  
		RTC->CRL&=~(1<<4);				//���ø���
		while(!(RTC->CRL&(1<<5)));		//�ȴ�RTC�Ĵ����������		 									  
		BKP->DR1=0X5050;  
//	 	printf("FIRST TIME\n");
#if (RTC_clksrc==LSE_clksrc)
	}
	else//ϵͳ������ʱ
	{
    	while(!(RTC->CRL&(1<<3)));	//�ȴ�RTC�Ĵ���ͬ��  
    	RTC->CRH|=0X01;				//�������ж�
    	while(!(RTC->CRL&(1<<5)));	//�ȴ�RTC�Ĵ����������
	}
#endif	
	MY_NVIC_Init(0,0,RTC_IRQn,4);	//���ȼ�����    
//	RTC_Get();//����ʱ�� 
	return 0; //ok
}		 				    
//RTCʱ���ж�
//ÿ�봥��һ��  	 
void RTC_IRQHandler(void)
{		 
	if(RTC->CRL&0x0001)			//�����ж�
	{				
		RTCcn_Get();			//��ȡ����ֵ
//		RTC_Get();				//����ʱ��   
		//printf("sec:%d\r\n",calendar.sec);
 	}
	if(RTC->CRL&0x0002)			//�����ж�
	{
		RTC->CRL&=~(0x0002);	//�������ж�	  
//		RTC_Get();				//����ʱ��   
//  	printf("Alarm Time:%d-%d-%d %d:%d:%d\n",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);//�������ʱ��	   
  	}
    RTC->CRL&=0X0FFA;         	//�������������жϱ�־
//	while(!(RTC->CRL&(1<<5)));	//�ȴ�RTC�Ĵ����������	  	    						 	   	 
}


//�ж��Ƿ������꺯��
//�·�   1  2  3  4  5  6  7  8  9  10 11 12
//����   31 29 31 30 31 30 31 31 30 31 30 31
//������ 31 28 31 30 31 30 31 31 30 31 30 31
//year:���
//����ֵ:������ǲ�������.1,��.0,����
u8 Is_Leap_Year(u16 year)
{			  
	if(year%4==0) //�����ܱ�4����
	{ 
		if(year%100==0) 
		{ 
			if(year%400==0)return 1;//�����00��β,��Ҫ�ܱ�400���� 	   
			else return 0;   
		}else return 1;   
	}else return 0;	
}	 			   
//����ʱ��(2^32 = 4,294,967,295)
//�������ʱ��ת��Ϊ����
//��1970��1��1��Ϊ��׼
//1970~2099��Ϊ�Ϸ����
//����ֵ:0,�ɹ�;����:�������.
//�·����ݱ�											 
u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //���������ݱ�	  
//ƽ����·����ڱ�
const u8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};
//syear,smon,sday,hour,min,sec��������ʱ����
//����ֵ�����ý����0���ɹ���1��ʧ�ܡ�
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec)
{
	u16 t;
	u32 seccount=0;
	
	if(get_sta == 0)return 0;
	
	if(syear<1970||syear>2099)return 1;
	for(t=1970;t<syear;t++)	//��������ݵ��������
	{
		if(Is_Leap_Year(t))seccount+=31622400;//�����������
		else seccount+=31536000;			  //ƽ���������
	}
	smon-=1;
	for(t=0;t<smon;t++)	   //��ǰ���·ݵ����������
	{
		seccount+=(u32)mon_table[t]*86400;//�·����������
		if(Is_Leap_Year(syear)&&t==1)seccount+=86400;//����2�·�����һ���������	   
	}
	seccount+=(u32)(sday-1)*86400;//��ǰ�����ڵ���������� 
	seccount+=(u32)hour*3600;//Сʱ������
    seccount+=(u32)min*60;	 //����������
	seccount+=sec;//�������Ӽ���ȥ
													    
	//����ʱ��
    RCC->APB1ENR|=1<<28;//ʹ�ܵ�Դʱ��
    RCC->APB1ENR|=1<<27;//ʹ�ܱ���ʱ��
	PWR->CR|=1<<8;    //ȡ��������д����
	//���������Ǳ����!
	RTC->CRL|=1<<4;   //�������� 
	RTC->CNTL=seccount&0xffff;
	RTC->CNTH=seccount>>16;
	RTC->CRL&=~(1<<4);					//���ø���
	while(!(RTC->CRL&(1<<5)));	//�ȴ�RTC�Ĵ���������� 
//	RTC_Get();//������֮�����һ������ 	
	return 0;	    
}
//��ʼ������		  
//��1970��1��1��Ϊ��׼
//1970~2099��Ϊ�Ϸ����
//syear,smon,sday,hour,min,sec�����ӵ�������ʱ����   
//����ֵ:0,�ɹ�;����:�������.
u8 RTC_Alarm_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec)
{
	u16 t;
	u32 seccount=0;
	if(syear<1970||syear>2099)return 1;	   
	for(t=1970;t<syear;t++)									//��������ݵ��������
	{
		if(Is_Leap_Year(t))seccount+=31622400;//�����������
		else seccount+=31536000;							//ƽ���������
	}
	smon-=1;
	for(t=0;t<smon;t++)											//��ǰ���·ݵ����������
	{
		seccount+=(u32)mon_table[t]*86400;		//�·����������
		if(Is_Leap_Year(syear)&&t==1)seccount+=86400;//����2�·�����һ���������	   
	}
	seccount+=(u32)(sday-1)*86400;//��ǰ�����ڵ���������� 
	seccount+=(u32)hour*3600;			//Сʱ������
	seccount+=(u32)min*60;				//����������
	seccount+=sec;								//�������Ӽ���ȥ 			    
	//����ʱ��
	RCC->APB1ENR|=1<<28;					//ʹ�ܵ�Դʱ��
	RCC->APB1ENR|=1<<27;					//ʹ�ܱ���ʱ��
	PWR->CR|=1<<8;								//ȡ��������д����
	//���������Ǳ����!
	RTC->CRL|=1<<4;								//�������� 
	RTC->ALRL=seccount&0xffff;
	RTC->ALRH=seccount>>16;
	RTC->CRL&=~(1<<4);						//���ø���
	while(!(RTC->CRL&(1<<5)));		//�ȴ�RTC�Ĵ����������  
	return 0;	    
}

u8 RTCcn_Get(void)
{
	if(get_sta == 0)return 1;
	timecount=RTC->CNTH;	//�õ��������е�ֵ(������)
	timecount<<=16;
	timecount+=RTC->CNTL;
	RTC_IRQflag = 1;		//���ʱ�����
	calendar.flag = 0;
	return 0;
}

u32 Get_RTCcont(void)
{
	return timecount;
}

//�õ���ǰ��ʱ�䣬���������calendar�ṹ������
//����ֵ:0,�ɹ�;����:�������.
u8 RTC_Get(void)
{
	static u16 daycnt=0;

	u32 temp=0;
	u16 temp1=0;
	
//	RTCcn_Get();
	if(get_sta == 0)return 0;
	
 	temp=timecount/86400;					//�õ�����(��������Ӧ��
	calendar.week2 = (temp+4)%7;
	if(daycnt!=temp)						//����һ����
	{	  
		daycnt=temp;
		temp1=1970;							//��1970�꿪ʼ
		while(temp>=365)
		{				 
			if(Is_Leap_Year(temp1))			//������
			{
				if(temp>=366)temp-=366;		//�����������
				else break;  
			}
			else temp-=365;					//ƽ�� 
			temp1++;  
		}   
		calendar.w_year=temp1;				//�õ����
		temp1=0;
		while(temp>=28)						//������һ����
		{
			if(Is_Leap_Year(calendar.w_year)&&temp1==1)	//�����ǲ�������/2�·�
			{
				if(temp>=29)temp-=29;					//�����������
				else break; 
			}
			else 
			{
				if(temp>=mon_table[temp1])temp-=mon_table[temp1];//ƽ��
				else break;
			}
			temp1++;  
		}
		calendar.w_month=temp1+1;			//�õ��·�
		calendar.w_date=temp+1;				//�õ����� 
	}
	temp=timecount%86400;					//�õ�������   	   
	calendar.hour=temp/3600;				//Сʱ
	calendar.min=(temp%3600)/60;			//����	
	calendar.sec=(temp%3600)%60;			//����
	calendar.week=RTC_Get_Week(calendar.w_year,calendar.w_month,calendar.w_date);	//��ȡ����   
	return 0;
}	 
//������������ڼ�
//��������:���빫�����ڵõ�����(ֻ����1901-2099��)
//year,month,day������������ 
//����ֵ�����ں�																						 
u8 RTC_Get_Week(u16 year,u8 month,u8 day)
{	
	u16 temp2;
	u8 yearH,yearL;
	
	yearH=year/100;	yearL=year%100; 
	// ���Ϊ21����,�������100  
	if (yearH>19)yearL+=100;
	// ����������ֻ��1900��֮���  
	temp2=yearL+yearL/4;
	temp2=temp2%7; 
	temp2=temp2+day+table_week[month-1];
	if (yearL%4==0&&month<3)temp2--;
	return(temp2%7);
}			  
















