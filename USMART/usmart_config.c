#include "usmart.h"
#include "usmart_str.h"

/*********************************************************************************
			  ___   _     _____  _____  _   _  _____  _____  _   __
			 / _ \ | |   |_   _||  ___|| \ | ||_   _||  ___|| | / /
			/ /_\ \| |     | |  | |__  |  \| |  | |  | |__  | |/ /
			|  _  || |     | |  |  __| | . ` |  | |  |  __| |    \
			| | | || |_____| |_ | |___ | |\  |  | |  | |___ | |\  \
			\_| |_/\_____/\___/ \____/ \_| \_/  \_/  \____/ \_| \_/

 *	******************************************************************************
 *	������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
 *	ALIENTEK Pandora STM32L475 IOT������
 *	USMART��������
 *	����ԭ��@ALIENTEK
 *	������̳:www.openedv.com
 *	�汾��V3.3
 *	��Ȩ���У�����ؾ���
 *	Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
 *	All rights reserved
 *	******************************************************************************/

////////////////////////////�û�������///////////////////////////////////////////////
//������Ҫ�������õ��ĺ�����������ͷ�ļ�(�û��Լ����)
#include "delay.h"
#include "sys.h"
#include "lcd.h"
#include "rtc.h"

//�������б��ʼ��(�û��Լ����)
//�û�ֱ������������Ҫִ�еĺ�����������Ҵ�
struct _m_usmart_nametab usmart_nametab[] =
{
#if USMART_USE_WRFUNS==1 	//���ʹ���˶�д����
    (void*)read_addr, "u32 read_addr(u32 addr)",
    (void*)write_addr, "void write_addr(u32 addr,u32 val)",
#endif
    (void*)delay_ms, "void delay_ms(u16 nms)",
    (void*)delay_us, "void delay_us(u32 nus)",

	(void*)rtc_set_time, "void rtc_set_time(u8 hour, u8 min, u8 sec, u8 ampm)",
	(void*)rtc_set_date, "void rtc_set_date(u8 year, u8 month, u8 date, u8 weekday)",
	(void*)rtc_set_alarm_a, "void rtc_set_alarm_a(u8 hour, u8 min, u8 sec);",
	(void*)rtc_set_wakeup, "void rtc_set_wakeup(u32 wksel, u16 cnt)",
};
///////////////////////////////////END///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//�������ƹ�������ʼ��
//�õ������ܿغ���������
//�õ�����������
struct _m_usmart_dev usmart_dev =
{
    usmart_nametab,
    usmart_init,
    usmart_cmd_rec,
    usmart_exe,
    usmart_scan,
    sizeof(usmart_nametab) / sizeof(struct _m_usmart_nametab), //��������
    0,	  	//��������
    0,	 	//����ID
    1,		//������ʾ����,0,10����;1,16����
    0,		//��������.bitx:,0,����;1,�ַ���
    0,	  	//ÿ�������ĳ����ݴ��,��ҪMAX_PARM��0��ʼ��
    0,		//�����Ĳ���,��ҪPARM_LEN��0��ʼ��
};



















