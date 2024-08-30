#ifndef __KEY_H__
#define	__KEY_H__
#include "stm32f10x.h"
#include "sys.h"

#define		ROW1		GPIO_Pin_4
#define		ROW2		GPIO_Pin_7
#define		ROW3		GPIO_Pin_8
#define		ROW4		GPIO_Pin_9

#define		COL1		GPIO_Pin_5
#define		COL2		GPIO_Pin_2
#define		COL3		GPIO_Pin_3
#define		COL4		GPIO_Pin_4

typedef enum
{
	Column,
	Row,
	Normal
} MatirxMode;

extern void MatrixKeyConfiguration(void);
extern uint8_t GetMatrixKeyValue(void);
u8 key_oled(u8 value);

#endif
