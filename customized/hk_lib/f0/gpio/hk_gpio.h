#ifndef __HK_GPIO_H
#define __HK_GPIO_H

#include "hk32f0xx.h"
#include "gpio.h"
#include "stdbool.h"

void conf_gpio_output(uint32_t ahbperiph, GPIO_TypeDef *gpio_port, uint32_t gpio_pin);

void set_gpio_value(GPIO_TypeDef *gpio_port , uint32_t gpio_pin ,uint8_t value);

void conf_gpio_input(uint32_t ahbperiph, GPIO_TypeDef *gpio_port, uint32_t gpio_pin, GPIOPuPd_TypeDef pupd);

void get_gpio_value(GPIO_TypeDef *gpio_port , uint32_t gpio_pin ,uint8_t *p_value);

void conf_whole_gpios_output(uint32_t ahbperiph, GPIO_TypeDef *gpio_port, uint16_t gpio_pins);

void set_halt_gpios_value(GPIO_TypeDef *gpio_port ,uint8_t value, bool lsb);

void conf_whole_gpios_input(uint32_t ahbperiph, GPIO_TypeDef *gpio_port, uint16_t gpio_pins, GPIOPuPd_TypeDef pupd);

void get_halt_gpios_value(GPIO_TypeDef *gpio_port ,bool lsb, uint8_t *p_value);

void output_shift_to_input(GPIO_TypeDef *gpio_port, uint16_t gpio_pin, GPIOPuPd_TypeDef pupd);

void input_shift_to_output(GPIO_TypeDef *gpio_port, uint16_t gpio_pin);

int hk_gpio_obj_init(gpio_cfg_t *p_cfg);
int hk_gpio_obj_out_set(gpio_cfg_t *p_cfg, uint8_t value);

#endif

