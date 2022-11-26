#ifndef __I2C_RESTRUCT_V1_1_H
#define __I2C_RESTRUCT_V1_1_H

#include "stdint.h"

/**
 * base i2c
 */

typedef struct
{
    void *p_pin_cfg;
    void *p_i2c_cfg;

    uint32_t speed;
    uint32_t flag;
} i2c_cfg_t;

typedef struct
{
    int (* i2c_init)            (i2c_cfg_t *p_i2c_cfg);
    int (* i2c_enable)          (i2c_cfg_t *p_i2c_cfg);
    int (* i2c_disable)         (i2c_cfg_t *p_i2c_cfg);
    int (* i2c_update_param)    (i2c_cfg_t *p_i2c_cfg);
    int (* i2c_read)            (i2c_cfg_t *p_i2c_cfg, uint8_t *p_data, uint16_t len);
    int (* i2c_write)           (i2c_cfg_t *p_i2c_cfg, uint8_t *p_data, uint16_t len);
} i2c_ops_t;

typedef struct
{
    i2c_cfg_t   i2c_cfg;
    i2c_ops_t   i2c_ops;
} i2c_object_t;


#endif

