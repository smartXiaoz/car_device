#include "24cxx.h" 
#include "delay.h" 										 
#include "usart.h" 

//��ʼ��IIC�ӿ�
void AT24CXX_Init(void)
{
	IIC1_Init();
}
//��AT24CXXָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ  
//����ֵ  :����������
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{			  
	u8 temp=0;		  	    																 
    IIC1_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC1_Send_Byte(EE_ADDR);	   //����д����
		IIC1_Wait_Ack();
		IIC1_Send_Byte(ReadAddr>>8);//���͸ߵ�ַ	    
	}else IIC1_Send_Byte(EE_ADDR+((ReadAddr/256)<<1));   //����������ַ0XA0,д���ݣ� (ReadAddr/256)<<1)Ϊҳ��ַ�������ռ��A0 - A2
	IIC1_Wait_Ack(); 
    IIC1_Send_Byte(ReadAddr%256);   //���͵͵�ַ 
	IIC1_Wait_Ack();	    
	IIC1_Start();  	 	   
	IIC1_Send_Byte(EE_ADDR|0x01);           //�������ģʽ			   
	IIC1_Wait_Ack();	 
    temp=IIC1_Read_Byte(0);		   
    IIC1_Stop();//����һ��ֹͣ����	    
	return temp;
}
//��AT24CXXָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ    
//DataToWrite:Ҫд�������
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
    IIC1_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC1_Send_Byte(EE_ADDR);	    //����д����
		IIC1_Wait_Ack();
		IIC1_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ	  
	}else IIC1_Send_Byte(EE_ADDR+((WriteAddr/256)<<1));   //����������ַ0XA0,д���� 	 
	IIC1_Wait_Ack();	   
    IIC1_Send_Byte(WriteAddr%256);   //���͵͵�ַ
	IIC1_Wait_Ack(); 	 										  		   
	IIC1_Send_Byte(DataToWrite);     //�����ֽ�							   
	IIC1_Wait_Ack();  		    	   
    IIC1_Stop();						//����һ��ֹͣ���� 
	delay_xms(100);	 				//EEPROM��д���ٶȱȽ����������ӳ�
}

//����1:���ʧ��
//����0:���ɹ�
u8 AT24CXX_Check(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(255);//����ÿ�ο�����дAT24CXX			   
	if(temp==0X55)return 0;		   
	else//�ų���һ�γ�ʼ�������
	{
		printf("it is new IC! \r\n");
		AT24CXX_WriteOneByte(255,0X55);
		AT24CXX_WriteOneByte(0,0X00);
		AT24CXX_WriteOneByte(1,0X00);
	    temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

//��AT24CXX�����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ���
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  
//��AT24CXX�����ָ����ַ��ʼд��ָ������������
//WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255
//pBuffer   :���������׵�ַ
//NumToWrite:Ҫд�����ݵĸ���
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}








