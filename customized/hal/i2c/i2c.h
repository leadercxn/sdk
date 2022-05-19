#ifndef __I2C_H
#define __I2C_H

#include "gpio.h"
#include <stdbool.h>

typedef enum
{
    I2C_TYPE_I2C1,
    I2C_TYPE_I2C2,
    I2C_TYPE_I2C3,
    I2C_TYPE_I2C4,
    I2C_TYPE_VIRT1,
    I2C_TYPE_VIRT2,
} i2c_type_e;

typedef struct
{
    gpio_object_t sda;
    gpio_object_t scl;

    void (* delay_us)   (uint32_t us);
} i2c_cfg_t;

typedef struct i2c_ops
{
    void (* delay_ms)   (uint32_t ms);

    void (* init)       (i2c_cfg_t *p_cfg);
    void (* xfer_start) (i2c_cfg_t *p_cfg);
    void (* xfer_stop)  (i2c_cfg_t *p_cfg);
    uint8_t (* wait_ack)   (i2c_cfg_t *p_cfg, struct i2c_ops *p_ops);
    void (* set_ack)    (i2c_cfg_t *p_cfg);
    void (* set_nack)   (i2c_cfg_t *p_cfg);
    void (* send_byte)  (i2c_cfg_t *p_cfg, uint8_t byte);
    uint8_t (* read_byte)(i2c_cfg_t *p_cfg, struct i2c_ops *p_ops, bool ack);
} i2c_ops_t;

typedef struct
{
    i2c_cfg_t i2c_cfg;
    i2c_ops_t i2c_ops;
} i2c_object_t;

#endif
