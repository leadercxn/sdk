#ifndef __ILI9341_FSMC_H
#define __ILI9341_FSMC_H

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
} ili9341_fsmc_info_t;

// basic function
int ili9341_hardware_init(tftlcd_driver_t *p_driver);
int ili9341_write_cmd(tftlcd_driver_t *p_driver, uint16_t cmd);
int ili9341_write_data(tftlcd_driver_t *p_driver, uint16_t data);
int ili9341_read_data(tftlcd_driver_t *p_driver);
int ili9341_write_reg(tftlcd_driver_t *p_driver, uint16_t cmd, uint16_t data);


// private function
int ili9341_read_reg(tftlcd_cfg_t *p_cfg, uint16_t cmd);
int ili9341_init(tftlcd_cfg_t *p_cfg, struct tftlcd_ops *p_ops);
int ili9341_set_cursor(tftlcd_cfg_t *p_cfg, uint16_t xpos, uint16_t ypos);
int ili9341_set_scan_dir(tftlcd_cfg_t *p_cfg, uint16_t dir);
int ili9341_write_ram_pre(tftlcd_cfg_t *p_cfg);
int ili9341_write_ram(tftlcd_cfg_t *p_cfg, uint16_t color);


#endif

