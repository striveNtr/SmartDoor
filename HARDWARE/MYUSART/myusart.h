#ifndef __USART4_H
#define __USART4_H
#include "stdio.h"	
#include "sys.h" 

extern u8 Receive_over;
extern u8 Close_door;
extern u8 Open_door;

#define USART4_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART4_RX 			1		//使能（1）/禁止（0）串口1接收
	  	
extern u8  USART4_RX_BUF[USART4_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART4_RX_STA;         		//接收状态标记	
//如果想串口中断接收，请不要注释以下宏定义
void uart4_init(u32 bound);

#endif
