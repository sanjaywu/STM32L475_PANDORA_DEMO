#include "rtc.h"
#include "low_power.h"

RTC_HandleTypeDef hrtc;
RTC_TimeTypeDef sTime;
RTC_DateTypeDef sDate;

/**************************************************************
函数名称 : rtc_init
函数功能 : RTC初始化
输入参数 : 无
返回值  	 : 无
备注		 : 无
**************************************************************/
void rtc_init(void)
{
    /* Initialize RTC Only */
	hrtc.Instance = RTC;
	hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
	hrtc.Init.AsynchPrediv = 127;
	hrtc.Init.SynchPrediv = 255;
	hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
	hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	
	if(HAL_RTC_Init(&hrtc) != HAL_OK)
	{
		printf("rtc init failed\r\n");
	}
	if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0) != 0x32F2)
	{
	    /* Initialize RTC and set the Time and Date */
	  	rtc_set_time(15, 50, 20, RTC_HOURFORMAT12_AM);
		rtc_set_date(19, 1, 10, 1);
    	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, 0x32F2);
  	}
}

/**************************************************************
函数名称 : HAL_RTC_MspInit
函数功能 : RTC底层驱动，时钟配置
输入参数 : hrtc：RTC句柄
返回值  	 : 无
备注		 : 无
**************************************************************/
void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    __HAL_RCC_PWR_CLK_ENABLE();	/* 使能电源时钟PWR */
    HAL_PWR_EnableBkUpAccess();	/* 取消备份区域写保护 */
	
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;	/* LSE配置 */
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;                	/* RTC使用LSE */
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC; /* 外设为RTC */
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE; /* RTC时钟源为LSE */
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

	__HAL_RCC_RTC_ENABLE();	/* RTC时钟使能 */
}

/**************************************************************
函数名称 : rtc_set_time
函数功能 : 设置RTC时间
输入参数 : hour：小时
		   min：分钟
		   sec：秒钟
		   ampm：RTC_HOURFORMAT12_AM/RTC_HOURFORMAT12_PM
返回值  	 : 无
备注		 : 无
**************************************************************/
void rtc_set_time(u8 hour, u8 min, u8 sec, u8 ampm)
{
	sTime.Hours = hour;
  	sTime.Minutes = min;
  	sTime.Seconds = sec;
	sTime.TimeFormat = ampm;
  	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  	sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	
  	if(HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  	{
    	printf("rtc set time failed\r\n");
  	}

}

/**************************************************************
函数名称 : rtc_set_date
函数功能 : 设置RTC日期
输入参数 : year：年
		   month：月
		   date：日
		   weekday：星期
返回值  	 : 无
备注		 : 无
**************************************************************/
void rtc_set_date(u8 year, u8 month, u8 date, u8 weekday)
{
	sDate.WeekDay = weekday;
  	sDate.Month = month;
  	sDate.Date = date;
  	sDate.Year = year;

  	if(HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
  	{
    	printf("rtc set date failed\r\n");
  	}

}

/**************************************************************
函数名称 : rtc_set_alarm_a
函数功能 : 设置闹钟时间(按星期闹铃,24小时制)
输入参数 : hour：年
		   min：月
		   sec：日
返回值  	 : 无
备注		 : 这里设置每天的固定某个时间闹钟
**************************************************************/
void rtc_set_alarm_a(u8 hour, u8 min, u8 sec)
{
	RTC_AlarmTypeDef sAlarm;

	/* Enable the Alarm A */
	sAlarm.AlarmTime.Hours = hour;
	sAlarm.AlarmTime.Minutes = min;
	sAlarm.AlarmTime.Seconds = sec;
	sAlarm.AlarmTime.SubSeconds = 0;
	sAlarm.AlarmTime.TimeFormat = RTC_HOURFORMAT12_AM;
	sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE; 
	sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET; 
	sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;				/* 屏蔽掉日期和时间 */
	sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
	sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY; 
	sAlarm.AlarmDateWeekDay = 1; 
	sAlarm.Alarm = RTC_ALARM_A;

	if(HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
	{
		printf("rtc set alarm_a failed\r\n");
	}
	
	HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
}

/**************************************************************
函数名称 : rtc_set_wakeup
函数功能 : 周期性唤醒定时器配置
输入参数 : wksel	
		   cnt：星期
返回值  	 : 无
备注		 : 无
**************************************************************/
void rtc_set_wakeup(u32 wksel, u16 cnt)
{
	 __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc, RTC_FLAG_WUTF);	/* 清除RTC WAKE UP的标志 */
	 
    /* Enable the WakeUp */
	if(HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, cnt, wksel) != HAL_OK)
	{
		printf("Enable the RTC WakeUp failed\r\n");
	}

    HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);

}

/**************************************************************
函数名称 : RTC_Alarm_IRQHandler
函数功能 : RTC闹钟中断服务函数
输入参数 : 无
返回值  	 : 无
备注		 : 无
**************************************************************/
void RTC_Alarm_IRQHandler(void)
{
    HAL_RTC_AlarmIRQHandler(&hrtc);
}

/**************************************************************
函数名称 : HAL_RTC_AlarmAEventCallback
函数功能 : RTC闹钟A中断处理回调函数
输入参数 : hrtc：RTC句柄
返回值  	 : 无
备注		 : 无
**************************************************************/
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
    printf("ALARM A!\r\n");
}

/**************************************************************
函数名称 : RTC_WKUP_IRQHandler
函数功能 : RTC WAKE UP中断服务函数
输入参数 : 无
返回值  	 : 无
备注		 : 无
**************************************************************/
void RTC_WKUP_IRQHandler(void)
{
    HAL_RTCEx_WakeUpTimerIRQHandler(&hrtc);
}

/**************************************************************
函数名称 : HAL_RTCEx_WakeUpTimerEventCallback
函数功能 : RTC WAKE UP中断回掉处理函数
输入参数 : hrtc：RTC句柄
返回值  	 : 无
备注		 : 无
**************************************************************/
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
    printf("RTC WAKE UP!\r\n");
}



























