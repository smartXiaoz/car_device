#include "delay.h"
#include "usart.h"
#include "rtc.h" 		    

#define	LSI_clksrc	1	//内部低速时钟
#define	LSE_clksrc	2	//外部低速时钟

#define	RTC_clksrc	LSI_clksrc


_calendar_obj calendar={8,0,0,2018,8,8,3,3,0};//时钟结构体 	
static u32	timecount=1533686400;
volatile u8	RTC_IRQflag = 0;

//实时时钟配置
//初始化RTC时钟,同时检测时钟是否工作正常
//BKP->DR1用于保存是否第一次配置的设置
//返回0:正常
//其他:错误代码
u8 get_sta = 0;


//单片机重启后初始化RTC
u8 RTC_Init4(void)
{
	static u8 ss = 0;
	u8 temp = 0;
	u8 mode = 0;
	
//	INTX_DISABLE();
	
	if(ss == 0)
	{
		RCC->APB1ENR|=1<<28;     //使能电源时钟
		RCC->APB1ENR|=1<<27;     //使能备份时钟
		PWR->CR|=1<<8;           //取消备份区写保护
		RCC->BDCR|=1<<16;        //备份区域软复位
		RCC->BDCR&=~(1<<16);     //备份区域软复位结束
		RCC->BDCR|=1<<0;         //开启外部低速振荡器
		
		ss = 1;
	}
	while((!(RCC->BDCR&0X02))&&temp<250)//等待外部时钟就绪	 
	{
		temp++;
//		delay_ms(10);
	}
	
	if(temp>=250)return 1;//初始化时钟失败,晶振有问题
	
//	if(temp>=250)//初始化时钟失败,晶振有问题
//	{
//		temp = 0;
//		RCC->CSR|=1<<0;         //开启内部低速振荡器 
//		while((!(RCC->CSR&0X02))&&temp<250)//等待内部时钟就绪	 
//		{
//			temp++;
//			delay_ms(10);
//		};
//		if(temp>=250)return 1;	//初始化时钟失败,
//		mode = 1;
//	}
	
	if(0 == mode)	RCC->BDCR|=1<<8;	//LSE作为RTC时钟	
	else			RCC->BDCR|=2<<8;	//LSI作为RTC时钟

	RCC->BDCR|=1<<15;		//RTC时钟使能	  
	while(!(RTC->CRL&(1<<5)));//等待RTC寄存器操作完成	 
	while(!(RTC->CRL&(1<<3)));//等待RTC寄存器同步  
	RTC->CRH|=0X01;  		  //允许秒中断
//	RTC->CRH|=0X02;  		  //允许闹钟中断
	while(!(RTC->CRL&(1<<5)));//等待RTC寄存器操作完成	 
	RTC->CRL|=1<<4;           //允许配置	  
	RTC->PRLH=0X0000;
	
	if(0 == mode)	RTC->PRLL=32767;			//时钟周期设置(有待观察,看是否跑慢了?)理论值：32767	
	else			RTC->PRLL=40893;			//时钟周期设置(有待观察,看是否跑慢了?)理论值：39999	
	
	RTC_Set(2018,8,8,9,0,0);	//设置时间	（目前使用）
//	RTC_Set((u16)YEAR,(u8)MONTH,(u8)DAY,(u8)HOUR,(u8)MINUTE,(u8)SECOND);	//设置时间（程序编译时间）
	
	RTC->CRL&=~(1<<4);				//配置更新
	while(!(RTC->CRL&(1<<5)));		//等待RTC寄存器操作完成	
	
	MY_NVIC_Init(0,0,RTC_IRQn,4);	//优先级设置   

//	printf("RTC mode : %d\r\n",mode);
	get_sta = 1;
//	INTX_ENABLE();
	RTC_Set(2018,8,8,8,0,0);	//设置时间	（目前使用）
	return 0; //ok
}




