#ifndef __HK_TIMER_H
#define __HK_TIMER_H

#include "stm32f10x.h"
#include "timer.h"

int hk_timer3_init(timer_cfg_t *p_timer_cfg);
int hk_timer3_start(timer_cfg_t *p_timer_cfg);
int hk_timer3_stop (timer_cfg_t *p_timer_cfg);
int hk_timer3_irq_enable    (timer_cfg_t *p_timer_cfg);
int hk_timer3_irq_disable   (timer_cfg_t *p_timer_cfg);
void TIM2_Configuration(void);

#endif
