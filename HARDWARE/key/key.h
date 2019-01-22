#ifndef __KEY_H__
#define __KEY_H__

#include "sys.h"


#define KEY0        HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_10)
#define KEY1        HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_9)
#define KEY2        HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_8)
#define WK_UP       HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)

enum KEY_PRES
{
	NO_PRES   = 0,
	KEY0_PRES = 1,
	KEY1_PRES = 2,
	KEY2_PRES = 3,
	WKUP_PRES = 4
};

void key_init(void);
u8 key_scan(u8 mode);




#endif







