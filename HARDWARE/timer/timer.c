#include "timer.h"
#include "iwdg.h"

TIM_HandleTypeDef htim3;

/**************************************************************
函数名称 : timer3_init
函数功能 : 定时器3初始化
输入参数 : prescaler：时钟分频系数，period：自动重载值
返回值  	 : 无
备注		 : 定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
		   Ft=定时器工作频率,单位:Mhz
		   定时器3挂在APB1上，时钟为HCLK
**************************************************************/
void timer3_init(u32 prescaler, u32 period)
{
  	htim3.Instance = TIM3;
  	htim3.Init.Prescaler = prescaler;
  	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;				/* 向上计数 */
  	htim3.Init.Period = period;
  	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;/* 使能自动重载 */
	
  	if(HAL_TIM_Base_Init(&htim3) != HAL_OK)
  	{
    	printf("timer3 init failed\r\n");
  	}
	HAL_TIM_Base_Start_IT(&htim3);
}

/**************************************************************
函数名称 : HAL_TIM_Base_MspInit
函数功能 : 定时器底层驱动，开启时钟，设置中断优先级
输入参数 : htim：定时器句柄
返回值  	 : 无
备注		 : 此函数会被HAL_TIM_Base_Init()函数调用
**************************************************************/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
	if(htim_base->Instance==TIM3)
  	{
	  	__HAL_RCC_TIM3_CLK_ENABLE();
	    HAL_NVIC_SetPriority(TIM3_IRQn, 2, 0);
	    HAL_NVIC_EnableIRQ(TIM3_IRQn);
	}
}

/**************************************************************
函数名称 : TIM3_IRQHandler
函数功能 : 定时器3中断服务函数
输入参数 : 无
返回值  	 : 无
备注		 : 无
**************************************************************/
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim3);
}

/**************************************************************
函数名称 : HAL_TIM_PeriodElapsedCallback
函数功能 : 回调函数，定时器中断服务函数调用
输入参数 : htim：定时器句柄
返回值  	 : 无
备注		 : 无
**************************************************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim == (&htim3))
    {
    	iwdg_feed();
        //printf("timer3, iwdg_feed\r\n");
    }
}






















