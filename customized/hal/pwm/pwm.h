#ifndef __PWM_H
#define __PWM_H

#include "stdint.h"

typedef struct
{
    void *p_pin_cfg;
    void *p_pwm_cfg;

    uint32_t flag;
} pwm_cfg_t;

typedef struct
{
    int (* pwm_init)        (pwm_cfg_t *p_pwm_cfg);
    int (* pwm_enable)      (pwm_cfg_t *p_pwm_cfg);
    int (* pwm_disable)     (pwm_cfg_t *p_pwm_cfg);
    int (* pwm_param_set)   (pwm_cfg_t *p_pwm_cfg);
} pwm_ops_t;


typedef struct
{
    pwm_cfg_t pwm_cfg;
    pwm_ops_t pwm_ops;
} pwm_object_t;

#endif
