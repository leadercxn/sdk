#include "stm8_gpio.h"

int stm8s_gpio_obj_init(gpio_cfg_t *p_cfg)
{
    if(p_cfg)
    {
        GPIO_Init(p_cfg->p_port, p_cfg->gpio_pin, p_cfg->mode);
    }

    return 0;
}

int stm8_gpio_obj_out_set(gpio_cfg_t *p_cfg, uint8_t value)
{
    if(value)
    {
        GPIO_WriteHigh((GPIO_TypeDef *)p_cfg->p_port, p_cfg->gpio_pin);
    }
    else
    {
        GPIO_WriteLow((GPIO_TypeDef *)p_cfg->p_port, p_cfg->gpio_pin);
    }

    return 0;
}


