#ifndef __RTC_H__
#define __RTC_H__

#include "sys.h"

extern RTC_HandleTypeDef hrtc;
extern RTC_TimeTypeDef sTime;
extern RTC_DateTypeDef sDate;

void rtc_init(void);
void rtc_set_time(u8 hour, u8 min, u8 sec, u8 ampm);
void rtc_set_date(u8 year, u8 month, u8 date, u8 weekday);
void rtc_set_alarm_a(u8 hour, u8 min, u8 sec);
void rtc_set_wakeup(u32 wksel, u16 cnt);













#endif



