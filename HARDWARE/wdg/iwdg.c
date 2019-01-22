#include "iwdg.h"


IWDG_HandleTypeDef hiwdg;

/**************************************************************
函数名称 : iwdg_init
函数功能 : 独立看门狗初始化
输入参数 : prescaler：预分频系数，reload：自动重装载值
返回值  	 : 无
备注		 : 时间计算(大概):Tout=((4*2^prer)*rlr)/32 (ms)
**************************************************************/
void iwdg_init(u32 prescaler, u32 reload)
{
	hiwdg.Instance = IWDG;
  	hiwdg.Init.Prescaler = prescaler;
	hiwdg.Init.Window = IWDG_WINDOW_DISABLE;
  	hiwdg.Init.Reload = reload;
	
  	if(HAL_IWDG_Init(&hiwdg) != HAL_OK)
  	{
    	printf("iwdg init failed\r\n");
  	}
}

/**************************************************************
函数名称 : iwdg_feed
函数功能 : 独立看门狗喂狗函数
输入参数 : 无
返回值  	 : 无
备注		 : 无
**************************************************************/
void iwdg_feed(void)
{
    HAL_IWDG_Refresh(&hiwdg);
}





















