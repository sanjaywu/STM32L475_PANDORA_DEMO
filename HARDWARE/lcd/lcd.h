#ifndef __LCD_H__
#define __LCD_H__

#include "sys.h"

/* 颜色 */
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40
#define BRRED 			 0XFC07
#define GRAY  			 0X8430

extern u16 POINT_COLOR;		/* 画笔颜色 */
extern u16 BACK_COLOR;  	/* 背景色 */ 

void lcd_init(void);
void lcd_clear(u16 color);
void lcd_draw_point(u16 x, u16 y, u16 color);
void lcd_show_char(u16 x, u16 y, u8 num, u8 size, u8 mode);
void lcd_show_string(u16 x, u16 y, u16 width, u16 height, u8 *p, u8 size, u8 mode);
void lcd_show_chinese(u16 x, u16 y, const u8 * ch, u8 size, u8 n, u8 mode);
















#endif




