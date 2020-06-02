#include "adc.h"
#include "delay.h"					   
#include "information.h"
#include "usart.h"
#include "dma.h"

void ADC_RegularChannelConfig(ADC_TypeDef* ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime)
{
  uint32_t tmpreg1 = 0, tmpreg2 = 0;
  /* Check the parameters */

  /* if ADC_Channel_10 ... ADC_Channel_17 is selected */
  if (ADC_Channel > 9)
  {
    /* Get the old register value */
    tmpreg1 = ADCx->SMPR1;
    /* Calculate the mask to clear */
    tmpreg2 = SMPR1_SMP_Set << (3 * (ADC_Channel - 10));
    /* Clear the old channel sample time */
    tmpreg1 &= ~tmpreg2;
    /* Calculate the mask to set */
    tmpreg2 = (uint32_t)ADC_SampleTime << (3 * (ADC_Channel - 10));
    /* Set the new channel sample time */
    tmpreg1 |= tmpreg2;
    /* Store the new register value */
    ADCx->SMPR1 = tmpreg1;
  }
  else /* ADC_Channel include in ADC_Channel_[0..9] */
  {
    /* Get the old register value */
    tmpreg1 = ADCx->SMPR2;
    /* Calculate the mask to clear */
    tmpreg2 = SMPR2_SMP_Set << (3 * ADC_Channel);
    /* Clear the old channel sample time */
    tmpreg1 &= ~tmpreg2;
    /* Calculate the mask to set */
    tmpreg2 = (uint32_t)ADC_SampleTime << (3 * ADC_Channel);
    /* Set the new channel sample time */
    tmpreg1 |= tmpreg2;
    /* Store the new register value */
    ADCx->SMPR2 = tmpreg1;
  }
  /* For Rank 1 to 6 */
  if (Rank < 7)
  {
    /* Get the old register value */
    tmpreg1 = ADCx->SQR3;
    /* Calculate the mask to clear */
    tmpreg2 = SQR3_SQ_Set << (5 * (Rank - 1));
    /* Clear the old SQx bits for the selected rank */
    tmpreg1 &= ~tmpreg2;
    /* Calculate the mask to set */
    tmpreg2 = (uint32_t)ADC_Channel << (5 * (Rank - 1));
    /* Set the SQx bits for the selected rank */
    tmpreg1 |= tmpreg2;
    /* Store the new register value */
    ADCx->SQR3 = tmpreg1;
  }
  /* For Rank 7 to 12 */
  else if (Rank < 13)
  {
    /* Get the old register value */
    tmpreg1 = ADCx->SQR2;
    /* Calculate the mask to clear */
    tmpreg2 = SQR2_SQ_Set << (5 * (Rank - 7));
    /* Clear the old SQx bits for the selected rank */
    tmpreg1 &= ~tmpreg2;
    /* Calculate the mask to set */
    tmpreg2 = (uint32_t)ADC_Channel << (5 * (Rank - 7));
    /* Set the SQx bits for the selected rank */
    tmpreg1 |= tmpreg2;
    /* Store the new register value */
    ADCx->SQR2 = tmpreg1;
  }
  /* For Rank 13 to 16 */
  else
  {
    /* Get the old register value */
    tmpreg1 = ADCx->SQR1;
    /* Calculate the mask to clear */
    tmpreg2 = SQR1_SQ_Set << (5 * (Rank - 13));
    /* Clear the old SQx bits for the selected rank */
    tmpreg1 &= ~tmpreg2;
    /* Calculate the mask to set */
    tmpreg2 = (uint32_t)ADC_Channel << (5 * (Rank - 13));
    /* Set the SQx bits for the selected rank */
    tmpreg1 |= tmpreg2;
    /* Store the new register value */
    ADCx->SQR1 = tmpreg1;
  }
}
volatile u16 ADC_ConvertedValue[ADC_NOFCH*ADC_CN];
//初始化ADC1
//这里我们仅以规则通道为例
//我们默认仅开启通道1																	   
void  Adc_Init(void)
{    
	
	DMA_Config(DMA1_Channel1,(u32)&ADC1->DR,(u32)ADC_ConvertedValue,ADC_NOFCH*ADC_CN);
	DMA_Enable(DMA1_Channel1,ENABLE);
	
	//先初始化IO口
 	RCC->APB2ENR|=1<<2;    //使能PORTA口时钟 
	RCC->APB2ENR|=1<<4;    //使能PORTC口时钟 
	GPIOA->CRL&=0XF0FFFFFF;//PA6 anolog输入 
	GPIOC->CRL&=0XFFFFFF00;//PC0、PC1 anolog输入
	
	RCC->APB2ENR|=1<<9;    //ADC1时钟使能	  
	RCC->APB2RSTR|=1<<9;   //ADC1复位
	RCC->APB2RSTR&=~(1<<9);//复位结束	    
	RCC->CFGR&=~(3<<14);   //分频因子清零	
	//SYSCLK/DIV2=9M ADC时钟设置为12M,ADC最大时钟不能超过14M!
	//否则将导致ADC准确度下降!  
	RCC->CFGR|=3<<14;      	 
	ADC1->CR1&=0XF0FFFF;   //工作模式清零
	ADC1->CR1|=0<<16;      //独立工作模式  
//	ADC1->CR1&=~(1<<8);    //非扫描模式	  
//	ADC1->CR2&=~(1<<1);    //单次转换模式
	ADC1->CR1|=1<<8;		//扫描模式	  
	ADC1->CR2|=1<<1;		//连续转换模式

	ADC1->CR2&=~(7<<17);	   
	ADC1->CR2|=7<<17;	   //软件控制转换  
	ADC1->CR2|=1<<20;      //使用用外部触发(SWSTART)!!!	必须使用一个事件来触发
	ADC1->CR2&=~(1<<11);   //右对齐

	ADC1->CR2|=1<<23;      //使能温度传感器
	
	ADC1->SQR1&=~(0XF<<20);
//	ADC1->SQR1|=0<<20;     //1个转换在规则序列中 也就是只转换规则序列1 

	ADC1->SQR1|=(ADC_NOFCH-(uint8_t)1)<<20;     //ADC_NOFCH个转换在规则序列中
	
	
//	//设置通道6的采样时间
//	ADC1->SMPR2&=~(7<<(3*6));   //通道6采样时间清空	  
// 	ADC1->SMPR2|=7<<(3*6); //通道6  239.5周期,提高采样时间可以提高精确度	 
//	
//	//通道16，内部温度
// 	ADC1->SMPR1&=~(7<<18);  //清除通道16原来的设置	 
//	ADC1->SMPR1|=7<<18;     //通道16  239.5周期,官方建议17.1us，12M时钟下，239.5个周期，采样时间约20us
	
	ADC_RegularChannelConfig(ADC1, 6, 1,  ADC_SampleTime_239Cycles5);//设置通道6的采样时间
	ADC_RegularChannelConfig(ADC1, 6, 2,  ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, 6, 3,  ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, 6, 4,  ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, 10, 5,  ADC_SampleTime_239Cycles5);//设置通道10的采样时间
	ADC_RegularChannelConfig(ADC1, 10, 6,  ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, 10, 7,  ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, 10, 8,  ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, 11, 9,  ADC_SampleTime_239Cycles5);//设置通道11的采样时间
	ADC_RegularChannelConfig(ADC1, 11, 10, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, 11, 11, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, 11, 12, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, 16, 13, ADC_SampleTime_239Cycles5);//设置通道16的采样时间
	ADC_RegularChannelConfig(ADC1, 16, 14, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, 16, 15, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, 16, 16, ADC_SampleTime_239Cycles5);
	
	ADC1->CR2|=1<<8;	   //DMA模式
	
	ADC1->CR2|=1<<0;	   //开启AD转换器	 
	ADC1->CR2|=1<<3;       //使能复位校准  
	while(ADC1->CR2&1<<3); //等待校准结束 			 
    //该位由软件设置并由硬件清除。在校准寄存器被初始化后该位将被清除。 		 
	ADC1->CR2|=1<<2;        //开启AD校准	   
	while(ADC1->CR2&1<<2);  //等待校准结束
	//该位由软件设置以开始校准，并在校准结束时由硬件清除  
	ADC1->CR2|=1<<22;       //启动规则转换通道 
}				  
//获得ADC1某个通道的值
//ch:通道值 0~16
//返回值:转换结果
u16 Get_Adc(u8 ch)
{
	//设置转换序列
	ADC1->SQR3&=0XFFFFFFE0;//规则序列1 通道ch
	ADC1->SQR3|=ch;
	ADC1->CR2|=1<<22;       //启动规则转换通道
	while(!(ADC1->SR&1<<1));//等待转换结束
	return ADC1->DR;		//返回adc值
}
//获取通道ch的转换值，取times次,然后平均
//ch:通道编号
//times:获取次数
//返回值:通道ch的times次转换结果平均值
u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(5);
	}
	return temp_val/times;
}

