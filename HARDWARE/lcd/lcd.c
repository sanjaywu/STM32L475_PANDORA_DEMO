#include "delay.h"
#include "lcd.h"
#include "font.h"

/*
如果使用软件SPI，只需把硬件SPI的宏开关去掉即可
*/
#define __LCD_HW_SPI_DRIVE__
#define __LCD_SW_SPI_DRIVE__

/*
LCD_PWR：PB7
LCD_RST：PB6
LCD_WR： PB4
LCD_CS： PD7
*/
#define	LCD_PWR(n)	(n ? HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET))
#define	LCD_RST(n)	(n ? HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET))
#define	LCD_WR(n)	(n ? HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET))
#define	LCD_CS(n)	(n ? HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET))

#define LCD_WIDTH 	240
#define LCD_HEIHGT 	240

/* 默认LCD的画笔颜色和背景色 */
u16 POINT_COLOR = 0x0000;	/* 画笔颜色 */
u16 BACK_COLOR = 0xFFFF;  	/* 背景色 */ 

#if defined(__LCD_HW_SPI_DRIVE__)	/* 硬件SPI驱动 */

SPI_HandleTypeDef hspi3;

/**************************************************************
函数名称 : spi3_init
函数功能 : 硬件sp3初始化
输入参数 : 无
返回值  	 : 无
备注		 : 配置成主机模式
**************************************************************/
void spi3_init(void)
{
	/* SPI3 parameter configuration*/
  	hspi3.Instance = SPI3;
  	hspi3.Init.Mode = SPI_MODE_MASTER;
  	hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  	hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  	hspi3.Init.CLKPolarity = SPI_POLARITY_HIGH;
  	hspi3.Init.CLKPhase = SPI_PHASE_2EDGE;
  	hspi3.Init.NSS = SPI_NSS_SOFT;
  	hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  	hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  	hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  	hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  	hspi3.Init.CRCPolynomial = 7;
	
  	if(HAL_SPI_Init(&hspi3) != HAL_OK)
  	{
    	printf("spi3 init failed\r\n");
  	}

	 __HAL_SPI_ENABLE(&hspi3);	/* 使能硬件SPI3 */
}

/**************************************************************
函数名称 : HAL_SPI_MspInit
函数功能 : 硬件SPI3底层驱动，时钟使能，引脚配置
输入参数 : hspi：硬件SPI3句柄
返回值  	 : 无
备注		 : PB3 ------> SPI3_SCK
	       PB5 ------> SPI3_MOSI 
**************************************************************/
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	if(hspi->Instance==SPI3)
	{
	    __HAL_RCC_SPI3_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
	  
	    GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_5;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull = GPIO_PULLUP;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
	    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }
}

/**************************************************************
函数名称 : sp3_write_byte
函数功能 : SPI3 写一个字节
输入参数 : w_data：要写入的字节，size：写入字节大小
返回值  	 : 0 --> 写入成功，其他 --> 写入失败
备注		 : 无
**************************************************************/
u8 sp3_write_byte(u8 *r_data, u16 size)
{
	return HAL_SPI_Transmit(&hspi3, r_data, size, 1000);
}

/**************************************************************
函数名称 : lcd_gpio_init
函数功能 : lcd gpio初始化
输入参数 : 无
返回值  	 : 无
备注		 : 无
**************************************************************/
void lcd_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

  	__HAL_RCC_GPIOD_CLK_ENABLE();
  	__HAL_RCC_GPIOB_CLK_ENABLE();

  	/*Configure GPIO pin : PD7 */
  	GPIO_InitStruct.Pin = GPIO_PIN_7;
  	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  	GPIO_InitStruct.Pull = GPIO_PULLUP;
  	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  	/*Configure GPIO pins : PB4 PB6 PB7 */
  	GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7;
  	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  	GPIO_InitStruct.Pull = GPIO_PULLUP;
  	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pin Output Level */
  	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);

  	/*Configure GPIO pin Output Level */
  	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_RESET);

	spi3_init();	/* spi3初始化 */		
}

/**************************************************************
函数名称 : lcd_write_reg
函数功能 : 向lcd驱动芯片写一个命令
输入参数 : reg：要写入的命令
返回值  	 : 无
备注		 : LCD_WR为0时表示写命令
**************************************************************/
void lcd_write_reg(u8 reg)
{
	LCD_CS(0);
	LCD_WR(0);
	sp3_write_byte(&reg, 1);
}