u8 RTC_Init3(void)
{
	//检查是不是第一次配置时钟
	u8 temp = 0;
	u8 mode = 0;
	
	if(BKP->DR1==0X5050)//外部晶振继续
	{
    	while(!(RTC->CRL&(1<<3)));	//等待RTC寄存器同步  
    	RTC->CRH|=0X01;				//允许秒中断
    	while(!(RTC->CRL&(1<<5)));	//等待RTC寄存器操作完成
//		printf("clk=%d\r\n",0);
	}
	else if(BKP->DR1==0X5051)//内部低速时钟继续继续
	{
	  	RCC->APB1ENR|=1<<28;     //使能电源时钟	    
		RCC->APB1ENR|=1<<27;     //使能备份时钟
		PWR->CR|=1<<8;           //取消备份区写保护
	    RCC->CSR|=1<<0;         //开启内部低速振荡器 
	    while((!(RCC->CSR&0X02))&&temp<250)//等待内部时钟就绪	 
		{
			temp++;
			delay_ms(10);
		};
		if(temp>=250)return 1;	//初始化时钟失败,晶振有问题	
		RCC->BDCR|=2<<8;		//LSI作为RTC时钟
		RCC->BDCR|=1<<15;		//RTC时钟使能	  
		while(!(RTC->CRL&(1<<5)));//等待RTC寄存器操作完成	 
		while(!(RTC->CRL&(1<<3)));//等待RTC寄存器同步  
		RTC->CRH|=0X01;  		  //允许秒中断
		while(!(RTC->CRL&(1<<5)));//等待RTC寄存器操作完成
//		printf("clk=%d\r\n",1);
	}
	else //掉过电，重新设置RTC
	{
	  	RCC->APB1ENR|=1<<28;     //使能电源时钟	    
		RCC->APB1ENR|=1<<27;     //使能备份时钟	    
		PWR->CR|=1<<8;           //取消备份区写保护
		RCC->BDCR|=1<<16;        //备份区域软复位	   
		RCC->BDCR&=~(1<<16);     //备份区域软复位结束
		
	    RCC->BDCR|=1<<0;         //开启外部低速振荡器 
	    while((!(RCC->BDCR&0X02))&&temp<250)//等待外部时钟就绪	 
		{
			temp++;
			delay_ms(10);
		};
//		if(temp>=250)return 1;//初始化时钟失败,晶振有问题
		if(temp>=250)//初始化时钟失败,晶振有问题
		{
			temp = 0;
			RCC->CSR|=1<<0;         //开启内部低速振荡器 
			while((!(RCC->CSR&0X02))&&temp<250)//等待内部时钟就绪	 
			{
				temp++;
				delay_ms(10);
			};
			if(temp>=250)return 1;	//初始化时钟失败,
			mode = 1;
		}
		
	
		if(0 == mode)	RCC->BDCR|=1<<8;	//LSE作为RTC时钟	
		else			RCC->BDCR|=2<<8;	//LSI作为RTC时钟
		
		
		RCC->BDCR|=1<<15;		//RTC时钟使能	  
		while(!(RTC->CRL&(1<<5)));//等待RTC寄存器操作完成	 
		while(!(RTC->CRL&(1<<3)));//等待RTC寄存器同步  
		RTC->CRH|=0X01;  		  //允许秒中断
//		RTC->CRH|=0X02;  		  //允许闹钟中断
		while(!(RTC->CRL&(1<<5)));//等待RTC寄存器操作完成	 
		RTC->CRL|=1<<4;           //允许配置	  
		RTC->PRLH=0X0000;
		
		if(0 == mode)	RTC->PRLL=32767;			//时钟周期设置(有待观察,看是否跑慢了?)理论值：32767	
		else			RTC->PRLL=40893;			//时钟周期设置(有待观察,看是否跑慢了?)理论值：39999	

//		RTC_Set(2019,1,29,9,30,00);	//设置时间	（目前使用）
		RTC_Set(YEAR,MONTH,DAY,HOUR,MINUTE,SECOND);	//设置时间（程序编译时间）
		
		RTC->CRL&=~(1<<4);				//配置更新
		while(!(RTC->CRL&(1<<5)));		//等待RTC寄存器操作完成	
		
		if(0 == mode)	BKP->DR1=0X5050;  
		else			BKP->DR1=0X5051;
//		printf("first time ; clk=%d\r\n",mode);
	}

	MY_NVIC_Init(0,0,RTC_IRQn,4);	//优先级设置    
//	RTC_Get();//更新时间 
	return 0; //ok
}




