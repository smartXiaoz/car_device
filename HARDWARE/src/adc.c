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
//��ʼ��ADC1
//�������ǽ��Թ���ͨ��Ϊ��
//����Ĭ�Ͻ�����ͨ��1																	   
void  Adc_Init(void)
{    
	
	DMA_Config(DMA1_Channel1,(u32)&ADC1->DR,(u32)ADC_ConvertedValue,ADC_NOFCH*ADC_CN);
	DMA_Enable(DMA1_Channel1,ENABLE);
	
	//�ȳ�ʼ��IO��
 	RCC->APB2ENR|=1<<2;    //ʹ��PORTA��ʱ�� 
	RCC->APB2ENR|=1<<4;    //ʹ��PORTC��ʱ�� 
	GPIOA->CRL&=0XF0FFFFFF;//PA6 anolog���� 
	GPIOC->CRL&=0XFFFFFF00;//PC0��PC1 anolog����
	
	RCC->APB2ENR|=1<<9;    //ADC1ʱ��ʹ��	  
	RCC->APB2RSTR|=1<<9;   //ADC1��λ
	RCC->APB2RSTR&=~(1<<9);//��λ����	    
	RCC->CFGR&=~(3<<14);   //��Ƶ��������	
	//SYSCLK/DIV2=9M ADCʱ������Ϊ12M,ADC���ʱ�Ӳ��ܳ���14M!
	//���򽫵���ADC׼ȷ���½�!  
	RCC->CFGR|=3<<14;      	 
	ADC1->CR1&=0XF0FFFF;   //����ģʽ����
	ADC1->CR1|=0<<16;      //��������ģʽ  
//	ADC1->CR1&=~(1<<8);    //��ɨ��ģʽ	  
//	ADC1->CR2&=~(1<<1);    //����ת��ģʽ
	ADC1->CR1|=1<<8;		//ɨ��ģʽ	  
	ADC1->CR2|=1<<1;		//����ת��ģʽ

	ADC1->CR2&=~(7<<17);	   
	ADC1->CR2|=7<<17;	   //�������ת��  
	ADC1->CR2|=1<<20;      //ʹ�����ⲿ����(SWSTART)!!!	����ʹ��һ���¼�������
	ADC1->CR2&=~(1<<11);   //�Ҷ���

	ADC1->CR2|=1<<23;      //ʹ���¶ȴ�����
	
	ADC1->SQR1&=~(0XF<<20);
//	ADC1->SQR1|=0<<20;     //1��ת���ڹ��������� Ҳ����ֻת����������1 

	ADC1->SQR1|=(ADC_NOFCH-(uint8_t)1)<<20;     //ADC_NOFCH��ת���ڹ���������
	
	
//	//����ͨ��6�Ĳ���ʱ��
//	ADC1->SMPR2&=~(7<<(3*6));   //ͨ��6����ʱ�����	  
// 	ADC1->SMPR2|=7<<(3*6); //ͨ��6  239.5����,��߲���ʱ�������߾�ȷ��	 
//	
//	//ͨ��16���ڲ��¶�
// 	ADC1->SMPR1&=~(7<<18);  //���ͨ��16ԭ��������	 
//	ADC1->SMPR1|=7<<18;     //ͨ��16  239.5����,�ٷ�����17.1us��12Mʱ���£�239.5�����ڣ�����ʱ��Լ20us
	
	ADC_RegularChannelConfig(ADC1, 6, 1,  ADC_SampleTime_239Cycles5);//����ͨ��6�Ĳ���ʱ��
	ADC_RegularChannelConfig(ADC1, 6, 2,  ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, 6, 3,  ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, 6, 4,  ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, 10, 5,  ADC_SampleTime_239Cycles5);//����ͨ��10�Ĳ���ʱ��
	ADC_RegularChannelConfig(ADC1, 10, 6,  ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, 10, 7,  ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, 10, 8,  ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, 11, 9,  ADC_SampleTime_239Cycles5);//����ͨ��11�Ĳ���ʱ��
	ADC_RegularChannelConfig(ADC1, 11, 10, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, 11, 11, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, 11, 12, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, 16, 13, ADC_SampleTime_239Cycles5);//����ͨ��16�Ĳ���ʱ��
	ADC_RegularChannelConfig(ADC1, 16, 14, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, 16, 15, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, 16, 16, ADC_SampleTime_239Cycles5);
	
	ADC1->CR2|=1<<8;	   //DMAģʽ
	
	ADC1->CR2|=1<<0;	   //����ADת����	 
	ADC1->CR2|=1<<3;       //ʹ�ܸ�λУ׼  
	while(ADC1->CR2&1<<3); //�ȴ�У׼���� 			 
    //��λ��������ò���Ӳ���������У׼�Ĵ�������ʼ�����λ��������� 		 
	ADC1->CR2|=1<<2;        //����ADУ׼	   
	while(ADC1->CR2&1<<2);  //�ȴ�У׼����
	//��λ����������Կ�ʼУ׼������У׼����ʱ��Ӳ�����  
	ADC1->CR2|=1<<22;       //��������ת��ͨ�� 
}				  
//���ADC1ĳ��ͨ����ֵ
//ch:ͨ��ֵ 0~16
//����ֵ:ת�����
u16 Get_Adc(u8 ch)
{
	//����ת������
	ADC1->SQR3&=0XFFFFFFE0;//��������1 ͨ��ch
	ADC1->SQR3|=ch;
	ADC1->CR2|=1<<22;       //��������ת��ͨ��
	while(!(ADC1->SR&1<<1));//�ȴ�ת������
	return ADC1->DR;		//����adcֵ
}
//��ȡͨ��ch��ת��ֵ��ȡtimes��,Ȼ��ƽ��
//ch:ͨ�����
//times:��ȡ����
//����ֵ:ͨ��ch��times��ת�����ƽ��ֵ
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
	
	temp = Get_Adc_Average(16,10);			//ԭʼֵ����
	V_Value = (float)temp*(3.3/4096);		//�����ѹֵ
	Temperature=(1.43-V_Value)/0.0043+25; 	//�������ǰ�¶�ֵ
	
	return Temperature;
}