/**************************************************************
函数名称 : lcd_write_data
函数功能 : 向lcd驱动芯片写一个数据
输入参数 : data：要写入的数据
返回值  	 : 无
备注		 : LCD_WR为1时表示写数据
**************************************************************/
void lcd_write_data(u8 data)
{
	LCD_CS(0);
	LCD_WR(1);
	sp3_write_byte(&data, 1);
}

#elif defined(__LCD_SW_SPI_DRIVE__)		/* 软件SPI驱动 */

/*
LCD_SDA：PB5
LCD_SCK：PB3
*/
#define	LCD_SDA(n)	(n ? HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET))
#define	LCD_SCK(n)	(n ? HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET) : HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET))

/**************************************************************
函数名称 : lcd_gpio_init
函数功能 : lcd gpio初始化
输入参数 : 无
返回值  	 : 无
备注		 : 无
**************************************************************/
void lcd_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

  	__HAL_RCC_GPIOD_CLK_ENABLE();
  	__HAL_RCC_GPIOB_CLK_ENABLE();

  	/*Configure GPIO pin : PD7 */
  	GPIO_InitStruct.Pin = GPIO_PIN_7;
  	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  	GPIO_InitStruct.Pull = GPIO_PULLUP;
  	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  	/*Configure GPIO pins : PB3 PB4 PB5 PB6 PB7 */
  	GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
  	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  	GPIO_InitStruct.Pull = GPIO_PULLUP;
  	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pin Output Level */
  	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);

  	/*Configure GPIO pin Output Level */
  	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_SET);
}

/**************************************************************
函数名称 : lcd_write_reg
函数功能 : 向lcd驱动芯片写一个命令
输入参数 : reg：要写入的命令
返回值  	 : 无
备注		 : LCD_WR为0时表示写命令
**************************************************************/
void lcd_write_reg(u8 reg)
{
	u8 i;

	LCD_CS(0);
	LCD_WR(0);
	
	for(i = 0; i < 8; i++)
	{			  
		LCD_SCK(0);
		if(reg & 0x80)
			LCD_SDA(1);
		else 
			LCD_SDA(0);
		LCD_SCK(1);
		reg <<= 1;   
	}
	LCD_CS(1);
	LCD_WR(1);
}

/**************************************************************
函数名称 : lcd_write_reg
函数功能 : 向lcd驱动芯片写一个数据
输入参数 : data：要写入的数据
返回值  	 : 无
备注		 : LCD_WR为1时表示写数据
**************************************************************/
void lcd_write_data(u8 data)
{
	u8 i;

	LCD_CS(0);
	LCD_WR(1);
	
	for(i = 0; i < 8; i++)
	{			  
		LCD_SCK(0);
		if(data & 0x80)
			LCD_SDA(1);
		else 
			LCD_SDA(0);
		LCD_SCK(1);
		data <<= 1;   
	}
	LCD_CS(1);
	LCD_WR(1);
}

#endif

/**************************************************************
函数名称 : lcd_write_ram
函数功能 : 使能写入数据到RAM
输入参数 : 无
返回值  	 : 无
备注		 : 无
**************************************************************/
void lcd_write_ram(void)
{
	lcd_write_reg(0x2C);
}

/**************************************************************
函数名称 : lcd_set_address
函数功能 : 设置坐标
输入参数 : x1,y1：起始地址，x2,y2：终点地址
返回值  	 : 无
备注		 : 无
**************************************************************/
void lcd_set_address(u16 x1, u16 y1, u16 x2, u16 y2)
{
    lcd_write_reg(0x2a);
    lcd_write_data(x1 >> 8);
    lcd_write_data(x1);
    lcd_write_data(x2 >> 8);
    lcd_write_data(x2);

    lcd_write_reg(0x2b);
    lcd_write_data(y1 >> 8);
    lcd_write_data(y1);
    lcd_write_data(y2 >> 8);
    lcd_write_data(y2);
}

/**************************************************************
函数名称 : lcd_display_on
函数功能 : 打开显示
输入参数 : 无
返回值  	 : 无
备注		 : 无
**************************************************************/
void lcd_display_on(void)
{
    //LCD_PWR(1);
	lcd_write_reg(0x29);
}