u8 RTC_Init(void)
{
	//检查是不是第一次配置时钟
	u8 temp=0;
	
#if (RTC_clksrc==LSE_clksrc)	
	if(BKP->DR1!=0X5050)//第一次配置
	{
#endif
	  	RCC->APB1ENR|=1<<28;     //使能电源时钟	    
		RCC->APB1ENR|=1<<27;     //使能备份时钟	    
		PWR->CR|=1<<8;           //取消备份区写保护
		RCC->BDCR|=1<<16;        //备份区域软复位	   
		RCC->BDCR&=~(1<<16);     //备份区域软复位结束	 
	
#if (RTC_clksrc==LSE_clksrc)		
	    RCC->BDCR|=1<<0;         //开启外部低速振荡器 
	    while((!(RCC->BDCR&0X02))&&temp<250)//等待外部时钟就绪	 
		{
			temp++;
			delay_ms(10);
		};
		if(temp>=250)return 1;	//初始化时钟失败,晶振有问题	   
		RCC->BDCR|=1<<8;		//LSE作为RTC时钟
#endif	
		
#if (RTC_clksrc==LSI_clksrc)	
	    RCC->CSR|=1<<0;         //开启内部低速振荡器 
	    while((!(RCC->CSR&0X02))&&temp<250)//等待内部时钟就绪	 
		{
			temp++;
			delay_ms(10);
		};
		if(temp>=250)return 1;	//初始化时钟失败,晶振有问题
		RCC->BDCR|=2<<8;		//LSI作为RTC时钟		
#endif
		
		RCC->BDCR|=1<<15;		//RTC时钟使能	  
		while(!(RTC->CRL&(1<<5)));//等待RTC寄存器操作完成	 
		while(!(RTC->CRL&(1<<3)));//等待RTC寄存器同步  
		RTC->CRH|=0X01;  		  //允许秒中断
//		RTC->CRH|=0X02;  		  //允许闹钟中断
		while(!(RTC->CRL&(1<<5)));//等待RTC寄存器操作完成	 
		RTC->CRL|=1<<4;           //允许配置	 
		RTC->PRLH=0X0000;
#if (RTC_clksrc==LSE_clksrc)	
		RTC->PRLL=32767;			//时钟周期设置(有待观察,看是否跑慢了?)理论值：32767	
#endif
#if (RTC_clksrc==LSI_clksrc)		
		RTC->PRLL=40893;			//时钟周期设置(有待观察,看是否跑慢了?)理论值：39999	
#endif	
		RTC_Set(2019,1,8,17,35,00);	//设置时间	  
		RTC->CRL&=~(1<<4);				//配置更新
		while(!(RTC->CRL&(1<<5)));		//等待RTC寄存器操作完成		 									  
		BKP->DR1=0X5050;  
//	 	printf("FIRST TIME\n");
#if (RTC_clksrc==LSE_clksrc)
	}
	else//系统继续计时
	{
    	while(!(RTC->CRL&(1<<3)));	//等待RTC寄存器同步  
    	RTC->CRH|=0X01;				//允许秒中断
    	while(!(RTC->CRL&(1<<5)));	//等待RTC寄存器操作完成
	}
#endif	
	MY_NVIC_Init(0,0,RTC_IRQn,4);	//优先级设置    
//	RTC_Get();//更新时间 
	return 0; //ok
}		 				    
//RTC时钟中断
//每秒触发一次  	 
void RTC_IRQHandler(void)
{		 
	if(RTC->CRL&0x0001)			//秒钟中断
	{				
		RTCcn_Get();			//获取计数值
//		RTC_Get();				//更新时间   
		//printf("sec:%d\r\n",calendar.sec);
 	}
	if(RTC->CRL&0x0002)			//闹钟中断
	{
		RTC->CRL&=~(0x0002);	//清闹钟中断	  
//		RTC_Get();				//更新时间   
//  	printf("Alarm Time:%d-%d-%d %d:%d:%d\n",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);//输出闹铃时间	   
  	}
    RTC->CRL&=0X0FFA;         	//清除溢出，秒钟中断标志
//	while(!(RTC->CRL&(1<<5)));	//等待RTC寄存器操作完成	  	    						 	   	 
}


