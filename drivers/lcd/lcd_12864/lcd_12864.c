#include "stdio.h"
#include "string.h"
#include "stdbool.h"

#include "boards.h"
#include "util.h"
#include "lcd_12864.h"


int lcd_12864_init(lcd_12864_t *p_lcd_12864_obj)
{
    IS_NULL(p_lcd_12864_obj);

    int      err_code = 0;

    p_lcd_12864_obj->cs1_pin.gpio_ops.gpio_init(&p_lcd_12864_obj->cs1_pin.gpio_cfg);

    p_lcd_12864_obj->e_pin.gpio_ops.gpio_output_set(&p_lcd_12864_obj->e_pin.gpio_cfg , 0);
    p_lcd_12864_obj->cs1_pin.gpio_ops.gpio_output_set(&p_lcd_12864_obj->cs1_pin.gpio_cfg , 1);
    p_lcd_12864_obj->cs2_pin.gpio_ops.gpio_output_set(&p_lcd_12864_obj->cs2_pin.gpio_cfg , 0);
}
