#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"


#define ADC_CN		4
#define ADC_NOFCH	16	
#define KEY_MSG		50


/* ADC SMPx mask */
#define SMPR1_SMP_Set               ((uint32_t)0x00000007)
#define SMPR2_SMP_Set               ((uint32_t)0x00000007)
/* ADC SQx mask */
#define SQR3_SQ_Set                 ((uint32_t)0x0000001F)
#define SQR2_SQ_Set                 ((uint32_t)0x0000001F)
#define SQR1_SQ_Set                 ((uint32_t)0x0000001F)

/** @defgroup ADC_sampling_time 
  * @{
  */
#define ADC_SampleTime_1Cycles5                    ((uint8_t)0x00)
#define ADC_SampleTime_7Cycles5                    ((uint8_t)0x01)
#define ADC_SampleTime_13Cycles5                   ((uint8_t)0x02)
#define ADC_SampleTime_28Cycles5                   ((uint8_t)0x03)
#define ADC_SampleTime_41Cycles5                   ((uint8_t)0x04)
#define ADC_SampleTime_55Cycles5                   ((uint8_t)0x05)
#define ADC_SampleTime_71Cycles5                   ((uint8_t)0x06)
#define ADC_SampleTime_239Cycles5                  ((uint8_t)0x07)





typedef struct{
 u8 name;
 u8 value;
} _key_dat;

typedef struct{
 u8 writpointer;
 u8 readpointer;
 _key_dat frame[KEY_MSG];
} _KEY_CACHE;





	   									   
void Adc_Init(void); 				//ADC通道初始化
u16  Get_Adc(u8 ch); 				//获得某个通道值 
u16 getchvalue(u8 ch);//规则序号;
u16 Get_Adc_Average(u8 ch,u8 times);//得到某个通道10次采样的平均值 	 
float Get_Temperature(void);
u16 gettempvalue(void);
u16 getbattervalue(void);


u8 KEY_SCAN(void);
void KEY_Get(void);
u8 Read_KEY(u8 *KEY,u8 *STA);
void clear_KEYvalue(void);

#endif 















