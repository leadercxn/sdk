#ifndef __NT35510_FSMC_H
#define __NT35510_FSMC_H

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
} nt35510_fsmc_info_t;

// basic function
int nt35510_hardware_init(tftlcd_driver_t *p_driver);
int nt35510_write_cmd(tftlcd_driver_t *p_driver, uint16_t cmd);
int nt35510_write_data(tftlcd_driver_t *p_driver, uint16_t data);
int nt35510_read_data(tftlcd_driver_t *p_driver);
int nt35510_write_reg(tftlcd_driver_t *p_driver, uint16_t cmd, uint16_t data);


// private function
int nt35510_read_reg(tftlcd_cfg_t *p_cfg, uint16_t cmd);

int nt35510_init(tftlcd_cfg_t *p_cfg, struct tftlcd_ops *p_ops);
int nt35510_set_cursor(tftlcd_cfg_t *p_cfg, uint16_t xpos, uint16_t ypos);
int nt35510_set_scan_dir(tftlcd_cfg_t *p_cfg, uint16_t dir);
int nt35510_write_ram_pre(tftlcd_cfg_t *p_cfg);
int nt35510_write_ram(tftlcd_cfg_t *p_cfg, uint16_t color);

int nt35510_clear_screen(tftlcd_cfg_t *p_cfg, struct tftlcd_ops *p_ops);
int nt35510_fill_area(tftlcd_cfg_t *p_cfg, struct tftlcd_ops *p_ops, fill_object_t area);


#endif

