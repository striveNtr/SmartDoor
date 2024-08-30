#include "sys.h"
#include "myusart.h"	
#include "stdio.h"	

u8 Receive_over=0;
u8 Close_door=0;
u8 Open_door=0;

u8 point=0;
u8 USART4_RX_BUF[USART4_REC_LEN];//接收缓冲,最大USART_REC_LEN个字节.
u16 USART4_RX_STA=0;       //接收状态标记	 

void uart4_init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//使能GPIOC时钟
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//使能AFIO复用时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);	//使能UART4时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC10

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PC11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC11


	//USART 初始化设置
	USART_DeInit(UART4); //复位串口4
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(UART4, &USART_InitStructure); //初始化串口1	

	//Uart4 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;//串口4中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//开启串口接受中断

  USART_Cmd(UART4, ENABLE);                    //使能串口4 
}



//55 66 11  77
//55  66  22  77  
void UART4_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		   Res =USART_ReceiveData(UART4);	//读取接收到的数据
		   USART4_RX_BUF[point]=Res;
		   point++;
		   if((point==4)&&(USART4_RX_BUF[0]==0x55)&&(USART4_RX_BUF[1]==0x66)&&(USART4_RX_BUF[3]==0x77))
			 {
			     point=0;
				   if(USART4_RX_BUF[2]==0x11)
					 {
						   Receive_over=1;
					     Open_door=1;
					 }else if(USART4_RX_BUF[2]==0x22)
					 {
						   Receive_over=1;
					     Close_door=1;
					 }
					 memset(USART4_RX_BUF,USART4_REC_LEN,0);
			 }
		 
  } 
} 
