#ifndef __ST7789_FSMC_H
#define __ST7789_FSMC_H

#include "tftlcd.h"
#include "gpio.h"

typedef struct 
{
    gpio_object_t rst_pin;
    gpio_object_t bl_pin;
    gpio_object_t fsmc_pin1;
    gpio_object_t fsmc_pin2;
    uint32_t    base_addr;
    void        *lcd_addr;
    uint8_t     buswidth;
} st7789_fsmc_info_t;

// basic function
int st7789_hw_fsmc_init(tftlcd_driver_t *p_driver);
int st7789_write_cmd_fsmc(tftlcd_driver_t *p_driver, uint16_t cmd);
int st7789_write_data_fsmc(tftlcd_driver_t *p_driver, uint16_t data);
int st7789_read_data_fsmc(tftlcd_driver_t *p_driver);
int st7789_write_reg_fsmc(tftlcd_driver_t *p_driver, uint16_t cmd, uint16_t data);


#endif
