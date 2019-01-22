#include "w25qxx.h"

//W25X系列/Q系列芯片列表	   
//W25Q80  ID  0XEF13
//W25Q16  ID  0XEF14
//W25Q32  ID  0XEF15
//W25Q64  ID  0XEF16	
//W25Q128 ID  0XEF17	
//W25Q256 ID  0XEF18

#define WRITE_ENABLE				0x06
#define WRITE_DISABLE				0x04
#define READ_STATUS_REGISTER1		0x05
#define WRITE_STATUS_REGISTER1		0x01
#define READ_STATUS_REGISTER2		0x35
#define WRITE_STATUS_REGISTER2		0x31
#define READ_STATUS_REGISTER3		0x15
#define WRITE_STATUS_REGISTER3		0x11
#define CHIP_ERASE					0xC7
#define POWER_DOWN					0xB9
#define RELEASE_POWER_DOWN			0xAB
#define DEVICE_ID					0x90
#define JEDEC_ID					0x9F
#define ENTER_QPI_MODE				0x38
#define EXIT_QPI_MODE				0xFF
#define PAGE_PROGRAM				0x02
#define SECTOR_ERASE				0x20
#define BLOCK_ERASE					0xD8
#define READ_DATA					0x03
#define FAST_READ_DATA				0x0B
#define FAST_READ_DUAL_OUTPUT		0x3B
#define SET_READ_PARAMETERS			0xC0

u8 	g_w25qxx_buffer[4096];
u16 g_w25qxx_device_id;

QSPI_HandleTypeDef hqspi;

/**************************************************************
函数名称 : qspi_init
函数功能 : QUADSPI初始化
输入参数 : 无
返回值  	 : 无
备注		 : 无
**************************************************************/
void qspi_init(void)
{
	/* QUADSPI parameter configuration*/
  	hqspi.Instance = QUADSPI;
  	hqspi.Init.ClockPrescaler = 0;	/* QPSI分频比，W25Q128最大频率为104MHz，这里为80/(0+1)=80MHz */
  	hqspi.Init.FifoThreshold = 4;	/* FIFO深度设置为4个字节 */
  	hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;/* 配置QSPI 在Flash 驱动信号后过半个CLK 周期才对Flash 驱动的数据采样 */
  	hqspi.Init.FlashSize = 23;	/* 23+1=24，24位寻址，这里24位寻址是寻字节地址（W25Q128为16M字节） */
  	hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_4_CYCLE;/* 片选高电平时间为4个时钟(12.5*4=50ns),即手册里面的tSHSL参数 */
  	hqspi.Init.ClockMode = QSPI_CLOCK_MODE_0;
  	if(HAL_QSPI_Init(&hqspi) != HAL_OK)
  	{
    	printf("qspi init failed\r\n");
  	}
}

/**************************************************************
函数名称 : HAL_QSPI_MspInit
函数功能 : QSPI底层驱动,引脚配置，时钟使能
输入参数 : hqspi：QSPI句柄
返回值  	 : 无
备注		 : 无
**************************************************************/
void HAL_QSPI_MspInit(QSPI_HandleTypeDef* hqspi)
{
  	GPIO_InitTypeDef GPIO_InitStruct;
  	if(hqspi->Instance==QUADSPI)
  	{
	    /* Peripheral clock enable */
	    __HAL_RCC_QSPI_CLK_ENABLE();
	  	__HAL_RCC_GPIOE_CLK_ENABLE();
		
	    /**QUADSPI GPIO Configuration    
	    PE10     ------> QUADSPI_CLK
	    PE11     ------> QUADSPI_NCS
	    PE12     ------> QUADSPI_BK1_IO0
	    PE13     ------> QUADSPI_BK1_IO1
	    PE14     ------> QUADSPI_BK1_IO2
	    PE15     ------> QUADSPI_BK1_IO3 
	    */
	    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13 
	                          |GPIO_PIN_14|GPIO_PIN_15;
	    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
	    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
  	}
}

