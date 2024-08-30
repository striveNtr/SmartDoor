#include "key.h"
#include "delay.h"

void MatrixModeConfig(MatirxMode mode)//��ɨ��ʱ����Ҫ���ж�Ӧ��IO�����ó��������룬�����ó����������ȫ����ֵΪ0���ж�Ӧ��IO�ڼ���Ƿ��е͵�ƽ�����룬�Ӷ��ж�����һ�а����ˡ�
{
	GPIO_InitTypeDef GPIO;
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOC, ENABLE); //GPIOʹ��
    
	switch(mode)
	{
		case Column:
		//==============================================//
			GPIO.GPIO_Pin   = ROW1 | ROW2|ROW3|ROW4;
			GPIO.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO.GPIO_Mode  = GPIO_Mode_Out_PP;
			GPIO_Init(GPIOC, &GPIO);          //���������0
		
		  BIT_ADDR(GPIOC_ODR_Addr,4) = 0;
			BIT_ADDR(GPIOC_ODR_Addr,7) = 0;
			BIT_ADDR(GPIOC_ODR_Addr,8) = 0;
			BIT_ADDR(GPIOC_ODR_Addr,9) = 0;
		
		  GPIO.GPIO_Pin   =COL1 | COL2 |COL3 |COL4;
			GPIO.GPIO_Mode  = GPIO_Mode_IPU;            
			GPIO_Init(GPIOE, &GPIO);           //��ɨ��ģʽʱ������������        
			break;
			
		case Row:
			GPIO.GPIO_Pin   = COL1 | COL2|COL3|COL4;
			GPIO.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO.GPIO_Mode  = GPIO_Mode_Out_PP;
			GPIO_Init(GPIOE, &GPIO);           //��ɨ��ģʽʱ�������������������0
		
			BIT_ADDR(GPIOE_ODR_Addr,5) = 0;
			BIT_ADDR(GPIOE_ODR_Addr,2) = 0;
			BIT_ADDR(GPIOE_ODR_Addr,3) = 0;
			BIT_ADDR(GPIOE_ODR_Addr,4) = 0;
			GPIO.GPIO_Pin   =  ROW1|ROW2|ROW3|ROW4;
			GPIO.GPIO_Mode  = GPIO_Mode_IPU;
			GPIO_Init(GPIOC, &GPIO);
			break;
	}
}

uint8_t GetMatrixKeyValue(void)    //֧������
{
	uint8_t value1 = 0;
	uint8_t Rowvalue = 0;
	MatrixModeConfig(Row);//��ɨ��
	
	if (BIT_ADDR(GPIOC_IDR_Addr,4) == 0)
	{
		 delay_ms(5);
		if (BIT_ADDR(GPIOC_IDR_Addr,4) == 0)
		{
			value1 += 0;
		}
	}
	else if (BIT_ADDR(GPIOC_IDR_Addr,7) == 0)
	{
		 delay_ms(5);
		if (BIT_ADDR(GPIOC_IDR_Addr,7) == 0)
		{
			value1 += 4;
		}
	}
	else if (BIT_ADDR(GPIOC_IDR_Addr,8) == 0)
	{
		 delay_ms(5);
		if (BIT_ADDR(GPIOC_IDR_Addr,8) == 0)
		{
			value1 += 8;
		}
	}
	else if (BIT_ADDR(GPIOC_IDR_Addr,9) == 0)
	{
		 delay_ms(5);
		if (BIT_ADDR(GPIOC_IDR_Addr,9) == 0)
		{
			value1 +=12;
		}
	}
	else
	{
		value1 = 0;
	}
	Rowvalue=value1;
	
	//key.h �ж���ö�ٱ���
	MatrixModeConfig(Column);   //��ɨ��
	
	if (GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_5) == 0)
	{
		 delay_ms(5);
		if (GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_5) == 0)
		{
			value1 += 1;
		}
	}
	else if (GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2) == 0)
	{
		 delay_ms(5);
		if (GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2) == 0)
		{
			value1 += 2;
		}
	}
	else if (GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3) == 0)
	{
		 delay_ms(5);
		if (GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3) == 0)
		{
			value1 += 3;
		}
	}
	else if (GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4) == 0)
	{
		 delay_ms(5);
		if (GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4) == 0)
		{
			value1 += 4;
		}
	}
	else
	{
		value1 = 0;
	}
  if(value1==Rowvalue)
  {
		 return 0;
  }     //˵����ɨ��ʧ�ܣ����ܰ����Ѿ��ɿ�����ֹ��ɨ��ɹ��󰴼��ɿ���������ɨ��ʧ�ܵĴ��������
	return value1;
}


//���ݼ�ֵ��ȷ����ʾ����
u8 key_oled(u8 value)
{
	u8 key = 0;
	switch(value)
	{
			case 1:
				key = '1';
				break;
			case 2:
				key = '2';
				break;
			case 3:
				key = '3';
				break;
			case 4:
				key = 'A';
				break;
			case 5:
				key = '4';
				break;
			case 6:
				key = '5';
				break;
			case 7:
				key = '6';
				break;
			case 8:
				key = 'B';
				break;
			case 9:
				key = '7';
				break;
			case 10:
			  key = '8';
				break;
			case 11:
				key = '9';
				break;
			case 12:
				key = 'C';
				break;
			case 13:
				key = '*';
				break;
			case 14:
				key = '0';
				break;
			case 15:
				key = '#';
				break;
			case 16:
				key = 'D';
				break;
	}
	return key;
}


