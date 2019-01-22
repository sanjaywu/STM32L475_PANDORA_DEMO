#include "wkup.h"
#include "delay.h"
#include "low_power.h"
#include "led.h"

u8 check_wkup(void)
{
	u8 press_cnt = 0;	/* 记录按下的次数 */
	u8 release_cnt = 0;	/* 记录松开的次数 */

	LED_R(0);
	while(1)			/* 死循环，由return结束 */
	{
		delay_ms(30);	/* 延迟一段时间再检测 */

		if(WKUP_READ)	/* 检测到按下按键 */
		{
			press_cnt++;		/* 记录按下次数 */
			release_cnt = 0;	/* 清除按键释放记录 */
			if(press_cnt >= 100)/* 按下时间足够 */
			{
				LED_R(1);
				return 1;		/* 检测到按键被时间长按下 */
			}
		}
		else
		{
			release_cnt++;		/* 记录释放次数 */
			if(release_cnt > 3)	/* 连续检测到释放超过3次 */
			{
				LED_R(0);
				return 0;		/* 按下时间太短，不是按键长按操作 */
			}
		}
	}
}


/**************************************************************
函数名称 : wkup_init
函数功能 : WK_UP唤醒初始化
输入参数 : 无
返回值  	 : 无
备注		 : 无
**************************************************************/
void wkup_init(void)
{
	 GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOC_CLK_ENABLE();
	
    GPIO_Initure.Pin = GPIO_PIN_13;
    GPIO_Initure.Mode = GPIO_MODE_IT_RISING;  /* 中断,上升沿 */
    GPIO_Initure.Pull = GPIO_PULLDOWN;
    GPIO_Initure.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(GPIOC, &GPIO_Initure);
    
	HAL_PWREx_EnableGPIOPullDown(PWR_GPIO_C, PWR_GPIO_BIT_13);
	HAL_PWREx_EnablePullUpPullDownConfig();

#if 1
    if(check_wkup()==0)
    {
        low_power_enter_standby_mode();/* 进入待机模式 */
    }
#endif

    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}



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
	if(GPIO_Pin == GPIO_PIN_13)
	{
        if(check_wkup())
        {
            low_power_enter_standby_mode();/* 进入待机模式 */
        }
	}
}