/**************************************************************
函数名称 : qspi_send_cmd
函数功能 : QSPI发送命令
输入参数 : 
instruction：	 要发送的指令
address：		 发送到的目的地址
dummyCycles：	 空指令周期数
instructionMode：指令模式;QSPI_INSTRUCTION_NONE,QSPI_INSTRUCTION_1_LINE,QSPI_INSTRUCTION_2_LINE,QSPI_INSTRUCTION_4_LINE
addressMode：	 地址模式; QSPI_ADDRESS_NONE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_2_LINE,QSPI_ADDRESS_4_LINE
addressSize：	 地址长度;QSPI_ADDRESS_8_BITS,QSPI_ADDRESS_16_BITS,QSPI_ADDRESS_24_BITS,QSPI_ADDRESS_32_BITS
dataMode：	 	 数据模式; QSPI_DATA_NONE,QSPI_DATA_1_LINE,QSPI_DATA_2_LINE,QSPI_DATA_4_LINE
返回值  	 : 无
备注		 : 无
**************************************************************/
void qspi_send_cmd(u32 instruction, u32 address, u32 dummyCycles, u32 instructionMode, u32 addressMode, u32 addressSize, u32 dataMode)
{
	QSPI_CommandTypeDef Cmdhandler;

    Cmdhandler.Instruction = instruction;                 		/* 指令 */
    Cmdhandler.Address = address;                            	/* 地址 */
    Cmdhandler.DummyCycles = dummyCycles;                   	/* 设置空指令周期数 */
    Cmdhandler.InstructionMode = instructionMode;				/* 指令模式 */
    Cmdhandler.AddressMode = addressMode;   					/* 地址模式 */
    Cmdhandler.AddressSize = addressSize;   					/* 地址长度 */
    Cmdhandler.DataMode = dataMode;             				/* 数据模式 */
    Cmdhandler.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;       		/* 每次都发送指令 */
    Cmdhandler.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE; 	/* 无交替字节 */
    Cmdhandler.DdrMode = QSPI_DDR_MODE_DISABLE;           		/* 关闭DDR模式 */
    Cmdhandler.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    HAL_QSPI_Command(&hqspi, &Cmdhandler, 5000);
}

/**************************************************************
函数名称 : qspi_receive
函数功能 : QSPI接收指定长度的数据
输入参数 : buf：接收数据缓冲区首地址
		   datalen：要传输的数据长度
返回值  	 : 0:正常;其他:错误代码
备注		 : 无
**************************************************************/
u8 qspi_receive(u8* buf, u32 datalen)
{
    hqspi.Instance->DLR = datalen - 1; /* 配置数据长度 */
    if(HAL_QSPI_Receive(&hqspi, buf, 5000) == HAL_OK) /* 接收数据 */
	{	
		return 0;
    }
    else 
	{
		return 1;
    }
}

/**************************************************************
函数名称 : qspi_transmit
函数功能 : QSPI发送指定长度的数据
输入参数 : buf：发送数据缓冲区首地址
		   datalen：要传输的数据长度
返回值  	 : 0:正常;其他:错误代码
备注		 : 无
**************************************************************/
u8 qspi_transmit(u8* buf, u32 datalen)
{
    hqspi.Instance->DLR = datalen - 1; /* 配置数据长度 */
    if(HAL_QSPI_Transmit(&hqspi, buf, 5000) == HAL_OK) /* 发送数据 */
	{
		return 0; 
    }
    else
    {	
    	return 1;
    }
}

/**************************************************************
函数名称 : w25qxx_read_status_reg
函数功能 : 读取w25qxx的状态寄存器
输入参数 : reg：状态寄存器号，范:1~3
返回值  	 : 状态寄存器值
备注		 : W25QXX一共有3个状态寄存器
* 			状态寄存器1：
* 			BIT7  6   5   4   3   2   1   0
* 			SPR   RV  TB BP2 BP1 BP0 WEL BUSY
* 			SPR:默认0,状态寄存器保护位,配合WP使用
* 			TB,BP2,BP1,BP0:FLASH区域写保护设置
* 			WEL:写使能锁定
* 			BUSY:忙标记位(1,忙;0,空闲)
* 			默认:0x00
* 			状态寄存器2：
* 			BIT7  6   5   4   3   2   1   0
* 			SUS   CMP LB3 LB2 LB1 (R) QE  SRP1
* 			状态寄存器3：
* 			BIT7      6    5    4   3   2   1   0
* 			HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
**************************************************************/
u8 w25qxx_read_status_reg(u8 reg)
{
    u8 byte = 0, cmd = 0;

    switch(reg)
    {
        case 1:
            cmd = READ_STATUS_REGISTER1;  /* 读状态寄存器1指令 */
            break;

        case 2:
            cmd = READ_STATUS_REGISTER1;  /* 读状态寄存器2指令 */
            break;

        case 3:
            cmd = READ_STATUS_REGISTER1;  /* 读状态寄存器3指令 */
            break;

        default:
            cmd = READ_STATUS_REGISTER1;
            break;
    }

    qspi_send_cmd(cmd, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_1_LINE);
    qspi_receive(&byte, 1);
	
    return byte;
}

