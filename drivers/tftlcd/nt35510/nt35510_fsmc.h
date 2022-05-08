#ifndef __NT35510_FSMC_H
#define __NT35510_FSMC_H

#include "tftlcd.h"
#include "gpio.h"

// FSMC相关设置
// 此处只针对F1系列而言，其FSMC控制4个BANK，其中BANK1是控制SRAM，BANK1有4个扇区，每个扇区64M
#define FSMC_BASE_ADDR          0x60000000
#define BANK1_SECTOR1_OFFSET    0x00000000
#define BANK1_SECTOR2_OFFSET    0x04000000
#define BANK1_SECTOR3_OFFSET    0x08000000
#define BANK1_SECTOR4_OFFSET    0x0C000000

// 此处是针对8b的外设RAM，当RS接到对应的Ax，需要设置的offset值
#define BANK_8B_A0_OFFSET       0x0000
#define BANK_8B_A1_OFFSET       0x0001
#define BANK_8B_A2_OFFSET       0x0003
#define BANK_8B_A3_OFFSET       0x0007
#define BANK_8B_A4_OFFSET       0x000F
#define BANK_8B_A5_OFFSET       0x001F
#define BANK_8B_A6_OFFSET       0x003F
#define BANK_8B_A7_OFFSET       0x007F
#define BANK_8B_A8_OFFSET       0x00FF
#define BANK_8B_A9_OFFSET       0x01FF
#define BANK_8B_A10_OFFSET      0x03FF
#define BANK_8B_A11_OFFSET      0x07FF
#define BANK_8B_A12_OFFSET      0x0FFF
#define BANK_8B_A13_OFFSET      0x1FFF
#define BANK_8B_A14_OFFSET      0x3FFF
#define BANK_8B_A15_OFFSET      0x7FFF
#define BANK_8B_A16_OFFSET      0xFFFF

// 此处是针对16b的外设RAM，当RS接到对应的Ax，需要设置的offset值
#define BANK_16B_A0_OFFSET       0x0000
#define BANK_16B_A1_OFFSET       0x0002
#define BANK_16B_A2_OFFSET       0x0006
#define BANK_16B_A3_OFFSET       0x000E
#define BANK_16B_A4_OFFSET       0x001E
#define BANK_16B_A5_OFFSET       0x003E
#define BANK_16B_A6_OFFSET       0x007E
#define BANK_16B_A7_OFFSET       0x00FE
#define BANK_16B_A8_OFFSET       0x01FE
#define BANK_16B_A9_OFFSET       0x03FE
#define BANK_16B_A10_OFFSET      0x07FE
#define BANK_16B_A11_OFFSET      0x0FFE
#define BANK_16B_A12_OFFSET      0x1FFE
#define BANK_16B_A13_OFFSET      0x3FFE
#define BANK_16B_A14_OFFSET      0x7FFE
#define BANK_16B_A15_OFFSET      0xFFFE
#define BANK_16B_A16_OFFSET      0x1FFFE


enum lcdbuswidth
{
    LCDBUSWIDTH_8B = 0,
    LCDBUSWIDTH_16B = 1,
};

typedef struct 
{
    uint8_t lcd_cmd;
    uint8_t lcd_data;
} lcdtype_8b_t;

typedef struct 
{
    uint16_t lcd_cmd;
    uint16_t lcd_data;
} lcdtype_16b_t;

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

