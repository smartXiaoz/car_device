
#ifndef __SWITCH_H
#define __SWITCH_H	 
#include "sys.h"


typedef struct{
	u8 ctr;//����ָ��
	u8 sta;//״̬
}Switch_STA;

extern Switch_STA Switch;


#define Switch_S0_Pin PBout(14)	
#define Switch_S1_Pin PBout(15)

void Switch_Init(void);	//��ʼ��		
u8 Switch_ctrl(void);	//�����л�
#endif




