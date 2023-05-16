#ifndef __STM8_GPIO_H
#define __STM8_GPIO_H

#include "gpio.h"
#include "stm8s.h"
#include "stm8s_gpio.h"

int stm8s_gpio_obj_init(gpio_cfg_t *p_cfg);
int stm8_gpio_obj_out_set(gpio_cfg_t *p_cfg, uint8_t value);

#endif
