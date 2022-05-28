#include "stddef.h"
#include "stdbool.h"

#include "trace.h"
#include "lib_error.h"

#include "hk_i2c.h"

#include "trace.h"

/* 
 * 
 * summary 
 * 
 * */
void hk_virt_i2c_init(i2c_cfg_t *p_cfg)
{
    if(!p_cfg)
    {
        trace_error("p_virt_i2c is NULL\n\r");
        return;
    }

    p_cfg->sda.gpio_ops.gpio_init(&p_cfg->sda.gpio_cfg);
    p_cfg->sda.gpio_ops.gpio_output_set(&p_cfg->sda.gpio_cfg, 1);

    p_cfg->scl.gpio_ops.gpio_init(&p_cfg->scl.gpio_cfg);
    p_cfg->scl.gpio_ops.gpio_output_set(&p_cfg->scl.gpio_cfg, 1);

    p_cfg->delay_us(1000);

}

void hk_virt_i2c_start(i2c_cfg_t *p_cfg)
{
    if (!p_cfg)
    {
        return;
    }

    p_cfg->sda.gpio_ops.gpio_fix_output(&p_cfg->sda.gpio_cfg);
    p_cfg->delay_us(2);
    p_cfg->sda.gpio_ops.gpio_output_set(&p_cfg->sda.gpio_cfg, 1);
    p_cfg->scl.gpio_ops.gpio_output_set(&p_cfg->scl.gpio_cfg, 1);
    p_cfg->delay_us(30);
    p_cfg->sda.gpio_ops.gpio_output_set(&p_cfg->sda.gpio_cfg, 0);
    p_cfg->delay_us(2);
    p_cfg->scl.gpio_ops.gpio_output_set(&p_cfg->scl.gpio_cfg, 0);
}

void hk_virt_i2c_stop(i2c_cfg_t *p_cfg)
{
    if(!p_cfg)
    {
        return;
    }

    p_cfg->sda.gpio_ops.gpio_fix_output(&p_cfg->sda.gpio_cfg);
    p_cfg->delay_us(2);
    p_cfg->sda.gpio_ops.gpio_output_set(&p_cfg->sda.gpio_cfg, 0);
    p_cfg->scl.gpio_ops.gpio_output_set(&p_cfg->scl.gpio_cfg, 0);

    p_cfg->delay_us(2);
    p_cfg->scl.gpio_ops.gpio_output_set(&p_cfg->scl.gpio_cfg, 1);
    p_cfg->delay_us(2);
    p_cfg->sda.gpio_ops.gpio_output_set(&p_cfg->sda.gpio_cfg, 1);
}

uint8_t hk_virt_i2c_wait_ack(i2c_cfg_t *p_cfg, struct i2c_ops *p_ops)
{
    uint8_t timeout = 0;
    uint8_t sda_value = 0;

    if(!p_cfg)
    {
        return -EINVAL;
    }
    
    p_cfg->sda.gpio_ops.gpio_fix_input(&p_cfg->sda.gpio_cfg);
    p_cfg->delay_us(2);
    p_cfg->sda.gpio_ops.gpio_output_set(&p_cfg->sda.gpio_cfg, 1);
    p_cfg->scl.gpio_ops.gpio_output_set(&p_cfg->scl.gpio_cfg, 1);
    p_cfg->delay_us(2);
    p_cfg->sda.gpio_ops.gpio_input_get(&p_cfg->sda.gpio_cfg, &sda_value);

    while(sda_value)
    {
        timeout++;
        if(timeout > 250)
        {
            p_ops->xfer_stop(p_cfg);
            return -EIO;
        }
        p_cfg->sda.gpio_ops.gpio_input_get(&p_cfg->sda.gpio_cfg, &sda_value);
        p_cfg->delay_us(2);
    }

    p_cfg->scl.gpio_ops.gpio_output_set(&p_cfg->scl.gpio_cfg, 0);

    return ENONE;
}

