#ifndef __GPIO_H
#define __GPIO_H

#include <stdint.h>

typedef enum
{
    GPIO_DIR_OUTPUR,
    GPIO_DIR_INPUT,
} gpio_dir_e;

typedef enum
{
    GPIO_TYPE_IO,
    GPIO_TYPE_AF,
} gpio_type_e;

typedef struct
{
    uint32_t    gpio_clk;
    void        *p_port;
    uint32_t    gpio_pin;
    gpio_dir_e  gpio_dir;
    uint32_t    flag;
} gpio_cfg_t;

typedef struct
{
    int (* gpio_init)       (gpio_cfg_t *p_cfg);
    int (* gpio_output_set) (gpio_cfg_t *p_cfg, uint8_t value);
    int (* gpio_input_get)  (gpio_cfg_t *p_cfg, uint8_t *p_value);
    int (* gpio_fix_input)  (gpio_cfg_t *p_cfg);
    int (* gpio_fix_output) (gpio_cfg_t *p_cfg);
} gpio_ops_t;


typedef struct
{
    gpio_cfg_t gpio_cfg;
    gpio_ops_t gpio_ops;
} gpio_object_t;



#endif