//判断是否是闰年函数
//月份   1  2  3  4  5  6  7  8  9  10 11 12
//闰年   31 29 31 30 31 30 31 31 30 31 30 31
//非闰年 31 28 31 30 31 30 31 31 30 31 30 31
//year:年份
//返回值:该年份是不是闰年.1,是.0,不是
u8 Is_Leap_Year(u16 year)
{			  
	if(year%4==0) //必须能被4整除
	{ 
		if(year%100==0) 
		{ 
			if(year%400==0)return 1;//如果以00结尾,还要能被400整除 	   
			else return 0;   
		}else return 1;   
	}else return 0;	
}	 			   
//设置时钟(2^32 = 4,294,967,295)
//把输入的时钟转换为秒钟
//以1970年1月1日为基准
//1970~2099年为合法年份
//返回值:0,成功;其他:错误代码.
//月份数据表											 
u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //月修正数据表	  
//平年的月份日期表
const u8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};
//syear,smon,sday,hour,min,sec：年月日时分秒
//返回值：设置结果。0，成功；1，失败。
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec)
{
	u16 t;
	u32 seccount=0;
	
	if(get_sta == 0)return 0;
	
	if(syear<1970||syear>2099)return 1;
	for(t=1970;t<syear;t++)	//把所有年份的秒钟相加
	{
		if(Is_Leap_Year(t))seccount+=31622400;//闰年的秒钟数
		else seccount+=31536000;			  //平年的秒钟数
	}
	smon-=1;
	for(t=0;t<smon;t++)	   //把前面月份的秒钟数相加
	{
		seccount+=(u32)mon_table[t]*86400;//月份秒钟数相加
		if(Is_Leap_Year(syear)&&t==1)seccount+=86400;//闰年2月份增加一天的秒钟数	   
	}
	seccount+=(u32)(sday-1)*86400;//把前面日期的秒钟数相加 
	seccount+=(u32)hour*3600;//小时秒钟数
    seccount+=(u32)min*60;	 //分钟秒钟数
	seccount+=sec;//最后的秒钟加上去
													    
	//设置时钟
    RCC->APB1ENR|=1<<28;//使能电源时钟
    RCC->APB1ENR|=1<<27;//使能备份时钟
	PWR->CR|=1<<8;    //取消备份区写保护
	//上面三步是必须的!
	RTC->CRL|=1<<4;   //允许配置 
	RTC->CNTL=seccount&0xffff;
	RTC->CNTH=seccount>>16;
	RTC->CRL&=~(1<<4);					//配置更新
	while(!(RTC->CRL&(1<<5)));	//等待RTC寄存器操作完成 
//	RTC_Get();//设置完之后更新一下数据 	
	return 0;	    
}
//初始化闹钟		  
//以1970年1月1日为基准
//1970~2099年为合法年份
//syear,smon,sday,hour,min,sec：闹钟的年月日时分秒   
//返回值:0,成功;其他:错误代码.
u8 RTC_Alarm_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec)
{
	u16 t;
	u32 seccount=0;
	if(syear<1970||syear>2099)return 1;	   
	for(t=1970;t<syear;t++)									//把所有年份的秒钟相加
	{
		if(Is_Leap_Year(t))seccount+=31622400;//闰年的秒钟数
		else seccount+=31536000;							//平年的秒钟数
	}
	smon-=1;
	for(t=0;t<smon;t++)											//把前面月份的秒钟数相加
	{
		seccount+=(u32)mon_table[t]*86400;		//月份秒钟数相加
		if(Is_Leap_Year(syear)&&t==1)seccount+=86400;//闰年2月份增加一天的秒钟数	   
	}
	seccount+=(u32)(sday-1)*86400;//把前面日期的秒钟数相加 
	seccount+=(u32)hour*3600;			//小时秒钟数
	seccount+=(u32)min*60;				//分钟秒钟数
	seccount+=sec;								//最后的秒钟加上去 			    
	//设置时钟
	RCC->APB1ENR|=1<<28;					//使能电源时钟
	RCC->APB1ENR|=1<<27;					//使能备份时钟
	PWR->CR|=1<<8;								//取消备份区写保护
	//上面三步是必须的!
	RTC->CRL|=1<<4;								//允许配置 
	RTC->ALRL=seccount&0xffff;
	RTC->ALRH=seccount>>16;
	RTC->CRL&=~(1<<4);						//配置更新
	while(!(RTC->CRL&(1<<5)));		//等待RTC寄存器操作完成  
	return 0;	    
}

