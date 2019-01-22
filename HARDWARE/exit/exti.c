#include "exti.h"
#include "key.h"
#include "delay.h"

/**************************************************************
函数名称 : exti_init
函数功能 : 外部中断初始化函数
输入参数 : 无
返回值  	 : 无
备注		 : 无
**************************************************************/
void exti_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

  	/* GPIO Ports Clock Enable */
  	__HAL_RCC_GPIOC_CLK_ENABLE();

  	/*Configure GPIO pin : PC13 */
  	GPIO_InitStruct.Pin = GPIO_PIN_13;
  	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  	/* EXTI interrupt init*/
  	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
  	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

#if 0
/**************************************************************
函数名称 : EXTI15_10_IRQHandler
函数功能 : EXTI15_10中断服务函数
输入参数 : 无
返回值  	 : 无
备注		 : 无
**************************************************************/
void EXTI15_10_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);	/* 调用中断处理公用函数 */
}
 
/**************************************************************
函数名称 : HAL_GPIO_EXTI_Callback
函数功能 : 外部中回调函数
输入参数 : GPIO_Pin：中断引脚号
返回值  	 : 无
备注		 : 无
**************************************************************/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    delay_ms(50);      /* 消抖 */
    switch(GPIO_Pin)
	{
		case  GPIO_PIN_13:
		{
			if(WK_UP == 1)
			{
				printf("IoT Board EXTI\r\n");
			}
			break;
		}	
		default:
			break;
	}
}

#endif





















