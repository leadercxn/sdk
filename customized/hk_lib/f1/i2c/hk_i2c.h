#ifndef __HK_I2C_H
#define __HK_I2C_H

#include "i2c.h"

void hk_virt_i2c_init(i2c_cfg_t *p_cfg);
void hk_virt_i2c_start(i2c_cfg_t *p_cfg);
void hk_virt_i2c_stop(i2c_cfg_t *p_cfg);
uint8_t  hk_virt_i2c_wait_ack(i2c_cfg_t *p_cfg, struct i2c_ops *p_ops);
void hk_virt_i2c_set_ack(i2c_cfg_t *p_cfg);
void hk_virt_i2c_set_nack(i2c_cfg_t *p_cfg);
void hk_virt_i2c_send_byte(i2c_cfg_t *p_cfg, uint8_t byte);
uint8_t hk_virt_i2c_read_byte(i2c_cfg_t *p_cfg, struct i2c_ops *p_ops, bool ack);

#endif

