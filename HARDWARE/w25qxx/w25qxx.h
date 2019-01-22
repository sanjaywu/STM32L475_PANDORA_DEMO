#ifndef __W25QXX_H__
#define __W25QXX_H__

#include "sys.h"

extern u16 g_w25qxx_device_id;

void w25qxx_init(void);
u16 w25qxx_read_device_id(void);
void w25qxx_read(u8* buf, u32 addr, u16 size);
void w25qxx_write(u8* buf, u32 addr, u16 size);
void w25qxx_erase_chip(void);



#endif






