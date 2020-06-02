#ifndef __STMFLASH_H__
#define __STMFLASH_H__
#include "sys.h"  

#define SAVE_ADDRESS	0X08010000		//F103C8T6���ٷ���FLASHΪ64K
										//ʵ�ʿɶ�д����128K��ֻ���64K�ٷ�δ���ɿ��Զ�д���ԣ�
										//�˴���64K����ʼ�洢�û����ݣ�

//�ڲ�����
//void STMFLASH_Unlock(void);					  //FLASH����
//void STMFLASH_Lock(void);					  //FLASH����
//u8 STMFLASH_GetStatus(void);				  //���״̬
//u8 STMFLASH_WaitDone(u16 time);				  //�ȴ���������
//u8 STMFLASH_ErasePage(u32 paddr);			  //����ҳ
//u8 STMFLASH_WriteHalfWord(u32 faddr, u16 dat);//д�����
//u16 STMFLASH_ReadHalfWord(u32 faddr);		  //��������  
//void STMFLASH_WriteLenByte(u32 WriteAddr,u32 DataToWrite,u16 Len);	//ָ����ַ��ʼд��ָ�����ȵ�����
//u32 STMFLASH_ReadLenByte(u32 ReadAddr,u16 Len);						//ָ����ַ��ʼ��ȡָ����������

//�ⲿ����
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����

#endif

















