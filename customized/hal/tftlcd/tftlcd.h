#ifndef __TFTLCD_H
#define __TFTLCD_H

#include <stdint.h>

//画笔颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40     //棕色
#define BRRED 			 0XFC07     //棕红色
#define GRAY  			 0X8430     //灰色
#define DARKBLUE      	 0X01CF	    //深蓝色
#define LIGHTBLUE      	 0X7D7C	    //浅蓝色  
#define GRAYBLUE       	 0X5458     //灰蓝色
#define LIGHTGREEN     	 0X841F     //浅绿色
#define LIGHTGRAY        0XEF5B     //浅灰色(PANNEL)
#define LGRAY 			 0XC618     //浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE        0XA651     //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12     //浅棕蓝色(选择条目的反色)

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
    uint16_t x;
    uint16_t y;
} coord_t;

typedef struct 
{
    coord_t coord;
    uint16_t color;
} point_object_t;

typedef struct chars
{
    uint8_t         num;        // 待显字符
    uint8_t         size;       // 字体大小
    uint8_t         mode;       // 是否叠加显示
    coord_t         coord;      // 字符坐标
}chars_info_t;


typedef struct 
{
    coord_t coord_s;
    coord_t coord_e;
    uint16_t color;
} graphic_object_t;

typedef graphic_object_t fill_object_t;

typedef struct 
{
    uint16_t setxcmd;
    uint16_t setycmd;
    uint16_t wramcmd;
    uint16_t width;
    uint16_t height;
    uint16_t id;
    uint16_t dir;
    uint16_t background_color;  
    uint16_t point_color;           
} tftlcd_info_t;


typedef struct 
{
    void *p_tft_cfg;        // 8080方式/fsmc方式
    tftlcd_info_t lcd_info;
} tftlcd_driver_t;


typedef struct tftlcd_cfg
{
    // lcd驱动信息
    tftlcd_driver_t *p_dri;

    // 对应lcd驱动的操作
    int (*hardware_init)    (tftlcd_driver_t *p_driver);
    int (*send_byte)        (tftlcd_driver_t *p_driver, uint16_t data);
    int (*write_cmd)        (tftlcd_driver_t *p_driver, uint16_t cmd);
    int (*write_data)       (tftlcd_driver_t *p_driver, uint16_t data);
    int (*write_burst_data) (tftlcd_driver_t *p_driver, unsigned char *buf, uint16_t cnt);
    int (*read_data)        (tftlcd_driver_t *p_driver);
    int (*write_reg)        (tftlcd_driver_t *p_driver, uint16_t cmd, uint16_t data);
    void (*delay_ms)         (uint32_t ms);
    void (*delay_us)         (uint32_t us);
} tftlcd_cfg_t;


typedef struct tftlcd_ops
{
    // private function
    int (*init)             (tftlcd_cfg_t *p_cfg, struct tftlcd_ops *p_ops);
    int (*set_cursor)       (tftlcd_cfg_t *p_cfg, uint16_t xpos, uint16_t ypos);
    int (*write_ram_pre)    (tftlcd_cfg_t *p_cfg);
    int (*write_ram)        (tftlcd_cfg_t *p_cfg, uint16_t color);
    int (*set_scan_dir)     (tftlcd_cfg_t *p_cfg, uint16_t dir);
    int (*read_reg)         (tftlcd_cfg_t *p_cfg, uint16_t cmd);

    // common function
    int (*clear_screen)     (tftlcd_cfg_t *p_cfg, struct tftlcd_ops *p_ops);
    int (*fill_area)        (tftlcd_cfg_t *p_cfg, struct tftlcd_ops *p_ops, fill_object_t area);
    int (*draw_point)       (tftlcd_cfg_t *p_cfg, struct tftlcd_ops *p_ops, point_object_t point);
    int (*show_char)        (tftlcd_cfg_t *p_cfg, struct tftlcd_ops *p_ops, chars_info_t ch);
    int (*show_string)      (tftlcd_cfg_t *p_cfg);
    int (*show_num)         (tftlcd_cfg_t *p_cfg);
    int (*draw_line)        (tftlcd_cfg_t *p_cfg);
    int (*draw_widthline)   (tftlcd_cfg_t *p_cfg);
    int (*draw_rectangle)   (tftlcd_cfg_t *p_cfg);
    int (*fill_rectangle)   (tftlcd_cfg_t *p_cfg);
    int (*draw_triangle)    (tftlcd_cfg_t *p_cfg);
    int (*fill_triangle)    (tftlcd_cfg_t *p_cfg);
    int (*show_bmp)         (tftlcd_cfg_t *p_cfg);
} tftlcd_ops_t;


typedef struct
{
    tftlcd_cfg_t tftlcd_cfg;
    tftlcd_ops_t tftlcd_ops;
} tftlcd_object_t;


// common function declare
int tftlcd_clear_screen(tftlcd_cfg_t *p_cfg, struct tftlcd_ops *p_ops);
int tftlcd_fill_area(tftlcd_cfg_t *p_cfg, struct tftlcd_ops *p_ops, fill_object_t area);
int tftlcd_draw_point(tftlcd_cfg_t *p_cfg, struct tftlcd_ops *p_ops, point_object_t point);
int tftlcd_show_char(tftlcd_cfg_t *p_cfg, struct tftlcd_ops *p_ops, chars_info_t ch);

#endif
