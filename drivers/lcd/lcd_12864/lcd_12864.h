#ifndef __LCD_12864_H
#define __LCD_12864_H

#include "gpio.h"

/**
 * lcd-8080 接口
 */
union data_port
{
    uint8_t byte;

    struct
    {   
        uint8_t db0 : 1;       //低位
        uint8_t db1 : 1;
        uint8_t db2 : 1;
        uint8_t db3 : 1;
        uint8_t db4 : 1;
        uint8_t db5 : 1;
        uint8_t db6 : 1;
        uint8_t db7 : 1;
    } db;
};

typedef struct
{
//    gpio_object_t     rst_pin;  //不包含rst。外部复位
    gpio_object_t       cs1_pin;
    gpio_object_t       cs2_pin;
    gpio_object_t       rw_pin;
    gpio_object_t       e_pin;
    gpio_object_t       rs_pin;

    union data_port     parallel_port;
}lcd_12864_t;


#endif
