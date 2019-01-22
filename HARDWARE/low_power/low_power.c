#include "low_power.h"
#include "rtc.h"

/**************************************************************
函数名称 : low_power_enter_standby_mode
函数功能 : 进入低功耗待机模式
输入参数 : 无
返回值  	 : 无
备注		 : STM32F4,当开启了RTC相关中断后,必须先关闭RTC中断,
		   再清中断标志位,然后重新设置RTC中断,
		   再进入待机模式才可以正常唤醒,否则会有问题.	
**************************************************************/
void low_power_enter_standby_mode(void)
{
	__HAL_RCC_AHB2_FORCE_RESET();       /* 复位所有IO口 */
	
	__HAL_RCC_PWR_CLK_ENABLE();         /* 使能PWR时钟 */
    __HAL_RCC_BACKUPRESET_FORCE();      /* 复位备份区域 */
    HAL_PWR_EnableBkUpAccess();         /* 后备区域访问使能 */
	
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
    __HAL_RTC_WRITEPROTECTION_DISABLE(&hrtc);/* 关闭RTC写保护 */
    
    /* 关闭RTC相关中断，可能在RTC实验打开了 */
    __HAL_RTC_WAKEUPTIMER_DISABLE_IT(&hrtc, RTC_IT_WUT);
    __HAL_RTC_TIMESTAMP_DISABLE_IT(&hrtc, RTC_IT_TS);
    __HAL_RTC_ALARM_DISABLE_IT(&hrtc, RTC_IT_ALRA|RTC_IT_ALRB);
    
    /* 清除RTC相关中断标志位 */
    __HAL_RTC_ALARM_CLEAR_FLAG(&hrtc, RTC_FLAG_ALRAF|RTC_FLAG_ALRBF);
    __HAL_RTC_TIMESTAMP_CLEAR_FLAG(&hrtc, RTC_FLAG_TSF); 
    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);
    
    __HAL_RCC_BACKUPRESET_RELEASE();			/* 备份区域复位结束 */
    __HAL_RTC_WRITEPROTECTION_ENABLE(&hrtc);	/* 使能RTC写保护 */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);			/* 清除Wake_UP标志 */
	
		
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN2);	/* 设置WKUP引脚用于唤醒 */
    HAL_PWR_EnterSTANDBYMode();					/* 进入待机模式 */
}


