/**************************************************************
函数名称 : lcd_display_off
函数功能 : 关闭显示
输入参数 : 无
返回值  	 : 无
备注		 : 无
**************************************************************/
void lcd_display_off(void)
{
   // LCD_PWR(0);
	lcd_write_reg(0x28);
}

/**************************************************************
函数名称 : lcd_clear
函数功能 : lcd清屏函数
输入参数 : color：要清屏的颜色
返回值  	 : 无
备注		 : 先关闭显示，等到所有颜色数据写入到RAM后再打开显示
**************************************************************/
void lcd_clear(u16 color)
{	
    u16 i, j;

	lcd_display_off();		/* 关闭显示 */
	lcd_set_address(0, 0, LCD_WIDTH - 1, LCD_HEIHGT - 1);
	lcd_write_ram();
	
	for(i = 0; i < LCD_HEIHGT; i++)
	{
		for(j = 0; j < LCD_WIDTH; j++)
		{
			lcd_write_data(color >> 8);
			lcd_write_data(color & 0x00ff);
		}
	}
	lcd_display_on();		/* 打开显示 */
}

/**************************************************************
函数名称 : lcd_draw_point
函数功能 : lcd画一个点
输入参数 : x,y	--> 画点坐标，color --> 点的颜色
返回值  	 : 无
备注		 : 无
**************************************************************/
void lcd_draw_point(u16 x, u16 y, u16 color)
{
	lcd_set_address(x, y, x, y);
	lcd_write_ram();
	lcd_write_data(color >> 8);
    lcd_write_data(color & 0x00ff); 
}

/**************************************************************
函数名称 : lcd_show_char
函数功能 : lcd显示一个字符
输入参数 : x,y:起始坐标
		   num:要显示的字符:" "--->"~"
		   size:字体大小
		   mode:叠加方式(1)还是非叠加方式(0)
返回值  	 : 无
备注		 : 无
**************************************************************/
void lcd_show_char(u16 x, u16 y, u8 num, u8 size, u8 mode)
{  							  
    u16 temp, t1, t;
	u16 y0 = y;
	u16 csize = ((size / 8) + ((size % 8) ? 1 : 0)) * (size/2);	/* 得到字体一个字符对应点阵集所占的字节数	 */
	
 	num = num - ' ';	/* 得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库） */
	
	for(t = 0; t < csize; t++)	/*遍历打印所有像素点到LCD */
	{   
     	if(16 == size)
		{
			temp = asc2_1608[num][t];	/* 调用1608字体 */
     	}
		else if(24 == size)
		{
			temp = asc2_2412[num][t];	/* 调用2412字体 */
		}
		else if(32 == size)
		{
			temp = asc2_3216[num][t];	/* 调用3216数码管字体 */
		}
		else
		{	
			return;		/* 没有找到对应的字库 */
		}
		for(t1 = 0; t1 < 8; t1++)	/* 打印一个像素点到液晶 */
		{			    
			if(temp & 0x80)
			{
				lcd_draw_point(x, y, POINT_COLOR);
			}
			else if(0 == mode)
			{
				lcd_draw_point(x, y, BACK_COLOR);
			}
			temp <<= 1;
			y++;
			
			if(y >= LCD_HEIHGT)
			{
				return;		/* 超区域了 */
			}
			if((y - y0) == size)
			{
				y = y0;
				x++;
				if(x >= LCD_WIDTH)
				{
					return;	/* 超区域了 */
				}
				break;
			}
		}  	 
	}  	    	   	 	  
} 

/**************************************************************
函数名称 : lcd_show_string
函数功能 : lcd显示字符串
输入参数 : x,y:起始坐标
		   width,height：区域大小
		   *p:字符串起始地址
		   size:字体大小
		   mode:叠加方式(1)还是非叠加方式(0)
返回值  	 : 无
备注		 : 无
**************************************************************/
void lcd_show_string(u16 x, u16 y, u16 width, u16 height, u8 *p, u8 size, u8 mode)
{         
	u16 x0 = x;
	width += x;
	height += y;
	
    while((*p<='~') && (*p>=' '))		/* 判断是不是非法字符! */
    {       
        if(x >= width)
		{
			x = x0; 
			y += size;
		}
		
        if(y >= height)
		{	
			break;
        }
        lcd_show_char(x, y, *p, size, mode);
        x += size/2;
        p++;
    }  
}

