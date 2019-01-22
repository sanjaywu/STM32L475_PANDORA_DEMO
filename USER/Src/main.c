#include "main.h"
#include "string.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "usmart.h"
#include "led.h"
#include "key.h"
#include "exti.h"
#include "iwdg.h"
#include "timer.h"
#include "lcd.h"
#include "font.h"
#include "rtc.h"
#include "low_power.h"
#include "wkup.h"
#include "aht10.h"
#include "w25qxx.h"

/***********************************************************************
(W25Q128)资源分配
序号	 	偏移地址				大小				存储内容
1		0x00000000		0x00200000		预留给RT Thread使用(2M)
2		0x00200000		0x00603000		预留给中文字库使用(6156K)
3		0x00803000		0x000FD000		预留给用户使用(1012K)
4		0x00900000		0x00700000		预留给FATFS使用(7M)
注意：在测试本例程时，请不要随意擦除扇区数据。
************************************************************************/

void hardware_init(void);

int main(void)
{	
	u8 datatemp[50];
	
	hardware_init();
	printf("IoT Board PANDORA\r\n");
	lcd_clear(WHITE);
	lcd_show_string(66, 0, 108, 24, "IoT Board", 24, 1);

	printf("flash_id:0x%X\r\n", g_w25qxx_device_id);
	while(1)
	{
		switch(key_scan(0))
		{
			case KEY0_PRES:
			{
				printf("w25qxx write\r\n");
				w25qxx_write("IoT_Board", 0x00803000, sizeof("IoT_Board"));
				break;
			}
			case KEY1_PRES:
			{
				printf("w25qxx read\r\n");
				w25qxx_read(datatemp, 0x00803000, sizeof("IoT_Board"));
				printf("datatemp:%s\r\n", datatemp);
				break;
			}
			default:
				break;
		}
	}
}

/**************************************************************
函数名称 : hardware_init
函数功能 : 硬件初始化
输入参数 : 无
返回值  	 : 无
备注		 : 包括hal库初始化、系统时钟初始化、延时函数初始化、
		   片级初始化、板级外设初始化
**************************************************************/
void hardware_init(void)
{
	HAL_Init();
  	SystemClock_Config();
	delay_init(80);
	uart_init(115200);
	usmart_dev.init(80);	/* 初始化USMART，80M系统时钟 */
	//wkup_init();
	led_init();
	key_init();
	//iwdg_init(IWDG_PRESCALER_64, 500);
	//timer3_init(8000 - 1, 8000 - 1);
	lcd_init();
	rtc_init();
	aht10_init();
	w25qxx_init();
}





