#ifndef __DMA_H
#define	__DMA_H	   
#include "sys.h"


/** @defgroup DMA_data_transfer_direction 
  * @{
  */
#define DMA_DIR_PeripheralDST              ((uint32_t)0x00000010)
#define DMA_DIR_PeripheralSRC              ((uint32_t)0x00000000)

/** @defgroup DMA_peripheral_incremented_mode 
  * @{
  */
#define DMA_PeripheralInc_Enable           ((uint32_t)0x00000040)
#define DMA_PeripheralInc_Disable          ((uint32_t)0x00000000)

/** @defgroup DMA_memory_incremented_mode 
  * @{
  */
#define DMA_MemoryInc_Enable               ((uint32_t)0x00000080)
#define DMA_MemoryInc_Disable              ((uint32_t)0x00000000)

/** @defgroup DMA_peripheral_data_size 
  * @{
  */
#define DMA_PeripheralDataSize_Byte        ((uint32_t)0x00000000)
#define DMA_PeripheralDataSize_HalfWord    ((uint32_t)0x00000100)
#define DMA_PeripheralDataSize_Word        ((uint32_t)0x00000200)

/** @defgroup DMA_memory_data_size 
  * @{
  */
#define DMA_MemoryDataSize_Byte            ((uint32_t)0x00000000)
#define DMA_MemoryDataSize_HalfWord        ((uint32_t)0x00000400)
#define DMA_MemoryDataSize_Word            ((uint32_t)0x00000800)

/** @defgroup DMA_circular_normal_mode 
  * @{
  */
#define DMA_Mode_Circular                  ((uint32_t)0x00000020)
#define DMA_Mode_Normal                    ((uint32_t)0x00000000)

/** @defgroup DMA_priority_level 
  * @{
  */
#define DMA_Priority_VeryHigh              ((uint32_t)0x00003000)
#define DMA_Priority_High                  ((uint32_t)0x00002000)
#define DMA_Priority_Medium                ((uint32_t)0x00001000)
#define DMA_Priority_Low                   ((uint32_t)0x00000000)

/** @defgroup DMA_memory_to_memory 
  * @{
  */
#define DMA_M2M_Enable                     ((uint32_t)0x00004000)
#define DMA_M2M_Disable                    ((uint32_t)0x00000000)

void DMA_Config(DMA_Channel_TypeDef *DMA_CHx,u32 cpar,u32 cmar,u16 cndtr);	//≈‰÷√DMA1_CHx
void DMA_Enable(DMA_Channel_TypeDef *DMA_CHx,FunctionalState NewState);		// πƒ‹DMA1_CHx		   
#endif







