#ifndef __ENCODER_H
#define __ENCODER_H

#include "gpio.h"
#include "exit.h"

typedef struct encoder
{
    exit_object_t *pin_sw;
    exit_object_t *pin_exit;
    gpio_object_t *pin_gpio;
    int8_t  step;
    uint8_t dir;            // 0:初始状态，1:正转1格，2:反转1格
}encoder_object_t;

void encoder_init(encoder_object_t *p_encoder_obj);
void encoder_sw_handler(exit_cfg_t *p_exit_cfg);
void encoder_data_handler(exit_cfg_t *p_exit_cfg);

#endif
