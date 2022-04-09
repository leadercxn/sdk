#ifndef __GPIO_H
#define __GPIO_H

typedef enum
{
    GPIO_DIR_OUTPUR,
    GPIO_DIR_INPUT,
} gpio_dir_e;


typedef struct
{
    uint32_t    gpio_clk;
    void        *port;
    uint32_t    gpio_pin;
    gpio_dir_e  gpio_dir;
    uint32_t    flag;
} gpio_cfg_t;

typedef struct
{
    int (* gpio_config)     (gpio_cfg_t *p_cfg);
    int (* gpio_output_set) (gpio_cfg_t *p_cfg, uint8_t value);
    int (* gpio_input_get)  (gpio_cfg_t *p_cfg, uint8_t *p_value);
} gpio_ops_t;


typedef struct
{
    gpio_cfg_t gpio_cfg;
    gpio_ops_t gpio_ops;
} gpio_object_t;



#endif
