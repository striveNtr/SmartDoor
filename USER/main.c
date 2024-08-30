#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "oledfun.h"
#include "oled.h"
#include "key.h"
#include "rc522.h"
#include "beep.h"
#include "pwm.h"
#include "timers.h"
#include "event_groups.h"
#include "string.h"
#include "myusart.h"
#include "timer.h"
#include "usart3.h"
#include "esp8266.h"
#include "server.h"
#include "usart2.h"
#include "AS608.h"

//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С	
#define START_STK_SIZE 		128  
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

//�������ȼ�
#define RC522_TASK_PRIO		2
//�����ջ��С	
#define RC522_STK_SIZE 		128  
//������
TaskHandle_t RC522Task_Handler;
//������
void rc522_task(void *pvParameters);

//�������ȼ�
#define BUTTON_TASK_PRIO		3
//�����ջ��С	
#define BUTTON_STK_SIZE 		128  
//������
TaskHandle_t ButtonTask_Handler;
//������
void button_task(void *pvParameters);

//��������¼���־�鴦������
//�������ȼ�
#define EVENTGROUP_TASK_PRIO		4
//�����ջ��С	
#define EVENTGROUP_STK_SIZE 		128 
//������
TaskHandle_t EventGroupTask_Handler;
//������
void eventgroup_task(void *pvParameters);

//oled����
//�������ȼ�
#define OLED_TASK_PRIO		5
//�����ջ��С	
#define OLED_STK_SIZE 		128 
//������
TaskHandle_t OledTask_Handler;
//������
void oled_task(void *pvParameters);

//��������
//�������ȼ�
#define BLUETOOTH_TASK_PRIO		6
//�����ջ��С	
#define BLUETOOTH_STK_SIZE 		128 
//������
TaskHandle_t BluetoothTask_Handler;
//������
void bluetooth_task(void *pvParameters);

//esp8266����
//�������ȼ�
#define ESP8266_TASK_PRIO		7
//�����ջ��С	
#define ESP8266_STK_SIZE 		128 
//������
TaskHandle_t Esp8266Task_Handler;
//������
void esp8266_task(void *pvParameters);


//ָ��ʶ������
//�������ȼ�
#define AS608_TASK_PRIO		  8
//�����ջ��С	
#define AS608_STK_SIZE 		128 
//������
TaskHandle_t AS608Task_Handler;
//������
void AS608_task(void *pvParameters);

////�������key���Կ���rfid�Ͱ����� ����
////�������ȼ�
//#define TASK3_TASK_PRIO		7
////�����ջ��С	
//#define TASK3_STK_SIZE 		128 
////������
//TaskHandle_t Task3Task_Handler;
////������
//void key_task(void *pvParameters);


EventGroupHandle_t EventGroupHandler;	//�¼���־����
 
#define EVENTBIT_0	(1<<0)				//�¼�λ
#define EVENTBIT_1	(1<<1)
#define EVENTBIT_ALL	(EVENTBIT_0|EVENTBIT_1)//������ȷ

u8 Pwd2[]={49,50,51,52,53,54};  //��������2
TimerHandle_t	 BacklightTimer_Handle;			//���ζ�ʱ�����
void BacklightTimerCallback(TimerHandle_t xTimer);		//���ζ�ʱ���ص�����

u8 value = 0; 
u8 OpenFlag=0;


void Server_Start(void)
{
	TIM_SetCompare1(TIM8,25);//180��
	delay_ms(1000);	
}

u8 ensure;
u16 ValidN;//ģ������Чָ�Ƹ���
SysPara AS608Para;//ָ��ģ��AS608����

//ˢָ��
void press_FR(void)
{
	SearchResult seach;
	u8 ensure;
	char *str;
	ensure=PS_GetImage();
	if(ensure==0x00)//��ȡͼ��ɹ� 
	{	
		//BEEP=1;//�򿪷�����	
		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //���������ɹ�
		{		
		//	BEEP=0;//�رշ�����	
			ensure=PS_HighSpeedSearch(CharBuffer1,0,AS608Para.PS_max,&seach);
			if(ensure==0x00)//�����ɹ�
			{				
				delay_ms(400);	  
			}				
	  }
	}
}

