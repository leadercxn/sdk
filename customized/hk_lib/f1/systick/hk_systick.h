#ifndef _HK_SYSTICK_H_
#define _HK_SYSTICK_H_

#include "stm32f10x.h"
#include "stdbool.h"
#include "systick.h"

void hk_systick_init(systick_cfg_t *p_sys_config);
void hk_delay_us(systick_cfg_t *p_sys_config, uint32_t us);
void hk_delay_ms(systick_cfg_t *p_sys_config, uint32_t ms);
uint32_t hk_systick_get(systick_cfg_t *p_sys_config);
void hk_systick_obj_init(systick_cfg_t *p_sys_config);


#endif