/**************************************************************
函数名称 : w25qxx_write_status_reg
函数功能 : 写w25qxx的状态寄存器
输入参数 : reg：状态寄存器号，范:1~3;sr：写入的数据首地址
返回值  	 : 无
备注		 : 无
**************************************************************/
void w25qxx_write_status_reg(u8 reg, u8 sr)
{
    u8 cmd = 0;

    switch(reg)
    {
        case 1:
            cmd = WRITE_STATUS_REGISTER1;  /* 写状态寄存器1指令 */
            break;

        case 2:
            cmd = WRITE_STATUS_REGISTER2;  /* 写状态寄存器2指令 */
            break;

        case 3:
            cmd = WRITE_STATUS_REGISTER3;  /* 写状态寄存器3指令 */
            break;

        default:
            cmd = WRITE_STATUS_REGISTER1;
            break;
    }

    qspi_send_cmd(cmd, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_1_LINE);
    qspi_transmit(&sr, 1);
}

/**************************************************************
函数名称 : w25qxx_wait_busy
函数功能 : 等待空闲
输入参数 : 无
返回值  	 : 无
备注		 : 无
**************************************************************/
void w25qxx_wait_busy(void)
{
    while((w25qxx_read_status_reg(1) & 0x01) == 0x01); /* 等待BUSY位清空 */ 
}

/**************************************************************
函数名称 : w25qxx_write_enable
函数功能 : w25qxx写使能,将S1寄存器的WEL置位
输入参数 : 无
返回值  	 : 无
备注		 : 无
**************************************************************/
void w25qxx_write_enable(void)
{
    qspi_send_cmd(WRITE_ENABLE, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);
}

/**************************************************************
函数名称 : w25qxx_write_disable
函数功能 : w25qxx写禁止,将WEL清零
输入参数 : 无
返回值  	 : 无
备注		 : 无
**************************************************************/
void w25qxx_write_disable(void)
{
    qspi_send_cmd(WRITE_DISABLE, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);
}

/**************************************************************
函数名称 : w25qxx_read_device_id
函数功能 : 读取flash芯片ID
输入参数 : 无
返回值  	 : 返回flash芯片ID
备注		 : 无
**************************************************************/
u16 w25qxx_read_device_id(void)
{
    u8 temp[2];
    u16 device_id;
	
    qspi_send_cmd(DEVICE_ID, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, QSPI_DATA_1_LINE);
    qspi_receive(temp, 2);
    device_id = (temp[0] << 8) | temp[1];
	
    return device_id;
}

/**************************************************************
函数名称 : w25qxx_read
函数功能 : 读取flash数据
输入参数 : buf：数据存储区
		   addr：开始读取的地址(最大32bit)
		   size：要读取的字节数(最大65535)
返回值  	 : 无
备注		 : 无
**************************************************************/
void w25qxx_read(u8* buf, u32 addr, u16 size)
{
    qspi_send_cmd(FAST_READ_DATA, addr, 8, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, QSPI_DATA_1_LINE);
    qspi_receive(buf, size);
}

/**************************************************************
函数名称 : w25qxx_write_page
函数功能 : 在指定地址开始写入最大256字节的数据
输入参数 : buf：数据存储区
		   addr：开始写的地址(最大32bit)
		   size：要写入的字节数(最大256),
		   该数不应该超过该页的剩余字节数
返回值  	 : 无
备注		 : 无
**************************************************************/
void w25qxx_write_page(u8* buf, u32 addr, u16 size)
{
    w25qxx_write_enable();	/* 写使能 */
    qspi_send_cmd(PAGE_PROGRAM, addr, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, QSPI_DATA_1_LINE);
    qspi_transmit(buf, size);
    w25qxx_wait_busy();		/* 等待写入结束 */
}

/**************************************************************
函数名称 : w25qxx_write_no_check
函数功能 : 无检验写falsh
输入参数 : buf：数据存储区
		   addr：开始写入的地址(最大32bit)
		   size：要写入的字节数(最大65535)
返回值  	 : 无
备注		 : 必须确保所写的地址范围内的数据全部为0XFF,
		   否则在非0XFF处写入的数据将失败!具有自动换页功能
		   在指定地址开始写入指定长度的数据,但是要确保地址不越界!
**************************************************************/
void w25qxx_write_no_check(u8* buf, u32 addr, u16 szie)
{
    u16 pageremain;
    pageremain = 256 - addr % 256; /* 单页剩余的字节数 */

    if(szie <= pageremain)
	{	
		pageremain = szie; 			/* 不大于256个字节 */
    }
    while(1)
    {
        w25qxx_write_page(buf, addr, pageremain);

        if(szie == pageremain)
		{
			break; 	/* 写入结束了 */
        }
        else 		/* szie > pageremain */
        {
            buf += pageremain;
            addr += pageremain;
            szie -= pageremain;	 /* 减去已经写入了的字节数 */

            if(szie > 256)/* 一次可以写入256个字节 */
			{
				pageremain = 256; 
            }
            else 
			{
				pageremain = szie;/* 不够256个字节了 */
            }
        }
    }
}

