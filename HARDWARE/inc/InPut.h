#ifndef __INPUT_H
#define __INPUT_H	 
#include "sys.h"


#define ACC_IN		PAin(5) //ACC���
#define Blight_IN	PAin(1)	//�������
#define BACK_IN		PCin(2)	//����


void InPut_Init(void);	//IO��ʼ��
u8 InPut_Scan(void);  	//ɨ�躯��					    
#endif
