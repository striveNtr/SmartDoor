#ifndef __USART4_H
#define __USART4_H
#include "stdio.h"	
#include "sys.h" 

extern u8 Receive_over;
extern u8 Close_door;
extern u8 Open_door;

#define USART4_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART4_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
	  	
extern u8  USART4_RX_BUF[USART4_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART4_RX_STA;         		//����״̬���	
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart4_init(u32 bound);

#endif
