#include "stddef.h"
#include "stdbool.h"

#include "board_config.h"
#include "hk_i2c.h"


/**
 * @brief 做一个简单的延时
 */
static void hk_i2c_delay(void)
{
    uint8_t i;
    for(i = 0; i < 4; i++);
}


/* 
 * 
 * summary 
 * 
 * */
void hk_virt_i2c_init(hk_virt_i2c_t *p_virt_i2c)
{
    if(!p_virt_i2c)
    {
        trace_error("p_virt_i2c is NULL\n\r");
        return;
    }

    conf_gpio_output(p_virt_i2c->scl_port_periph_clk, p_virt_i2c->p_scl_gpio_port, p_virt_i2c->scl_gpio_pin);
    set_gpio_value(p_virt_i2c->p_scl_gpio_port, p_virt_i2c->scl_gpio_pin, 1);

    conf_gpio_output(p_virt_i2c->sda_port_periph_clk, p_virt_i2c->p_sda_gpio_port, p_virt_i2c->sda_gpio_pin);
    set_gpio_value(p_virt_i2c->p_sda_gpio_port, p_virt_i2c->sda_gpio_pin, 1);
}

void hk_virt_i2c_start(hk_virt_i2c_t *p_virt_i2c)
{
    if(!p_virt_i2c)
    {
        return;
    }

    conf_gpio_output(p_virt_i2c->sda_port_periph_clk, p_virt_i2c->p_sda_gpio_port, p_virt_i2c->sda_gpio_pin);
    set_gpio_value(p_virt_i2c->p_sda_gpio_port, p_virt_i2c->sda_gpio_pin, 1);
    set_gpio_value(p_virt_i2c->p_scl_gpio_port, p_virt_i2c->scl_gpio_pin, 1);

    hk_i2c_delay();
    set_gpio_value(p_virt_i2c->p_sda_gpio_port, p_virt_i2c->sda_gpio_pin, 0);

    hk_i2c_delay();
    set_gpio_value(p_virt_i2c->p_scl_gpio_port, p_virt_i2c->scl_gpio_pin, 0);
}

void hk_virt_i2c_stop(hk_virt_i2c_t *p_virt_i2c)
{
    if(!p_virt_i2c)
    {
        return;
    }

    conf_gpio_output(p_virt_i2c->sda_port_periph_clk, p_virt_i2c->p_sda_gpio_port, p_virt_i2c->sda_gpio_pin);
    set_gpio_value(p_virt_i2c->p_sda_gpio_port, p_virt_i2c->sda_gpio_pin, 0);
    set_gpio_value(p_virt_i2c->p_scl_gpio_port, p_virt_i2c->scl_gpio_pin, 0);

    hk_i2c_delay();
    set_gpio_value(p_virt_i2c->p_scl_gpio_port, p_virt_i2c->scl_gpio_pin, 1);
    hk_i2c_delay();
    set_gpio_value(p_virt_i2c->p_sda_gpio_port, p_virt_i2c->sda_gpio_pin, 1);
}

int hk_virt_i2c_wait_ack(hk_virt_i2c_t *p_virt_i2c)
{
    uint8_t timeout = 0;
    uint8_t sda_value = 0;

    if(!p_virt_i2c)
    {
        return -EINVAL;
    }
    
    conf_gpio_input(p_virt_i2c->sda_port_periph_clk, p_virt_i2c->p_sda_gpio_port, p_virt_i2c->sda_gpio_pin, GPIO_PuPd_UP);

    set_gpio_value(p_virt_i2c->p_scl_gpio_port, p_virt_i2c->scl_gpio_pin, 1);
    hk_i2c_delay();

    get_gpio_value(p_virt_i2c->p_sda_gpio_port, p_virt_i2c->sda_gpio_pin, &sda_value);
    while(sda_value)
    {
        timeout++;
        if(timeout > 250)
        {
            hk_virt_i2c_stop(p_virt_i2c);
            return -EIO;
        }

        get_gpio_value(p_virt_i2c->p_sda_gpio_port, p_virt_i2c->sda_gpio_pin, &sda_value);
        hk_i2c_delay();
    }

    set_gpio_value(p_virt_i2c->p_scl_gpio_port, p_virt_i2c->scl_gpio_pin, 0);

    return ENONE;
}

