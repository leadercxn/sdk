#ifndef __HK_I2C_H
#define __HK_I2C_H

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
    uint32_t    sda_port_periph_clk;
    uint32_t    scl_port_periph_clk;
    GPIO_TypeDef *p_sda_gpio_port;
    GPIO_TypeDef *p_scl_gpio_port;
    uint16_t    sda_gpio_pin;
    uint16_t    scl_gpio_pin;
} hk_virt_i2c_t;

void hk_virt_i2c_init(hk_virt_i2c_t *p_virt_i2c);
void hk_virt_i2c_start(hk_virt_i2c_t *p_virt_i2c);
void hk_virt_i2c_stop(hk_virt_i2c_t *p_virt_i2c);
int  hk_virt_i2c_wait_ack(hk_virt_i2c_t *p_virt_i2c);
void hk_virt_i2c_set_ack(hk_virt_i2c_t *p_virt_i2c);
void hk_virt_i2c_set_nack(hk_virt_i2c_t *p_virt_i2c);
void hk_virt_i2c_send_byte(hk_virt_i2c_t *p_virt_i2c, uint8_t byte);
uint8_t hk_virt_i2c_read_byte(hk_virt_i2c_t *p_virt_i2c, bool ack);

#endif

