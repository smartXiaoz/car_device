#include "dma.h"
#include "delay.h"


//DMA_CHx:DMAͨ��CHx
//cpar:�����ַ
//cmar:�洢����ַ
//cndtr:���ݴ�����  
void DMA_Config(DMA_Channel_TypeDef *DMA_CHx,u32 cpar,u32 cmar,u16 cndtr)
{
	RCC->AHBENR|=1<<0;			//����DMA1ʱ��
	delay_ms(5);				//�ȴ�DMAʱ���ȶ�
	DMA_CHx->CPAR=cpar; 	 	//DMA1 �����ַ 
	DMA_CHx->CMAR=(u32)cmar; 	//DMA1,�洢����ַ
	DMA_CHx->CNDTR=cndtr;    	//DMA1,����������
	DMA_CHx->CCR=0X00000000;	//��λ
	DMA_CHx->CCR|=DMA_DIR_PeripheralSRC;			//�Ӵ洢����
	DMA_CHx->CCR|=DMA_Mode_Circular;				//ѭ������ģʽ
	DMA_CHx->CCR|=DMA_PeripheralInc_Disable;		//�����ַ������ģʽ
	DMA_CHx->CCR|=DMA_MemoryInc_Enable;				//�洢������ģʽ
	DMA_CHx->CCR|=DMA_PeripheralDataSize_HalfWord;	//�������ݿ��Ϊ16λ
	DMA_CHx->CCR|=DMA_MemoryDataSize_HalfWord;		//�洢�����ݿ��16λ
	DMA_CHx->CCR|=DMA_Priority_High;				//�е����ȼ�
	DMA_CHx->CCR|=DMA_M2M_Disable;					//�Ǵ洢�����洢��ģʽ		  	
} 

void DMA_Enable(DMA_Channel_TypeDef *DMA_CHx,FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    /* Enable the selected DMAy Channelx */
    DMA_CHx->CCR |= DMA_CCR1_EN;
  }
  else
  {
    /* Disable the selected DMAy Channelx */
    DMA_CHx->CCR &= (uint16_t)(~DMA_CCR1_EN);
  }
}	  

 








