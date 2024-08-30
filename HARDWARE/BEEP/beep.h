#ifndef _BEEP_H_
#define _BEEP_H_


#include "sys.h"



//STM32F103ZE核心板
//蜂鸣器驱动代码	   
				  
////////////////////////////////////////////////////////////////////////////////// 
//蜂鸣器端口定义
#define beep PBout(8)	// BEEP,蜂鸣器接口		   

void BEEP_Init(void);	//初始化
		 				    
#endif
