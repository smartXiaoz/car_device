#ifndef __STMFLASH_H__
#define __STMFLASH_H__
#include "sys.h"  

#define SAVE_ADDRESS	0X08010000		//F103C8T6，官方称FLASH为64K
										//实际可读写共有128K，只因后64K官方未做可靠性读写测试，
										//此处从64K处开始存储用户数据，

//内部调用
//void STMFLASH_Unlock(void);					  //FLASH解锁
//void STMFLASH_Lock(void);					  //FLASH上锁
//u8 STMFLASH_GetStatus(void);				  //获得状态
//u8 STMFLASH_WaitDone(u16 time);				  //等待操作结束
//u8 STMFLASH_ErasePage(u32 paddr);			  //擦除页
//u8 STMFLASH_WriteHalfWord(u32 faddr, u16 dat);//写入半字
//u16 STMFLASH_ReadHalfWord(u32 faddr);		  //读出半字  
//void STMFLASH_WriteLenByte(u32 WriteAddr,u32 DataToWrite,u16 Len);	//指定地址开始写入指定长度的数据
//u32 STMFLASH_ReadLenByte(u32 ReadAddr,u16 Len);						//指定地址开始读取指定长度数据

//外部调用
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);		//从指定地址开始写入指定长度的数据
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);   		//从指定地址开始读出指定长度的数据

#endif

















