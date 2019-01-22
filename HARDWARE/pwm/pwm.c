#include "pwm.h"


TIM_HandleTypeDef htim2;

/**************************************************************
函数名称 : timer2_pwm_init
函数功能 : 定时器2 pwm初始化
输入参数 : prescaler：时钟分频系数，period：自动重载值
返回值  	 : 无
备注		 : 无
**************************************************************/
void timer2_pwm_init(u32 prescaler, u32 period)
{
	TIM_OC_InitTypeDef sConfigOC;

  	htim2.Instance = TIM2;
  	htim2.Init.Prescaler = prescaler;
  	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  	htim2.Init.Period = period;
  	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

  	if(HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  	{
    	printf("pwm init failed\r\n");
  	}

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_ENABLE;
	if(HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  	{
    	printf("pwm ConfigChanne1 failed\r\n");
  	}
  	if(HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  	{
    	printf("pwm ConfigChanne2 failed\r\n");;
  	}
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1); 	/* 开启PWM通道1 */
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); 	/* 开启PWM通道2 */
}

/**************************************************************
函数名称 : HAL_TIM_MspPostInit
函数功能 : 定时器底层驱动，时钟使能，引脚配置
输入参数 : htim：定时器句柄
返回值  	 : 无
备注		 : PA0 ------> TIM2_CH1，PA1 ------> TIM2_CH2 
**************************************************************/
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* htim)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
  	if(htim->Instance==TIM2)
  	{
	    __HAL_RCC_TIM2_CLK_ENABLE();
	    __HAL_RCC_GPIOA_CLK_ENABLE();
	    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
	    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  }

}

/**************************************************************
函数名称 : set_tim2_channel_1_compare
函数功能 : 设置TIM2通道1的占空比
输入参数 : compare：比较值
返回值  	 : 无
备注		 : 无
**************************************************************/
void set_tim2_channel_1_compare(u32 compare)
{
    TIM2->CCR1 = compare;
}

/**************************************************************
函数名称 : set_tim1_channel_2_compare
函数功能 : 设置TIM2通道2的占空比
输入参数 : compare：比较值
返回值  	 : 无
备注		 : 无
**************************************************************/
void set_tim1_channel_2_compare(u32 compare)
{
    TIM2->CCR2 = compare;
}
























