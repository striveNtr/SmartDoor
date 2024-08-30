#include "oled.h"
#include "bmp.h"
#include "delay.h"
//#include "key.h"

//?
void oled_start()
{
	int i = 0;
	for(i = 0; i < 3; i++)
	{
		OLED_ShowPicture(0,0,128,64,start,1);
		OLED_Refresh();
		delay_ms(300);
		
		OLED_DrawPoint(30,58,1);
		OLED_Refresh();
		delay_ms(300);
		OLED_DrawPoint(55,58,1);
		OLED_Refresh();
		delay_ms(300);
		OLED_DrawPoint(75,58,1);
		OLED_Refresh();
		delay_ms(300);
		OLED_DrawPoint(95,58,1);
		OLED_Refresh();
		delay_ms(300);
		
		OLED_DrawPoint(30,58,0);
		OLED_DrawPoint(55,58,0);
		OLED_DrawPoint(75,58,0);
		OLED_DrawPoint(95,58,0);
		OLED_Refresh();
	}
	OLED_Clear();
}


//1
void zhu_1()
{
	OLED_ShowPicture(0,0,128,16,zhu1,1);
	
	OLED_ShowChinese(25,30,5,16,1);//
	OLED_ShowChinese(45,30,6,16,1);//?
	OLED_ShowChinese(65,30,7,16,1);//
	OLED_ShowChinese(85,30,8,16,1);//
	OLED_Refresh();
}


//?
void open_door()
{
		OLED_ShowChinese(20,0,0,16,1);//
		OLED_ShowChinese(40,0,1,16,1);//
		OLED_ShowChinese(60,0,2,16,1);//
		OLED_ShowChinese(80,0,3,16,1);//
		OLED_ShowChinese(100,0,4,16,1);//
		
		OLED_DrawLine(5,50,21,50,1); //
		OLED_DrawLine(25,50,41,50,1);
		OLED_DrawLine(45,50,61,50,1);
		OLED_DrawLine(65,50,81,50,1);
		OLED_DrawLine(85,50,101,50,1);
		OLED_DrawLine(105,50,121,50,1);
		OLED_Refresh();
}

	

//?
void oped_ok()  //¿ªËø³É¹¦
{
	OLED_ShowChinese(20,30,9,16,1);//
	OLED_ShowChinese(40,30,10,16,1);//
	OLED_ShowChinese(60,30,11,16,1);//
	OLED_ShowChinese(80,30,12,16,1);//
	OLED_ShowChinese(100,30,13,16,1);//
	OLED_Refresh();
}


//
void open_error() //ÃÜÂë´íÎó
{
	OLED_ShowChinese(20,30,14,16,1);//
	OLED_ShowChinese(40,30,15,16,1);//
	OLED_ShowChinese(60,30,16,16,1);//
	OLED_ShowChinese(80,30,17,16,1);//
	OLED_ShowChinese(100,30,18,16,1);//
	OLED_Refresh();
}


//??
void dht11_show(u32 *buf)  //ÎÂÊª¶È
{
	u32 pos = 0;
	
	OLED_ShowPicture(0,0,128,16,zhu1,1);
	
	OLED_ShowChinese(20,20,24,16,1);//
	OLED_ShowChinese(40,20,25,16,1);//
	OLED_ShowString(60,20,":",16,1);
	
	if(buf[0] < 10)
	{
		OLED_ShowNum(70,20,buf[0],1,16,1);
		pos = 8;
	}
	else if(buf[0] < 100)
	{
		OLED_ShowNum(70,20,buf[0],2,16,1);
		pos = 16;
	}
	else
	{
		OLED_ShowNum(70,20,buf[0],3,16,1);
		pos = 24;
	}
	OLED_ShowString(70+pos,20,".",16,1);
	pos += 8;
	if(buf[1] < 10)
	{
		OLED_ShowNum(70+pos,20,buf[1],1,16,1);
	}
	else if(buf[1] < 100)
	{
		OLED_ShowNum(70+pos,20,buf[1],2,16,1);
	}
	else
	{
		OLED_ShowNum(70+pos,20,buf[1],3,16,1);
	}
	
		
	OLED_ShowChinese(20,40,26,16,1);//?
	OLED_ShowChinese(40,40,27,16,1);//
	OLED_ShowString(60,40,":",16,1);
	
	if(buf[2] < 10)
	{
		OLED_ShowNum(70,40,buf[2],1,16,1);
		pos = 8;
	}
	else if(buf[2] < 100)
	{
		OLED_ShowNum(70,40,buf[2],2,16,1);
		pos = 16;
	}
	else
	{
		OLED_ShowNum(70,40,buf[2],3,16,1);
		pos = 24;
	}
	OLED_ShowString(70+pos,40,".",16,1);
	pos += 8;
	if(buf[3] < 10)
	{
		OLED_ShowNum(70+pos,40,buf[3],1,16,1);
	}
	else if(buf[3] < 100)
	{
		OLED_ShowNum(70+pos,40,buf[3],2,16,1);
	}
	else
	{
		OLED_ShowNum(70+pos,40,buf[3],3,16,1);
	}
	OLED_Refresh();

}


void root_show()
{
	OLED_ShowString(15,0,"Welcome to root",12,1);
	OLED_ShowChinese(15,30,19,16,1);//
	OLED_ShowChinese(35,30,20,16,1);//
	OLED_ShowChinese(55,30,21,16,1);//
	OLED_ShowChinese(75,30,22,16,1);//?
	OLED_ShowChinese(95,30,23,16,1);//?
	OLED_Refresh();

}
	
void root_menu1()
{
	OLED_ShowChinese(55,0,28,16,1);//
	OLED_ShowChinese(70,0,29,16,1);//
	
	OLED_ShowString(0,16,"->",12,1);
	OLED_ShowString(16,16,"UserPassword",12,1);
	OLED_ShowString(16,28,"RootPassword",12,1);
	OLED_ShowString(16,40,"Time",12,1);
	OLED_ShowString(16,52,"Led",12,1);
	
	OLED_Refresh();
}

void root_menu2()
{
	OLED_ShowChinese(55,0,28,16,1);//
	OLED_ShowChinese(70,0,29,16,1);//
	
	OLED_ShowString(16,16,"UserPassword",12,1);
	OLED_ShowString(0,28,"->",12,1);
	OLED_ShowString(16,28,"RootPassword",12,1);
	OLED_ShowString(16,40,"Time",12,1);
	OLED_ShowString(16,52,"Led",12,1);
	
	OLED_Refresh();
}

void root_menu3()
{
	OLED_ShowChinese(55,0,28,16,1);//
	OLED_ShowChinese(70,0,29,16,1);//
	
	OLED_ShowString(16,16,"UserPassword",12,1);
	OLED_ShowString(16,28,"RootPassword",12,1);
	OLED_ShowString(0,40,"->",12,1);
	OLED_ShowString(16,40,"Time",12,1);
	OLED_ShowString(16,52,"Led",12,1);
	
	OLED_Refresh();
}

void root_menu4()
{
	OLED_ShowChinese(55,0,28,16,1);//
	OLED_ShowChinese(70,0,29,16,1);//
	
	OLED_ShowString(16,16,"UserPassword",12,1);
	OLED_ShowString(16,28,"RootPassword",12,1);
	OLED_ShowString(16,40,"Time",12,1);
	OLED_ShowString(0,52,"->",12,1);
	OLED_ShowString(16,52,"Led",12,1);
	
	OLED_Refresh();
}

