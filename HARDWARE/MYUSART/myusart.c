#include "sys.h"
#include "myusart.h"	
#include "stdio.h"	

u8 Receive_over=0;
u8 Close_door=0;
u8 Open_door=0;

u8 point=0;
u8 USART4_RX_BUF[USART4_REC_LEN];//���ջ���,���USART_REC_LEN���ֽ�.
u16 USART4_RX_STA=0;       //����״̬���	 

void uart4_init(u32 bound)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//ʹ��GPIOCʱ��
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//ʹ��AFIO����ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);	//ʹ��UART4ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PC10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC10

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PC11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC11


	//USART ��ʼ������
	USART_DeInit(UART4); //��λ����4
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(UART4, &USART_InitStructure); //��ʼ������1	

	//Uart4 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;//����4�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//�������ڽ����ж�

  USART_Cmd(UART4, ENABLE);                    //ʹ�ܴ���4 
}



//55 66 11  77
//55  66  22  77  
void UART4_IRQHandler(void)                	//����1�жϷ������
{
	u8 Res;
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		   Res =USART_ReceiveData(UART4);	//��ȡ���յ�������
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