void hk_virt_i2c_set_ack(hk_virt_i2c_t *p_virt_i2c)
{
    if(!p_virt_i2c)
    {
        return;
    }

    set_gpio_value(p_virt_i2c->p_scl_gpio_port, p_virt_i2c->scl_gpio_pin, 0);
    conf_gpio_output(p_virt_i2c->sda_port_periph_clk, p_virt_i2c->p_sda_gpio_port, p_virt_i2c->sda_gpio_pin);
    set_gpio_value(p_virt_i2c->p_sda_gpio_port, p_virt_i2c->sda_gpio_pin, 0);

    hk_i2c_delay();

    set_gpio_value(p_virt_i2c->p_scl_gpio_port, p_virt_i2c->scl_gpio_pin, 1);
    hk_i2c_delay();
    set_gpio_value(p_virt_i2c->p_scl_gpio_port, p_virt_i2c->scl_gpio_pin, 0);
}

void hk_virt_i2c_set_nack(hk_virt_i2c_t *p_virt_i2c)
{
    if(!p_virt_i2c)
    {
        return;
    }

    set_gpio_value(p_virt_i2c->p_scl_gpio_port, p_virt_i2c->scl_gpio_pin, 0);
    conf_gpio_output(p_virt_i2c->sda_port_periph_clk, p_virt_i2c->p_sda_gpio_port, p_virt_i2c->sda_gpio_pin);
    set_gpio_value(p_virt_i2c->p_sda_gpio_port, p_virt_i2c->sda_gpio_pin, 1);

    hk_i2c_delay();

    set_gpio_value(p_virt_i2c->p_scl_gpio_port, p_virt_i2c->scl_gpio_pin, 1);
    hk_i2c_delay();
    set_gpio_value(p_virt_i2c->p_scl_gpio_port, p_virt_i2c->scl_gpio_pin, 0);
}

void hk_virt_i2c_send_byte(hk_virt_i2c_t *p_virt_i2c, uint8_t byte)
{
    uint8_t i = 0;
    uint8_t temp = 0;

    conf_gpio_output(p_virt_i2c->sda_port_periph_clk, p_virt_i2c->p_sda_gpio_port, p_virt_i2c->sda_gpio_pin);
    set_gpio_value(p_virt_i2c->p_scl_gpio_port, p_virt_i2c->scl_gpio_pin, 0);

    for(i = 0; i < 8; i++)
    {
        temp = (byte & 0x80) >> 7;

        set_gpio_value(p_virt_i2c->p_sda_gpio_port, p_virt_i2c->sda_gpio_pin, temp);
        hk_i2c_delay();
        set_gpio_value(p_virt_i2c->p_scl_gpio_port, p_virt_i2c->scl_gpio_pin, 1);
        hk_i2c_delay();
        set_gpio_value(p_virt_i2c->p_scl_gpio_port, p_virt_i2c->scl_gpio_pin, 0);
        hk_i2c_delay();

        byte <<= 1;
    }

    set_gpio_value(p_virt_i2c->p_sda_gpio_port, p_virt_i2c->sda_gpio_pin, 1);
}

/**
 * @brief 
 * 
 * @param ack 读取完一个字节后是否发送一个应答信号
 */
uint8_t hk_virt_i2c_read_byte(hk_virt_i2c_t *p_virt_i2c, bool ack)
{
    uint8_t i = 0,recv_byte = 0;
    uint8_t sda_value = 0;

    conf_gpio_input(p_virt_i2c->sda_port_periph_clk, p_virt_i2c->p_sda_gpio_port, p_virt_i2c->sda_gpio_pin, GPIO_PuPd_UP);

    for(i = 0; i < 8; i++)
    {
        recv_byte <<= 1;

        set_gpio_value(p_virt_i2c->p_scl_gpio_port, p_virt_i2c->scl_gpio_pin, 0);
        hk_i2c_delay();
        set_gpio_value(p_virt_i2c->p_scl_gpio_port, p_virt_i2c->scl_gpio_pin, 1);

        get_gpio_value(p_virt_i2c->p_sda_gpio_port, p_virt_i2c->sda_gpio_pin, &sda_value);
        if(sda_value)
        {
            recv_byte++;
        }
        hk_i2c_delay();
    }

    if(ack)
    {
        hk_virt_i2c_set_ack(p_virt_i2c);
    }
    else
    {
        hk_virt_i2c_set_nack(p_virt_i2c);
    }

    return recv_byte;
}

