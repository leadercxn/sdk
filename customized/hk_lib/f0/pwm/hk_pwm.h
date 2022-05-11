#ifndef __HK_PWM_H
#define __HK_PWM_H

#include "hk32f0xx.h"
#include "pwm.h"

typedef struct
{
    uint32_t        pwm_clk;

    GPIO_TypeDef    *pwm_pin_port;
    uint32_t        pwm_pin_port_clk;

    uint32_t        pwm_pin;

    uint8_t         pwm_pin_source;
    uint8_t         pwm_pin_af;
} hk_pwm_pin_cfg;

typedef struct
{
    uint32_t        freq;
    uint8_t         duty_cycle;

    uint16_t        output_polarity;
    uint16_t        output_n_polarity;
} hk_pwm_cfg;

int hk_timer15_ch1_pwm_init(pwm_cfg_t *p_pwm_cfg);

int hk_timer15_ch1_pwm_enable(pwm_cfg_t *p_pwm_cfg);

int hk_timer15_ch1_pwm_disable(pwm_cfg_t *p_pwm_cfg);


#endif
