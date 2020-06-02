#include "dma.h"
#include "delay.h"


//DMA_CHx:DMA通道CHx
//cpar:外设地址
//cmar:存储器地址
//cndtr:数据传输量  
void DMA_Config(DMA_Channel_TypeDef *DMA_CHx,u32 cpar,u32 cmar,u16 cndtr)
{
	RCC->AHBENR|=1<<0;			//开启DMA1时钟
	delay_ms(5);				//等待DMA时钟稳定
	DMA_CHx->CPAR=cpar; 	 	//DMA1 外设地址 
	DMA_CHx->CMAR=(u32)cmar; 	//DMA1,存储器地址
	DMA_CHx->CNDTR=cndtr;    	//DMA1,传输数据量
	DMA_CHx->CCR=0X00000000;	//复位
	DMA_CHx->CCR|=DMA_DIR_PeripheralSRC;			//从存储器读
	DMA_CHx->CCR|=DMA_Mode_Circular;				//循环传输模式
	DMA_CHx->CCR|=DMA_PeripheralInc_Disable;		//外设地址非增量模式
	DMA_CHx->CCR|=DMA_MemoryInc_Enable;				//存储器增量模式
	DMA_CHx->CCR|=DMA_PeripheralDataSize_HalfWord;	//外设数据宽度为16位
	DMA_CHx->CCR|=DMA_MemoryDataSize_HalfWord;		//存储器数据宽度16位
	DMA_CHx->CCR|=DMA_Priority_High;				//中等优先级
	DMA_CHx->CCR|=DMA_M2M_Disable;					//非存储器到存储器模式		  	
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

 








