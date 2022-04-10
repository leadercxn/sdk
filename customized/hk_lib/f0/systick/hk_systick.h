#ifndef _HK_SYSTICK_H_
#define _HK_SYSTICK_H_

#include "hk32f0xx.h"
#include "stdbool.h"

void hk_systick_init(void);
void hk_delay_us(uint32_t us);
void hk_delay_ms(uint32_t ms);

#endif