void hk_virt_i2c_set_ack(i2c_cfg_t *p_cfg)
{
    if(!p_cfg)
    {
        return;
    }

    p_cfg->scl.gpio_ops.gpio_output_set(&p_cfg->scl.gpio_cfg, 0);
    p_cfg->sda.gpio_ops.gpio_fix_output(&p_cfg->sda.gpio_cfg);
    p_cfg->sda.gpio_ops.gpio_output_set(&p_cfg->sda.gpio_cfg, 0);
    p_cfg->delay_us(2);

    p_cfg->scl.gpio_ops.gpio_output_set(&p_cfg->scl.gpio_cfg, 1);
    p_cfg->delay_us(2);
    p_cfg->scl.gpio_ops.gpio_output_set(&p_cfg->scl.gpio_cfg, 0);
}

void hk_virt_i2c_set_nack(i2c_cfg_t *p_cfg)
{
    if (!p_cfg)
    {
        return;
    }

    p_cfg->sda.gpio_ops.gpio_fix_output(&p_cfg->sda.gpio_cfg);
    p_cfg->delay_us(2);
    p_cfg->scl.gpio_ops.gpio_output_set(&p_cfg->scl.gpio_cfg, 0);
    p_cfg->sda.gpio_ops.gpio_output_set(&p_cfg->sda.gpio_cfg, 1);
    p_cfg->delay_us(2);

    p_cfg->scl.gpio_ops.gpio_output_set(&p_cfg->scl.gpio_cfg, 1);
    p_cfg->delay_us(2);
    p_cfg->scl.gpio_ops.gpio_output_set(&p_cfg->scl.gpio_cfg, 0);
}


void hk_virt_i2c_send_byte(i2c_cfg_t *p_cfg, uint8_t byte)
{
    uint8_t i = 0;
    uint8_t temp = 0;

    p_cfg->sda.gpio_ops.gpio_fix_output(&p_cfg->sda.gpio_cfg);
    p_cfg->delay_us(2);
    p_cfg->scl.gpio_ops.gpio_output_set(&p_cfg->scl.gpio_cfg, 0);
    p_cfg->delay_us(2);

    for(i = 0; i < 8; i++)
    {
        temp = (byte & 0x80) >> 7;

        p_cfg->sda.gpio_ops.gpio_output_set(&p_cfg->sda.gpio_cfg, temp);
        p_cfg->delay_us(2);

        p_cfg->scl.gpio_ops.gpio_output_set(&p_cfg->scl.gpio_cfg, 1);
        p_cfg->delay_us(2);
        p_cfg->scl.gpio_ops.gpio_output_set(&p_cfg->scl.gpio_cfg, 0);
        p_cfg->delay_us(2);

        byte <<= 1;
    }

    // p_cfg->sda.gpio_ops.gpio_output_set(&p_cfg->sda.gpio_cfg, 1);
}

/**
 * @brief 
 * 
 * @param ack 读取完一个字节后是否发送一个应答信号
 */
uint8_t hk_virt_i2c_read_byte(i2c_cfg_t *p_cfg, struct i2c_ops *p_ops, bool ack)
{
    uint8_t i = 0,recv_byte = 0;
    uint8_t sda_value = 0;

    p_cfg->sda.gpio_ops.gpio_fix_input(&p_cfg->sda.gpio_cfg);
    p_cfg->delay_us(50);

    for(i = 0; i < 8; i++)
    {
        p_cfg->scl.gpio_ops.gpio_output_set(&p_cfg->scl.gpio_cfg, 0);
        p_cfg->delay_us(2);
        p_cfg->scl.gpio_ops.gpio_output_set(&p_cfg->scl.gpio_cfg, 1);
        recv_byte <<= 1;

        p_cfg->sda.gpio_ops.gpio_input_get(&p_cfg->sda.gpio_cfg, &sda_value);
        if (sda_value)
        {
            recv_byte++;
        }
        p_cfg->delay_us(2);
    }

    if (ack)
    {
        p_ops->set_ack(p_cfg);
    }
    else
    {
        p_ops->set_nack(p_cfg);
    }

    return recv_byte;
}

