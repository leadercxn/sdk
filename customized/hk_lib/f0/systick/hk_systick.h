#ifndef _HK_SYSTICK_H_
#define _HK_SYSTICK_H_

#include "hk32f0xx.h"
#include "systick.h"

void hk_systick_init(void);
void hk_delay_us(uint32_t us);
void hk_delay_ms(uint32_t ms);

void hk_systick_init_rebuild(systick_cfg_t *p_cfg);
void hk_delay_us_rebuild(systick_cfg_t *p_cfg, uint32_t us);
void hk_delay_ms_rebuild(systick_cfg_t *p_cfg, uint32_t ms);
uint32_t hk_systick_get(void);

#endif
