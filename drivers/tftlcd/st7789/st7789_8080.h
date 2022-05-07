#ifndef __ST7789_8080_H
#define __ST7789_8080_H

#include "tftlcd.h"
#include "gpio.h"

// 定义一个st7789专用结构体
typedef struct 
{
    gpio_object_t   cs_pin;
    gpio_object_t   rst_pin;
    gpio_object_t   rs_pin;
    gpio_object_t   wr_pin;
    gpio_object_t   rd_pin;
    gpio_object_t   db_pin;      // 固定为Px0~7
} st7789_8080_info_t;


// basic function
int st7789_hardware_init(tftlcd_driver_t *p_driver);
int st7789_send_byte(tftlcd_driver_t *p_driver, uint16_t data);
int st7789_write_cmd(tftlcd_driver_t *p_driver, uint16_t cmd);
int st7789_write_data(tftlcd_driver_t *p_driver, uint16_t data);
int st7789_write_burst_data(tftlcd_driver_t *p_driver, unsigned char *buf, uint16_t cnt);

// private function
int st7789_init(tftlcd_cfg_t *p_cfg, struct tftlcd_ops *p_ops);
int st7789_set_cursor(tftlcd_cfg_t *p_cfg, uint16_t xpos, uint16_t ypos);
int st7789_write_ram_pre(tftlcd_cfg_t *p_cfg);
int st7789_write_ram(tftlcd_cfg_t *p_cfg, uint16_t color);
int st7789_set_scan_dir(tftlcd_cfg_t *p_cfg, uint16_t dir);
int st7789_clear_screen(tftlcd_cfg_t *p_cfg, struct tftlcd_ops *p_ops);
int st7789_fill_area(tftlcd_cfg_t *p_cfg, struct tftlcd_ops *p_ops, fill_object_t area);

#endif