/**************************************************************
函数名称 : w25qxx_erase_sector
函数功能 : 擦除一个扇区，擦除一个扇区的最少时间:150ms
输入参数 : addr：扇区地址 根据实际容量设置
返回值  	 : 无
备注		 : 无
**************************************************************/
void w25qxx_erase_sector(u32 addr)
{
    //printf("fe:%x\r\n", addr);/* 监视flash擦除情况,测试用 */
    addr *= 4096;
    w25qxx_write_enable();		/* 使能写 */
    w25qxx_wait_busy();	
    qspi_send_cmd(SECTOR_ERASE, addr, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_1_LINE, QSPI_ADDRESS_24_BITS, QSPI_DATA_NONE);
    w25qxx_wait_busy();  		/* 等待擦除完成 */
}

/**************************************************************
函数名称 : w25qxx_write
函数功能 : 写falsh
输入参数 : buf：数据存储区
		   addr：开始写入的地址(最大32bit)
		   size：要写入的字节数(最大65535)
返回值  	 : 无
备注		 : 在指定地址开始写入指定长度的数据该函数带擦除操作!
**************************************************************/
void w25qxx_write(u8* buf, u32 addr, u16 size)
{
    u32 secpos;
    u16 secoff;
    u16 secremain;
    u16 i;
    u8 * w25qxx_buf;
    w25qxx_buf = g_w25qxx_buffer;
    secpos = addr / 4096; 		/* 扇区地址 */
    secoff = addr % 4096; 		/* 在扇区内的偏移 */
    secremain = 4096 - secoff; 	/* 扇区剩余空间大小 */

    //printf("ad:%X,nb:%X\r\n",addr,size);
    if(size <= secremain)
	{
		secremain = size; /* 不大于4096个字节 */
    }
    while(1)
    {
        w25qxx_read(w25qxx_buf, secpos * 4096, 4096); /* 读出整个扇区的内容 */

        for(i = 0; i < secremain; i++) /* 校验数据 */
        {
            if(w25qxx_buf[secoff + i] != 0XFF)/* 需要擦除 */
			{
				break;
            }
        }
		
        if(i < secremain) //需要擦除
        {
            w25qxx_erase_sector(secpos);	/* 擦除这个扇区 */

            for(i = 0; i < secremain; i++)	 /* 复制 */
            {
                w25qxx_buf[i + secoff] = buf[i];
            }

            w25qxx_write_no_check(w25qxx_buf, secpos * 4096, 4096); /* 写入整个扇区 */
        }
        else
		{
			w25qxx_write_no_check(buf, addr, secremain); /* 写已经擦除了的,直接写入扇区剩余区间 */
        }

        if(size == secremain)/* 写入结束了 */
		{
			break; 
        }
        else/* 写入未结束 */
        {
            secpos++;	/* 扇区地址增1 */
            secoff = 0; /* 偏移位置为0 */
            buf += secremain; 	/* 指针偏移 */
            addr += secremain; 	/* 写地址偏移 */
            size -= secremain;	/* 字节数递减 */

            if(size > 4096)/* 下一个扇区还是写不完 */
			{
				secremain = 4096;
            }
            else /* 下一个扇区可以写完了 */
			{
				secremain = size;			
            }
        }
    };
}

/**************************************************************
函数名称 : w25qxx_erase_chip
函数功能 : 擦除整个芯片
输入参数 : 无
返回值  	 : 无
备注		 : 无
**************************************************************/
void w25qxx_erase_chip(void)
{
    w25qxx_write_enable();	/* 使能写 */
    w25qxx_wait_busy();
    qspi_send_cmd(CHIP_ERASE, 0, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE, QSPI_ADDRESS_8_BITS, QSPI_DATA_NONE);
    w25qxx_wait_busy();		/* 等待芯片擦除结束 */
}

/**************************************************************
函数名称 : w25qxx_init
函数功能 : w25qxx初始化
输入参数 : 无
返回值  	 : 无
备注		 : 无
**************************************************************/
void w25qxx_init(void)
{
	qspi_init();
	g_w25qxx_device_id = w25qxx_read_device_id();
}








