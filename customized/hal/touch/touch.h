#ifndef __TOUCH_H
#define __TOUCH_H

#include <stdint.h>

#include "i2c.h"

#define TP_PRES_DOWN 0x80  		//触屏被按下	  
#define TP_CATH_PRES 0x40  		//有按键按下了 

#define CT_MAX_TOUCH  5    		//电容屏支持的点数,固定为5点

//触摸屏控制器
typedef struct 
{
	uint16_t x[CT_MAX_TOUCH]; 		//当前坐标
	uint16_t y[CT_MAX_TOUCH];		//电容屏有最多5组坐标,电阻屏则用x[0],y[0]代表:此次扫描时,触屏的坐标,用
								//x[4],y[4]存储第一次按下时的坐标. 
	uint16_t lcd_width;
	uint16_t lcd_height;
	uint8_t  sta;				//笔的状态 
								//b7:按下1/松开0; 
	                            //b6:0,没有按键按下;1,有按键按下. 
								//b5:保留
								//b4~b0:电容触摸屏按下的点数(0,表示未按下,1表示按下)

//新增的参数,当触摸屏的左右上下完全颠倒时需要用到.
//b0:0,竖屏(适合左右为X坐标,上下为Y坐标的TP)
//   1,横屏(适合左右为Y坐标,上下为X坐标的TP) 
//b1~6:保留.
//b7:0,电阻屏
//   1,电容屏 
	uint8_t touchtype;

} tp_dev_t;

typedef struct 
{
    i2c_object_t  p_i2c_obj;
    gpio_object_t p_rst_obj;
    gpio_object_t p_int_obj;

    tp_dev_t      *p_touch_dev;
} touch_cfg_t;

typedef struct 
{
    uint8_t (* send_cfg)    (touch_cfg_t *p_cfg, uint8_t mode);
    uint8_t (* write_reg)   (touch_cfg_t *p_cfg, uint16_t reg, uint8_t *buf, uint8_t len);
    void    (* read_reg)    (touch_cfg_t *p_cfg, uint16_t reg, uint8_t *buf, uint8_t len);
    uint8_t (* init)        (touch_cfg_t *p_cfg);
    uint8_t (* scan)        (touch_cfg_t *p_cfg, uint8_t mode);
} touch_ops_t;

typedef struct
{
    touch_cfg_t touch_cfg;
    touch_ops_t touch_ops;
} touch_object_t;

#endif
