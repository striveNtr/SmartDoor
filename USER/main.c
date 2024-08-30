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

//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		128  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define RC522_TASK_PRIO		2
//任务堆栈大小	
#define RC522_STK_SIZE 		128  
//任务句柄
TaskHandle_t RC522Task_Handler;
//任务函数
void rc522_task(void *pvParameters);

//任务优先级
#define BUTTON_TASK_PRIO		3
//任务堆栈大小	
#define BUTTON_STK_SIZE 		128  
//任务句柄
TaskHandle_t ButtonTask_Handler;
//任务函数
void button_task(void *pvParameters);

//舵机任务（事件标志组处理任务）
//任务优先级
#define EVENTGROUP_TASK_PRIO		4
//任务堆栈大小	
#define EVENTGROUP_STK_SIZE 		128 
//任务句柄
TaskHandle_t EventGroupTask_Handler;
//任务函数
void eventgroup_task(void *pvParameters);

//oled任务
//任务优先级
#define OLED_TASK_PRIO		5
//任务堆栈大小	
#define OLED_STK_SIZE 		128 
//任务句柄
TaskHandle_t OledTask_Handler;
//任务函数
void oled_task(void *pvParameters);

//蓝牙任务
//任务优先级
#define BLUETOOTH_TASK_PRIO		6
//任务堆栈大小	
#define BLUETOOTH_STK_SIZE 		128 
//任务句柄
TaskHandle_t BluetoothTask_Handler;
//任务函数
void bluetooth_task(void *pvParameters);

//esp8266任务
//任务优先级
#define ESP8266_TASK_PRIO		7
//任务堆栈大小	
#define ESP8266_STK_SIZE 		128 
//任务句柄
TaskHandle_t Esp8266Task_Handler;
//任务函数
void esp8266_task(void *pvParameters);


//指纹识别任务
//任务优先级
#define AS608_TASK_PRIO		  8
//任务堆栈大小	
#define AS608_STK_SIZE 		128 
//任务句柄
TaskHandle_t AS608Task_Handler;
//任务函数
void AS608_task(void *pvParameters);

////任务挂起（key可以控制rfid和按键） 任务
////任务优先级
//#define TASK3_TASK_PRIO		7
////任务堆栈大小	
//#define TASK3_STK_SIZE 		128 
////任务句柄
//TaskHandle_t Task3Task_Handler;
////任务函数
//void key_task(void *pvParameters);


EventGroupHandle_t EventGroupHandler;	//事件标志组句柄
 
#define EVENTBIT_0	(1<<0)				//事件位
#define EVENTBIT_1	(1<<1)
#define EVENTBIT_ALL	(EVENTBIT_0|EVENTBIT_1)//密码正确

u8 Pwd2[]={49,50,51,52,53,54};  //解锁密码2
TimerHandle_t	 BacklightTimer_Handle;			//单次定时器句柄
void BacklightTimerCallback(TimerHandle_t xTimer);		//单次定时器回调函数

u8 value = 0; 
u8 OpenFlag=0;


void Server_Start(void)
{
	TIM_SetCompare1(TIM8,25);//180度
	delay_ms(1000);	
}

u8 ensure;
u16 ValidN;//模块内有效指纹个数
SysPara AS608Para;//指纹模块AS608参数

//刷指纹
void press_FR(void)
{
	SearchResult seach;
	u8 ensure;
	char *str;
	ensure=PS_GetImage();
	if(ensure==0x00)//获取图像成功 
	{	
		//BEEP=1;//打开蜂鸣器	
		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //生成特征成功
		{		
		//	BEEP=0;//关闭蜂鸣器	
			ensure=PS_HighSpeedSearch(CharBuffer1,0,AS608Para.PS_max,&seach);
			if(ensure==0x00)//搜索成功
			{				
				delay_ms(400);	  
			}				
	  }
	}
}

int main(void)
{
	delay_init();	    				//延时函数初始化	  
	uart_init(115200);					//初始化串口
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	 
	uart4_init(9600);
	TIM8_PWM_Init();
	usart2_init(115200);//初始化串口2,用于与指纹模块通讯 
	PS_StaGPIO_Init();	//初始化FR读状态引脚
	delay_ms(100);	 
	while(PS_HandShake(&AS608Addr))//与AS608模块握手
	{
			delay_ms(400);	  
	}
	ensure=PS_ValidTempleteNum(&ValidN);//读库指纹个数
	ensure=PS_ReadSysPara(&AS608Para);  //读参数 
	OLED_Init(); 
	OLED_ColorTurn(0);//0正常显示，1 反色显示
  OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示
	delay_ms(200);
//	BEEP_Init();
	RC522_Init();
//	usart3_init(115200);	 				//串口初始化为11520 esp8266
////	esp8266_start_trans();							//esp8266进行初始化 
	oled_start(); //开机显示
	zhu_1();
	GetMatrixKeyValue();
	//创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
}

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区   
    //创建事件标志组
	EventGroupHandler=xEventGroupCreate();	 //创建事件标志组
	
	  //创建RFID卡任务
    xTaskCreate((TaskFunction_t )rc522_task,     
                (const char*    )"rc522_task",   
                (uint16_t       )RC522_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )RC522_TASK_PRIO,
                (TaskHandle_t*  )&RC522Task_Handler);  
    //创建4*4button按键任务
    xTaskCreate((TaskFunction_t )button_task,     	
                (const char*    )"button_task",   	
                (uint16_t       )BUTTON_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )BUTTON_TASK_PRIO,	
                (TaskHandle_t*  )&ButtonTask_Handler);   
      
    //创建事件标志组处理任务