u8 RTCcn_Get(void)
{
	if(get_sta == 0)return 1;
	timecount=RTC->CNTH;	//得到计数器中的值(秒钟数)
	timecount<<=16;
	timecount+=RTC->CNTL;
	RTC_IRQflag = 1;		//标记时间更新
	calendar.flag = 0;
	return 0;
}

u32 Get_RTCcont(void)
{
	return timecount;
}

//得到当前的时间，结果保存在calendar结构体里面
//返回值:0,成功;其他:错误代码.
u8 RTC_Get(void)
{
	static u16 daycnt=0;

	u32 temp=0;
	u16 temp1=0;
	
//	RTCcn_Get();
	if(get_sta == 0)return 0;
	
 	temp=timecount/86400;					//得到天数(秒钟数对应的
	calendar.week2 = (temp+4)%7;
	if(daycnt!=temp)						//超过一天了
	{	  
		daycnt=temp;
		temp1=1970;							//从1970年开始
		while(temp>=365)
		{				 
			if(Is_Leap_Year(temp1))			//是闰年
			{
				if(temp>=366)temp-=366;		//闰年的秒钟数
				else break;  
			}
			else temp-=365;					//平年 
			temp1++;  
		}   
		calendar.w_year=temp1;				//得到年份
		temp1=0;
		while(temp>=28)						//超过了一个月
		{
			if(Is_Leap_Year(calendar.w_year)&&temp1==1)	//当年是不是闰年/2月份
			{
				if(temp>=29)temp-=29;					//闰年的秒钟数
				else break; 
			}
			else 
			{
				if(temp>=mon_table[temp1])temp-=mon_table[temp1];//平年
				else break;
			}
			temp1++;  
		}
		calendar.w_month=temp1+1;			//得到月份
		calendar.w_date=temp+1;				//得到日期 
	}
	temp=timecount%86400;					//得到秒钟数   	   
	calendar.hour=temp/3600;				//小时
	calendar.min=(temp%3600)/60;			//分钟	
	calendar.sec=(temp%3600)%60;			//秒钟
	calendar.week=RTC_Get_Week(calendar.w_year,calendar.w_month,calendar.w_date);	//获取星期   
	return 0;
}	 
//获得现在是星期几
//功能描述:输入公历日期得到星期(只允许1901-2099年)
//year,month,day：公历年月日 
//返回值：星期号																						 
u8 RTC_Get_Week(u16 year,u8 month,u8 day)
{	
	u16 temp2;
	u8 yearH,yearL;
	
	yearH=year/100;	yearL=year%100; 
	// 如果为21世纪,年份数加100  
	if (yearH>19)yearL+=100;
	// 所过闰年数只算1900年之后的  
	temp2=yearL+yearL/4;
	temp2=temp2%7; 
	temp2=temp2+day+table_week[month-1];
	if (yearL%4==0&&month<3)temp2--;
	return(temp2%7);
}			  
