u16 getchvalue(u8 ch)//�������
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


//��ȡADCֵ�������ֵ��ͬһͨ����βɼ����������⴮�����⣩
u8 KEY_SCAN(void)
{
	u16 Temp = 0;
	u8 KEY = 0;

	Temp = getchvalue(2);//��4��
	KEY = (Temp/930);//�����ֵ
	if(KEY != 4) 
	{		
		return KEY+5;
	}
	Temp = getchvalue(1);//��4��
	KEY = (Temp/930);//�����ֵ
	if(KEY != 4) 
	{		
		return KEY+1;
	}
	return 0;
}

_KEY_CACHE KEY_BUFF;

const unsigned char KEY_index[8] =	{5,0,1,4,6,2,3,7};
//KEY:���ţ�STA:��ֵ��0�ͷţ�1�̰���2����
u8 Writ_KEY(u8 KEY,u8 STA)	//д��һ�ΰ���������������
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
//KEY:���ţ�STA:��ֵ��0�ͷţ�1�̰���2����
u8 Read_KEY(u8 *KEY,u8 *STA)//�ӻ���������һ�ΰ�������
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

//�˲������ж��ͷ��볤�̰�
//KEY:���ţ�STA:��ֵ��0�ͷţ�1�̰���2����

static u16 Cnt_num = 0;
static u8 Last_Key = 0;
static u8 KEY_STA = 0;


void KEY_Get(void)
{
	u8 Key = 0;
	Key = KEY_SCAN();
	
	if(Key==0)//����û�а���
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
	else//��������
	{
		if(Last_Key!=Key)//��ֵ�仯
		{
			if(Last_Key!=0)//֮ǰ�ļ�ֵ����
			{
				if(KEY_STA == 1)//�仯֮ǰ�����ˣ��ͷ�һ��
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
			Last_Key = Key;//��¼��ֵ����
		}
		if(Cnt_num<160)Cnt_num++;//�������Ϊ1��
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