//    xTaskCreate((TaskFunction_t )eventgroup_task,             
//                (const char*    )"eventgroup_task",           
//                (uint16_t       )EVENTGROUP_STK_SIZE,        
//                (void*          )NULL,                  
//                (UBaseType_t    )EVENTGROUP_TASK_PRIO,        
//                (TaskHandle_t*  )&EventGroupTask_Handler);	
    //创建OLED任务
    xTaskCreate((TaskFunction_t )oled_task,             
                (const char*    )"oled_task",           
                (uint16_t       )OLED_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )OLED_TASK_PRIO,        
                (TaskHandle_t*  )&OledTask_Handler);
    //创建蓝牙任务
    xTaskCreate((TaskFunction_t )bluetooth_task,             
                (const char*    )"bluetooth_task",           
                (uint16_t       )BLUETOOTH_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )BLUETOOTH_TASK_PRIO,        
                (TaskHandle_t*  )&BluetoothTask_Handler);				
    //创建esp8266任务
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
//		   //创建key任务
//    xTaskCreate((TaskFunction_t )key_task,             
//                (const char*    )"key_task",           
//                (uint16_t       )TASK3_STK_SIZE,        
//                (void*          )NULL,                  
//                (UBaseType_t    )TASK3_TASK_PRIO,        
//                (TaskHandle_t*  )&Task3Task_Handler);	  
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//任务1函数 
void rc522_task(void *pvParameters)
{	
unsigned char	status;
unsigned char	ID[4];
unsigned char CT[2];//卡类型0
unsigned char kahao1[4] = {0xA5, 0xA5, 0xA5, 0xA5};
u8		KEY[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	char cStr[4];
	
    while(1)
		{
				vTaskDelay(30);
		  	status	= PcdRequest( PICC_REQALL, CT );                /* 寻卡 */
					if ( status != MI_OK )                                  /* 寻卡成功 */
					{
						continue;
					}
					status = PcdAnticoll( ID );                             /* 防冲撞 */
					if ( status != MI_OK )                                  /* 防冲撞成功 */
					{
						ShowID(ID);
					}
					status = PcdSelect( ID );                               /*选定卡片 */
					if ( status != MI_OK )                                  /*选定卡片成功 */
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
						  printf("%s\n","识别成功，您是合法用户");
							//用户开锁
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
									open_error(); //密码错误
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
          status = PcdAuthState( PICC_AUTHENT1A, 3, KEY, ID );   /* 验证卡片密码 */
				  if ( status != MI_OK )                                  /* 验证卡片密码成功 */
				  {
						continue;
				  }

	}
}


//任务2函数
void button_task(void *pvParameters)
{
	u32 i = 0;
	u8 func_index = 0; 
	void (*current_operation_index)();
	typedef struct
{
	u8 current;
	u8 up; //向上翻索引号 
	u8 down; //向下翻索引号 
	u8 back; //返回首页
	u8 enter;//确认索引号	
	void (*current_operation)(); 
}key_table;

key_table  code_table[10]=
{ 	
  {0,0,0,0,0,(*zhu_1)},  //主界面 
	{1,0,0,0,0,(*open_door)}, //输入密码

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
		 if(value != 8 && value != 12 && value != 13&& value != 15 && value != 16 && value != 0)   	//按密码开锁
		 {	
				u32 j = 0;
				u32 tmp1 = 0;
				u8 ret = value;
				func_index = 1; //密码输入
				i++;
				if(i == 1)
				{
					OLED_Clear();
				}
				password_buf[i-1] = key_oled(value);
				OLED_ShowChar(7+(i-1)*20,34,password_buf[i-1],16,1);
				OLED_Refresh();//更新显存
				while(ret == value)
				{	
					//BEEP
					beep = 1;
					value = GetMatrixKeyValue();
					current_operation_index=code_table[func_index].current_operation;     
					(*current_operation_index)();//执行当前操作函数
				}
				beep = 0;
				if(i == 6)
				{
						//判断密码是否正确
						for(j = 0; j < 6; j++)
						{
							if(password_buf[j] == password[j])
							{
									tmp1++;
							}
							password_buf[j] = '0'; //buf 清空
						}
				
						if(tmp1 == 6)   //用户开锁
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
							open_error(); //密码错误
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


//事件标志组处理任务（g90舵机）
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


	
//oled屏幕处理任务
void oled_task(void *pvParameters)
{
	EventBits_t EventValue;
	while(1)
	{
		if(EventGroupHandler!=NULL)
		{
			//等待事件组中的相应事件位
			EventValue=xEventGroupWaitBits((EventGroupHandle_t	)EventGroupHandler,		
										   (EventBits_t			)EVENTBIT_ALL,
										   (BaseType_t			)pdTRUE,				
										   (BaseType_t			)pdFALSE,
								           (TickType_t			)portMAX_DELAY);	
			printf("事件标志组的值:%d\r\n",EventValue); 
								OLED_Clear();
						  	oped_ok();//OLED
								vTaskDelay(10);
		}
		else
		{
			vTaskDelay(10); //延时10ms，也就是10个时钟节拍
		}
	}
}

void bluetooth_task(void *pvParameters)
{
	u8 x;
	while(1)
	{
		if((Receive_over==1)&&(Open_door==1))  //开锁
		{
				Receive_over=0;
				Open_door=0;
				OLED_Clear();
			  OpenFlag=1;
				oped_ok();//OLED
		}
		else if((Receive_over==1)&&(Close_door==1))     //关锁
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

			if(strstr((const char*)a,"close"))TIM_SetCompare1(TIM3, 195);//0度 复位
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
		if(PS_Sta)	 //检测PS_Sta状态，如果有手指按下
		{
			press_FR();//刷指纹			
		}			
	   vTaskDelay(100);
	}
}
