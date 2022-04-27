#ifndef __HK_EXIT_H
#define __HK_EXIT_H

#include "exit.h"

#include "hk32f0xx.h"

typedef struct
{
    uint32_t     exit_clk;

    GPIO_TypeDef *exit_port;
    uint32_t     exit_pin_clk;

    uint16_t     exit_pin;
    uint16_t     exit_pin_source;
} hk_exit_pin_cfg;

typedef struct
{
    uint8_t     exit_mode;
    uint8_t     exit_trigger;
    uint8_t     exit_line_cmd;
    uint32_t    exit_line;
} hk_exit_cfg;


int hk_exit4_15_init(exit_cfg_t *p_exit_cfg);

int hk_exit4_15_enable(exit_cfg_t *p_exit_cfg);

int hk_exit4_15_disable(exit_cfg_t *p_exit_cfg);

#endif
