#ifndef __AHT10_H__
#define __AHT10_H__

#include "sys.h"

u8 aht10_init(void);
void aht10_read_data(float *temperature, float *humidity);

float aht10_read_humidity(void);
float aht10_read_temperature(void);




#endif


