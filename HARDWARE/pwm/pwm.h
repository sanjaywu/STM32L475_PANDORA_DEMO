#ifndef __PWM_H__
#define __PWM_H__

#include "sys.h"


void timer2_pwm_init(u32 prescaler, u32 period);
void set_tim2_channel_1_compare(u32 compare);
void set_tim1_channel_2_compare(u32 compare);


#endif





