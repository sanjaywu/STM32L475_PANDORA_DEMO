#include "delay.h"
#include "key.h"


/**************************************************************
函数名称 : key_init
函数功能 : IoT_Board 按键初始化
输入参数 : 无
返回值  	 : 无
备注		 : KEY0：PD10
		   KEY1：PD9
		   KEY2：PD8
		   WK_UP:PC13
**************************************************************/
void key_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();  

	/*Configure GPIO pin : PC13 */
  	GPIO_InitStruct.Pin = GPIO_PIN_13;
  	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  	GPIO_InitStruct.Pull = GPIO_PULLUP;
  	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pins : PD8 PD9 PD10 */
  	GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;
  	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

/**************************************************************
函数名称 : key_scan
函数功能 : IoT_Board 按键扫描
输入参数 : mode：1 --> 支持连按，0 --> 不支持连按
返回值  	 : 按键值
备注		 : 无
**************************************************************/
u8 key_scan(u8 mode)
{
    static u8 key_up = 1;   /* 按键松开标志 */

    if(mode == 1)
	{	
		key_up = 1; 		/* 支持连按 */
    }
    if(key_up && (KEY0 == 0 || KEY1 == 0 || KEY2 == 0 || WK_UP == 1))
    {
        delay_ms(10);
        key_up = 0;

        if(KEY0 == 0)
        {
			return KEY0_PRES;
        }
        else if(KEY1 == 0)
        {
			return KEY1_PRES;
        }
        else if(KEY2 == 0)
        {
			return KEY2_PRES;
        }
        else if(WK_UP == 1)
        {
			return WKUP_PRES;
        }
    }
    else if(KEY0 == 1 && KEY1 == 1 && KEY2 == 1 && WK_UP == 0)
	{
		key_up = 1;
    }

    return NO_PRES;   /* 无按键按下 */
}




