float Get_Temperature(void)
{
	u16 temp = 0;
	float V_Value = 0.0;
	float Temperature = 0.0;
	
	temp = Get_Adc_Average(16,10);			//原始值采样
	V_Value = (float)temp*(3.3/4096);		//计算电压值
	Temperature=(1.43-V_Value)/0.0043+25; 	//计算出当前温度值
	
	return Temperature;
}

u16 getchvalue(u8 ch)//规则序号
{
	u16 temp = 0;
	for(u8 i=0;i<ADC_CN;i++)
	{
		temp += ADC_ConvertedValue[i*ADC_NOFCH+ch*4+2];
	}
	return temp/ADC_CN;
}
u16 getbattervalue(void)
{
	u16 temp = 0;
	for(u8 i=0;i<ADC_CN;i++)
	{
		temp += ADC_ConvertedValue[i*ADC_NOFCH+2];
	}
	return temp/ADC_CN;
}
u16 gettempvalue(void)
{
	u16 temp = 0;
	for(u8 i=0;i<ADC_CN;i++)
	{
		temp += ADC_ConvertedValue[i*ADC_NOFCH+14];
	}
	return temp/ADC_CN;
}


//读取ADC值，计算键值（同一通道多次采集，尽量避免串扰问题）
u8 KEY_SCAN(void)
{
	u16 Temp = 0;
	u8 KEY = 0;

	Temp = getchvalue(2);//采4次
	KEY = (Temp/930);//计算键值
	if(KEY != 4) 
	{		
		return KEY+5;
	}
	Temp = getchvalue(1);//采4次
	KEY = (Temp/930);//计算键值
	if(KEY != 4) 
	{		
		return KEY+1;
	}
	return 0;
}