int main(void)
{
	delay_init();	    				//��ʱ������ʼ��	  
	uart_init(115200);					//��ʼ������
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4	 
	uart4_init(9600);
	TIM8_PWM_Init();
	usart2_init(115200);//��ʼ������2,������ָ��ģ��ͨѶ 
	PS_StaGPIO_Init();	//��ʼ��FR��״̬����
	delay_ms(100);	 
	while(PS_HandShake(&AS608Addr))//��AS608ģ������
	{
			delay_ms(400);	  
	}
	ensure=PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
	ensure=PS_ReadSysPara(&AS608Para);  //������ 
	OLED_Init(); 
	OLED_ColorTurn(0);//0������ʾ��1 ��ɫ��ʾ
  OLED_DisplayTurn(0);//0������ʾ 1 ��Ļ��ת��ʾ
	delay_ms(200);
//	BEEP_Init();
	RC522_Init();
//	usart3_init(115200);	 				//���ڳ�ʼ��Ϊ11520 esp8266
////	esp8266_start_trans();							//esp8266���г�ʼ�� 
	oled_start(); //������ʾ
	zhu_1();
	GetMatrixKeyValue();
	//������ʼ����
    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������              
    vTaskStartScheduler();          //�����������
}

//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���   
    //�����¼���־��
	EventGroupHandler=xEventGroupCreate();	 //�����¼���־��
	
	  //����RFID������
    xTaskCreate((TaskFunction_t )rc522_task,     
                (const char*    )"rc522_task",   
                (uint16_t       )RC522_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )RC522_TASK_PRIO,
                (TaskHandle_t*  )&RC522Task_Handler);  
    //����4*4button��������
    xTaskCreate((TaskFunction_t )button_task,     	
                (const char*    )"button_task",   	
                (uint16_t       )BUTTON_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )BUTTON_TASK_PRIO,	
                (TaskHandle_t*  )&ButtonTask_Handler);   
      
    //�����¼���־�鴦������
//    xTaskCreate((TaskFunction_t )eventgroup_task,             
//                (const char*    )"eventgroup_task",           
//                (uint16_t       )EVENTGROUP_STK_SIZE,        
//                (void*          )NULL,                  
//                (UBaseType_t    )EVENTGROUP_TASK_PRIO,        
//                (TaskHandle_t*  )&EventGroupTask_Handler);	
    //����OLED����
    xTaskCreate((TaskFunction_t )oled_task,             
                (const char*    )"oled_task",           
                (uint16_t       )OLED_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )OLED_TASK_PRIO,        
                (TaskHandle_t*  )&OledTask_Handler);
    //������������
    xTaskCreate((TaskFunction_t )bluetooth_task,             
                (const char*    )"bluetooth_task",           
                (uint16_t       )BLUETOOTH_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )BLUETOOTH_TASK_PRIO,        
                (TaskHandle_t*  )&BluetoothTask_Handler);				
    //����esp8266����
    xTaskCreate((TaskFunction_t )esp8266_task,             
                (const char*    )"esp8266_task",           
                (uint16_t       )ESP8266_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )ESP8266_TASK_PRIO,        
                (TaskHandle_t*  )&AS608Task_Handler);		

    xTaskCreate((TaskFunction_t )AS608_task,             
                (const char*    )"AS608_task",           
                (uint16_t       )AS608_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )AS608_TASK_PRIO,        
                (TaskHandle_t*  )&Esp8266Task_Handler);										
