#ifndef __TOUCH_H
#define __TOUCH_H

#include <stdint.h>

#include "i2c.h"

typedef struct 
{
    i2c_object_t  p_i2c_obj;
    gpio_object_t p_rst_obj;
    gpio_object_t p_int_obj;

    void *p_touch_cfg;
} touch_cfg_t;

typedef struct 
{
    uint8_t (* send_cfg)    (touch_cfg_t *p_cfg, uint8_t mode);
    uint8_t (* write_reg)   (touch_cfg_t *p_cfg, uint16_t reg, uint8_t *buf, uint8_t len);
    void    (* read_reg)    (touch_cfg_t *p_cfg, uint16_t reg, uint8_t *buf, uint8_t len);
    uint8_t (* init)        (touch_cfg_t *p_cfg);
    uint8_t (* scan)        (touch_cfg_t *p_cfg, uint8_t mode);
} touch_ops_t;

typedef struct
{
    touch_cfg_t touch_cfg;
    touch_ops_t touch_ops;
} touch_object_t;

#endif