/**************************************************************
函数名称 : lcd_show_chinese
函数功能 : lcd显示字符串
输入参数 : x,y:起始坐标
		   *ch:汉字字符串起始地址
		   size:字体大小
		   n:汉字个数
		   mode:叠加方式(1)还是非叠加方式(0)
返回值  	 : 无
备注		 : 无
**************************************************************/
void lcd_show_chinese(u16 x, u16 y, const u8 * ch, u8 size, u8 n, u8 mode)
{
	u32 temp, t, t1;
    u16 y0 = y;
    u32 csize = ((size / 8) + ((size % 8) ? 1 : 0)) * (size) * n;	/* 得到字体字符对应点阵集所占的字节数 */
    
    for(t = 0; t < csize; t++)
	{   												   
		temp = ch[t];	/* 得到点阵数据 */
		
		for(t1 = 0; t1 < 8; t1++)
		{
			if(temp & 0x80)
			{
				lcd_draw_point(x, y, POINT_COLOR);
			}
			else if(mode==0)
			{
				lcd_draw_point(x, y, BACK_COLOR);
			}
			temp <<= 1;
			y++;
			if((y - y0) == size)
			{
				y = y0;
				x++;
				break;
			}
		}  	 
	}  
}

/**************************************************************
函数名称 : lcd_init
函数功能 : lcd初始化
输入参数 : 无
返回值  	 : 无
备注		 : 无
**************************************************************/
void lcd_init(void)
{
	lcd_gpio_init();

	LCD_RST(0);
    delay_ms(120);
    LCD_RST(1);
	delay_ms(120);

	/* Sleep Out */
    lcd_write_reg(0x11);
    /* wait for power stability */
    delay_ms(120);

    /* Memory Data Access Control */
    lcd_write_reg(0x36);
    lcd_write_data(0x00);

    /* RGB 5-6-5-bit  */
    lcd_write_reg(0x3A);
    lcd_write_data(0x65);

    /* Porch Setting */
    lcd_write_reg(0xB2);
    lcd_write_data(0x0C);
    lcd_write_data(0x0C);
    lcd_write_data(0x00);
    lcd_write_data(0x33);
    lcd_write_data(0x33);

    /*  Gate Control */
    lcd_write_reg(0xB7);
    lcd_write_data(0x72);

    /* VCOM Setting */
    lcd_write_reg(0xBB);
    lcd_write_data(0x3D);   //Vcom=1.625V

    /* LCM Control */
    lcd_write_reg(0xC0);
    lcd_write_data(0x2C);

    /* VDV and VRH Command Enable */
    lcd_write_reg(0xC2);
    lcd_write_data(0x01);

    /* VRH Set */
    lcd_write_reg(0xC3);
    lcd_write_data(0x19);

    /* VDV Set */
    lcd_write_reg(0xC4);
    lcd_write_data(0x20);

    /* Frame Rate Control in Normal Mode */
    lcd_write_reg(0xC6);
    lcd_write_data(0x0F);	//60MHZ

    /* Power Control 1 */
    lcd_write_reg(0xD0);
    lcd_write_data(0xA4);
    lcd_write_data(0xA1);

    /* Positive Voltage Gamma Control */
    lcd_write_reg(0xE0);
    lcd_write_data(0xD0);
    lcd_write_data(0x04);
    lcd_write_data(0x0D);
    lcd_write_data(0x11);
    lcd_write_data(0x13);
    lcd_write_data(0x2B);
    lcd_write_data(0x3F);
    lcd_write_data(0x54);
    lcd_write_data(0x4C);
    lcd_write_data(0x18);
    lcd_write_data(0x0D);
    lcd_write_data(0x0B);
    lcd_write_data(0x1F);
    lcd_write_data(0x23);

    /* Negative Voltage Gamma Control */
    lcd_write_reg(0xE1);
    lcd_write_data(0xD0);
    lcd_write_data(0x04);
    lcd_write_data(0x0C);
    lcd_write_data(0x11);
    lcd_write_data(0x13);
    lcd_write_data(0x2C);
    lcd_write_data(0x3F);
    lcd_write_data(0x44);
    lcd_write_data(0x51);
    lcd_write_data(0x2F);
    lcd_write_data(0x1F);
    lcd_write_data(0x1F);
    lcd_write_data(0x20);
    lcd_write_data(0x23);

    /* Display Inversion On */
    lcd_write_reg(0x21);
    lcd_write_reg(0x29);
	
	LCD_PWR(1);	
}