//		   //����key����
//    xTaskCreate((TaskFunction_t )key_task,             
//                (const char*    )"key_task",           
//                (uint16_t       )TASK3_STK_SIZE,        
//                (void*          )NULL,                  
//                (UBaseType_t    )TASK3_TASK_PRIO,        
//                (TaskHandle_t*  )&Task3Task_Handler);	  
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//����1���� 
void rc522_task(void *pvParameters)
{	
unsigned char	status;
unsigned char	ID[4];
unsigned char CT[2];//������0
unsigned char kahao1[4] = {0xA5, 0xA5, 0xA5, 0xA5};
u8		KEY[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	char cStr[4];
	
    while(1)
		{
				vTaskDelay(30);
		  	status	= PcdRequest( PICC_REQALL, CT );                /* Ѱ�� */
					if ( status != MI_OK )                                  /* Ѱ���ɹ� */
					{
						continue;
					}
					status = PcdAnticoll( ID );                             /* ����ײ */
					if ( status != MI_OK )                                  /* ����ײ�ɹ� */
					{
						ShowID(ID);
					}
					status = PcdSelect( ID );                               /*ѡ����Ƭ */
					if ( status != MI_OK )                                  /*ѡ����Ƭ�ɹ� */
					{	
							sprintf ( cStr, "%02X%02X%02X%02X",
               ID [ 0 ],
               ID [ 1 ],
               ID [ 2 ], 
               ID [ 3 ] );
						   printf("%s",cStr);
							 
			       if(ID[0]==kahao1[ 0 ]&&ID[ 1 ]==kahao1[ 1 ]&&ID[ 2 ]==kahao1[ 2 ]&&ID[ 3 ]==kahao1[ 3 ])
							{
							printf("\n");
						  printf("%s\n","ʶ��ɹ������ǺϷ��û�");
							//�û�����
							beep=1;
							vTaskDelay(200);
							beep=0;
							vTaskDelay(200);
							beep=1;
							vTaskDelay(200);
							beep=0;
							vTaskDelay(200);
							xEventGroupSetBits(EventGroupHandler,EVENTBIT_0);
							vTaskDelay(1500);
							OLED_Clear();
							zhu_1();
							vTaskDelay(200);
//								xTimerReset(BacklightTimer_Handle, 3);
							}
							else
							{
									OLED_Clear();
									open_error(); //�������
									vTaskDelay(300);
									beep = 0;
									vTaskDelay(1000);
									beep = 1;
									vTaskDelay(3000);
									beep = 0;
									vTaskDelay(300);
									OLED_Clear();
									zhu_1();
									vTaskDelay(200);
//								xTimerReset(BacklightTimer_Handle, 3);
							}
						continue;
					}
          status = PcdAuthState( PICC_AUTHENT1A, 3, KEY, ID );   /* ��֤��Ƭ���� */
				  if ( status != MI_OK )                                  /* ��֤��Ƭ����ɹ� */
				  {
						continue;
				  }

	}
}


//����2����
void button_task(void *pvParameters)
{
	u32 i = 0;
	u8 func_index = 0; 
	void (*current_operation_index)();
	typedef struct
{
	u8 current;
	u8 up; //���Ϸ������� 
	u8 down; //���·������� 
	u8 back; //������ҳ
	u8 enter;//ȷ��������	
	void (*current_operation)(); 
}key_table;

key_table  code_table[10]=
{ 	
  {0,0,0,0,0,(*zhu_1)},  //������ 
	{1,0,0,0,0,(*open_door)}, //��������

};
	                                  
 	u8 password_buf[6]={0}; 
	u8 password[6]="123456";
	
    while(1)
    {
			LED1=0;
			vTaskDelay(200);
			LED1=1;
			vTaskDelay(800);
			value = GetMatrixKeyValue();
		 if(value != 8 && value != 12 && value != 13&& value != 15 && value != 16 && value != 0)   	//�����뿪��
		 {	
				u32 j = 0;
				u32 tmp1 = 0;
				u8 ret = value;
				func_index = 1; //��������
				i++;
				if(i == 1)
				{
					OLED_Clear();
				}
				password_buf[i-1] = key_oled(value);
				OLED_ShowChar(7+(i-1)*20,34,password_buf[i-1],16,1);
				OLED_Refresh();//�����Դ�
				while(ret == value)
				{	
					//BEEP
					beep = 1;
					value = GetMatrixKeyValue();
					current_operation_index=code_table[func_index].current_operation;     
					(*current_operation_index)();//ִ�е�ǰ��������
				}
				beep = 0;
				if(i == 6)
				{
						//�ж������Ƿ���ȷ
						for(j = 0; j < 6; j++)
						{
							if(password_buf[j] == password[j])
							{
									tmp1++;
							}
							password_buf[j] = '0'; //buf ���
						}
				
						if(tmp1 == 6)   //�û�����
						{
							OpenFlag=1;
							OLED_Clear();
							oped_ok();//OLED
							delay_ms(300);
							beep = 1;
							delay_ms(200);
							beep = 0;
							delay_ms(200);
							beep = 1;
							delay_ms(200);
							beep = 0;
							delay_ms(100);
							xEventGroupSetBits(EventGroupHandler,EVENTBIT_1);
							vTaskDelay(1500);
							func_index = 0;
						}
						else
						{
							OLED_Clear();
							open_error(); //�������
							delay_ms(300);
							beep = 0;
							delay_ms(1000);
							beep = 1;
							delay_ms(3000);
							beep = 0;
							func_index = 0;
						}
						i=0;
						OLED_Clear();
						zhu_1();
						delay_ms(200);
						xTimerReset(BacklightTimer_Handle, 3);
						vTaskDelay(200);
				}	
	  }
	}
}


//�¼���־�鴦������g90�����
void eventgroup_task(void *pvParameters)
{
	EventBits_t EventValue;
	while(1)
	{
		if(OpenFlag)
		{
			 OpenFlag=0;
		   Server_Start();
		}
		vTaskDelay(200);
	}
}


	
//oled��Ļ��������
void oled_task(void *pvParameters)
{
	EventBits_t EventValue;
	while(1)
	{
		if(EventGroupHandler!=NULL)
		{
			//�ȴ��¼����е���Ӧ�¼�λ
			EventValue=xEventGroupWaitBits((EventGroupHandle_t	)EventGroupHandler,		
										   (EventBits_t			)EVENTBIT_ALL,
										   (BaseType_t			)pdTRUE,				
										   (BaseType_t			)pdFALSE,
								           (TickType_t			)portMAX_DELAY);	
			printf("�¼���־���ֵ:%d\r\n",EventValue); 
								OLED_Clear();
						  	oped_ok();//OLED
								vTaskDelay(10);
		}
		else
		{
			vTaskDelay(10); //��ʱ10ms��Ҳ����10��ʱ�ӽ���
		}
	}
}

void bluetooth_task(void *pvParameters)
{
	u8 x;
	while(1)
	{
		if((Receive_over==1)&&(Open_door==1))  //����
		{
				Receive_over=0;
				Open_door=0;
				OLED_Clear();
			  OpenFlag=1;
				oped_ok();//OLED
		}
		else if((Receive_over==1)&&(Close_door==1))     //����
		{
				Receive_over=0;
		   	Close_door=0;
		}
		vTaskDelay(10);

	}
}

void esp8266_task(void *pvParameters)
{
	char a[15];
	while(1)
	{
		if(USART3_RX_STA&0x8000)
		{
			printf("USART3_RX_BUF=%s\r\n",USART3_RX_BUF);
			sprintf(a,"%s",USART3_RX_BUF);
			printf("a=%s",a);
			if(strstr((const char*)a,"on"))  GPIO_ResetBits(GPIOB,GPIO_Pin_5);
				
			if(strstr((const char*)a,"off")) GPIO_SetBits(GPIOB,GPIO_Pin_5);
			
			if(strstr((const char*)a,"open"))TIM_SetCompare1(TIM3,185);

			if(strstr((const char*)a,"close"))TIM_SetCompare1(TIM3, 195);//0�� ��λ
				vTaskDelay(10);
			USART3_RX_STA=0;
		}
	vTaskDelay(10);
	}
}



void AS608_task(void *pvParameters)
{
	while(1)
	{
		if(PS_Sta)	 //���PS_Sta״̬���������ָ����
		{
			press_FR();//ˢָ��			
		}			
	   vTaskDelay(100);
	}
}
