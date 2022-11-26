#ifndef __HK_EXIT_H
#define __HK_EXIT_H

#include "exit.h"
#include "gpio.h"
#include "stm32f10x.h"

typedef struct
{
    gpio_object_t *exit_gpio_cfg;

    uint32_t     exit_clk;      // AFIO clk
    uint8_t      exit_pin_port_source;
    uint8_t      exit_pin_source;

    uint32_t    press_cnt;

} hk_exit_pin_cfg;

typedef struct
{
    uint8_t     exit_mode;
    uint8_t     exit_trigger;
    uint8_t     exit_line_cmd;
    uint32_t    exit_line;
    IRQn_Type   exit_irqn;
    uint8_t     exit_pre_prio;
    uint8_t     exit_sub_prio;
} hk_exit_cfg;


int hk_exit_init(exit_cfg_t *p_exit_cfg);

int hk_exit_enable(exit_cfg_t *p_exit_cfg);

int hk_exit_disable(exit_cfg_t *p_exit_cfg);

void exit0_irq_handler(exit_cfg_t *p_exit_cfg);
void exit_irq_handler(exit_cfg_t *p_exit_cfg);
void exit13_irq_handler(exit_cfg_t *p_exit_cfg);

#endif