_KEY_CACHE KEY_BUFF;

const unsigned char KEY_index[8] =	{5,0,1,4,6,2,3,7};
//KEY:键号；STA:键值，0释放，1短按，2长按
u8 Writ_KEY(u8 KEY,u8 STA)	//写入一次按键操作到缓冲区
{
	if(SYS_Inf.MPU_STA!=1)return 0;
	
#ifdef EMMCDEBUG
	return 0;
#endif
	
	KEY_BUFF.frame[KEY_BUFF.writpointer].name = KEY_index[KEY-1];
	KEY_BUFF.frame[KEY_BUFF.writpointer++].value = STA;
	
	if(KEY_BUFF.writpointer>=KEY_MSG)KEY_BUFF.writpointer=0;
	if(KEY_BUFF.writpointer==KEY_BUFF.readpointer)
	{
		KEY_BUFF.readpointer++;
		if(KEY_BUFF.readpointer>=KEY_MSG)KEY_BUFF.readpointer=0;
	}
	
	return 1;
}
//KEY:键号；STA:键值，0释放，1短按，2长按
u8 Read_KEY(u8 *KEY,u8 *STA)//从缓冲区读出一次按键操作
{
#ifdef EMMCDEBUG
	return 0;
#endif
	
	if(KEY_BUFF.readpointer == KEY_BUFF.writpointer) return 0;
	*KEY = KEY_BUFF.frame[KEY_BUFF.readpointer].name;
	*STA = KEY_BUFF.frame[KEY_BUFF.readpointer++].value;
	if(KEY_BUFF.readpointer>=KEY_MSG)KEY_BUFF.readpointer=0;
	return 1;
}

void clear_KEYvalue(void)
{
	KEY_BUFF.readpointer = KEY_BUFF.writpointer;
}

//滤波处理，判断释放与长短按
//KEY:键号；STA:键值，0释放，1短按，2长按

static u16 Cnt_num = 0;
static u8 Last_Key = 0;
static u8 KEY_STA = 0;


void KEY_Get(void)
{
	u8 Key = 0;
	Key = KEY_SCAN();
	
	if(Key==0)//按键没有按下
	{
		if(Last_Key == 0)return;
		
		if(KEY_STA == 1)
		{
			Writ_KEY(Last_Key,0);	
			KEY_STA = 0;
		}
		else if(Cnt_num>4)Writ_KEY(Last_Key,1),Writ_KEY(Last_Key,0);
		Cnt_num = 0;
		Last_Key = 0;
		return;
	}
	else//按键按下
	{
		if(Last_Key!=Key)//键值变化
		{
			if(Last_Key!=0)//之前的键值处理
			{
				if(KEY_STA == 1)//变化之前长按了，释放一次
				{
					Writ_KEY(Last_Key,0);
					KEY_STA = 0;
				}
#ifdef	SKY321
#else	
				else if(Cnt_num>4)Writ_KEY(Last_Key,1),Writ_KEY(Last_Key,0);
#endif			
			}
			Cnt_num=0;	
			Last_Key = Key;//记录键值更新
		}
		if(Cnt_num<160)Cnt_num++;//长按标记为1秒
		else
		{	
			if(KEY_STA==0)
			{
				Writ_KEY(Last_Key,2);
				KEY_STA = 1;
			}
		}
	}
	return;
}













