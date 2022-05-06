#include "st7789_8080.h"
#include "stm32f10x.h"
#include "trace.h"

// st7789最底层接口
int st7789_hardware_init(tftlcd_driver_t *p_driver)
{
    st7789_8080_info_t *p_drv_info = (st7789_8080_info_t *)p_driver->p_tft_cfg;

    p_drv_info->cs_pin.gpio_ops.gpio_init(&p_drv_info->cs_pin.gpio_cfg);
    p_drv_info->rst_pin.gpio_ops.gpio_init(&p_drv_info->rst_pin.gpio_cfg);
    p_drv_info->rs_pin.gpio_ops.gpio_init(&p_drv_info->rs_pin.gpio_cfg);
    p_drv_info->wr_pin.gpio_ops.gpio_init(&p_drv_info->wr_pin.gpio_cfg);
    p_drv_info->rd_pin.gpio_ops.gpio_init(&p_drv_info->rd_pin.gpio_cfg);
    p_drv_info->db_pin.gpio_ops.gpio_init(&p_drv_info->db_pin.gpio_cfg);
    
    p_drv_info->cs_pin.gpio_ops.gpio_output_set(&p_drv_info->cs_pin.gpio_cfg, 1);
    p_drv_info->rst_pin.gpio_ops.gpio_output_set(&p_drv_info->rst_pin.gpio_cfg, 1);
    p_drv_info->rs_pin.gpio_ops.gpio_output_set(&p_drv_info->rs_pin.gpio_cfg, 1);
    p_drv_info->wr_pin.gpio_ops.gpio_output_set(&p_drv_info->wr_pin.gpio_cfg, 1);
    p_drv_info->rd_pin.gpio_ops.gpio_output_set(&p_drv_info->rd_pin.gpio_cfg, 1);
    p_drv_info->db_pin.gpio_ops.gpio_output_set(&p_drv_info->db_pin.gpio_cfg, 0);

    return 0;
}

int st7789_send_byte(tftlcd_driver_t *p_driver, uint16_t data)
{
    st7789_8080_info_t *p_drv_info = (st7789_8080_info_t *)p_driver->p_tft_cfg;
    GPIO_TypeDef *port = (GPIO_TypeDef *)p_drv_info->db_pin.gpio_cfg.p_port;

    port->BSRR = (data & 0x00FF);
    port->BRR  = (~(data) & 0x00FF);

    return 0;
}

int st7789_write_cmd(tftlcd_driver_t *p_driver, uint16_t cmd)
{
    st7789_8080_info_t *p_drv_info = (st7789_8080_info_t *)p_driver->p_tft_cfg;

    p_drv_info->cs_pin.gpio_ops.gpio_output_set(&p_drv_info->cs_pin.gpio_cfg, 0);
    p_drv_info->rs_pin.gpio_ops.gpio_output_set(&p_drv_info->rs_pin.gpio_cfg, 0);   // rs = 0, send cmd
    p_drv_info->wr_pin.gpio_ops.gpio_output_set(&p_drv_info->wr_pin.gpio_cfg, 0);
    st7789_send_byte(p_driver, cmd);
    p_drv_info->wr_pin.gpio_ops.gpio_output_set(&p_drv_info->wr_pin.gpio_cfg, 1);
    p_drv_info->cs_pin.gpio_ops.gpio_output_set(&p_drv_info->cs_pin.gpio_cfg, 1);

    return 0;
}

int st7789_write_data(tftlcd_driver_t *p_driver, uint16_t data)
{
    st7789_8080_info_t *p_drv_info = (st7789_8080_info_t *)p_driver->p_tft_cfg;

    p_drv_info->cs_pin.gpio_ops.gpio_output_set(&p_drv_info->cs_pin.gpio_cfg, 0);
    p_drv_info->rs_pin.gpio_ops.gpio_output_set(&p_drv_info->rs_pin.gpio_cfg, 1);   // rs = 1, send data
    p_drv_info->wr_pin.gpio_ops.gpio_output_set(&p_drv_info->wr_pin.gpio_cfg, 0);
    st7789_send_byte(p_driver, data);
    p_drv_info->wr_pin.gpio_ops.gpio_output_set(&p_drv_info->wr_pin.gpio_cfg, 1);
    p_drv_info->cs_pin.gpio_ops.gpio_output_set(&p_drv_info->cs_pin.gpio_cfg, 1);

    return 0;
}

// st7789
int st7789_init(tftlcd_cfg_t *p_cfg)
{
    tftlcd_driver_t *p_dri = p_cfg->p_dri;

    // 1. hardware init
    p_cfg->hardware_init(p_dri);
    trace_debug("st7789 debug.\r\n");

    // 2. software init--> 发送命令的初始化
    p_cfg->write_cmd(p_dri, 0x36);      // 显示扫描方向
    p_cfg->write_data(p_dri, 0xA0);     // 00:从左到右，从上到下
                                        // A0:从下到上，从左到右
                                        // 60:从上到下，从右到左

    p_cfg->write_cmd(p_dri, 0x3A);    // color format
    p_cfg->write_data(p_dri, 0x05);    // 16bit/pixel

    p_cfg->write_cmd(p_dri, 0xB2);    // Porch Setting  
    p_cfg->write_data(p_dri, 0x0C);   
    p_cfg->write_data(p_dri, 0x0C);   
    p_cfg->write_data(p_dri, 0x00);   
    p_cfg->write_data(p_dri, 0x33);   
    p_cfg->write_data(p_dri, 0x33);   

    p_cfg->write_cmd(p_dri, 0xB7);    // Gate Control
    p_cfg->write_data(p_dri, 0x35);   

    p_cfg->write_cmd(p_dri, 0xBB);    // VCOM  
    p_cfg->write_data(p_dri, 0x1C);   

    p_cfg->write_cmd(p_dri, 0xC0);    // LCM control   
    p_cfg->write_data(p_dri, 0x2C);   

    p_cfg->write_cmd(p_dri, 0xC2);    // VDV and VRH Command Enable 
    p_cfg->write_data(p_dri, 0x01);   

    p_cfg->write_cmd(p_dri, 0xC3);    //  VRH Set  
    p_cfg->write_data(p_dri, 0x0B);       

    p_cfg->write_cmd(p_dri, 0xC4);    // VDV Set
    p_cfg->write_data(p_dri, 0x20);   

    p_cfg->write_cmd(p_dri, 0xC6);    // Frame Rate    
    p_cfg->write_data(p_dri, 0x0F);   

    p_cfg->write_cmd(p_dri, 0xD0);    // Power Control 1
    p_cfg->write_data(p_dri, 0xA4);   
    p_cfg->write_data(p_dri, 0xA1);   

    p_cfg->write_cmd(p_dri, 0xE0);    // Positive Voltage     
    p_cfg->write_data(p_dri, 0xD0);   
    p_cfg->write_data(p_dri, 0x06);   
    p_cfg->write_data(p_dri, 0x0B);   
    p_cfg->write_data(p_dri, 0x0A);   
    p_cfg->write_data(p_dri, 0x09);   
    p_cfg->write_data(p_dri, 0x06);   
    p_cfg->write_data(p_dri, 0x2F);   
    p_cfg->write_data(p_dri, 0x44);   
    p_cfg->write_data(p_dri, 0x45);   
    p_cfg->write_data(p_dri, 0x18);   
    p_cfg->write_data(p_dri, 0x14);   
    p_cfg->write_data(p_dri, 0x14);   
    p_cfg->write_data(p_dri, 0x27);   
    p_cfg->write_data(p_dri, 0x2D);   

    p_cfg->write_cmd(p_dri, 0xE1);    // Negative Voltage    
    p_cfg->write_data(p_dri, 0xD0);   
    p_cfg->write_data(p_dri, 0x06);   
    p_cfg->write_data(p_dri, 0x0B);   
    p_cfg->write_data(p_dri, 0x0A);   
    p_cfg->write_data(p_dri, 0x09);   
    p_cfg->write_data(p_dri, 0x05);   
    p_cfg->write_data(p_dri, 0x2E);   
    p_cfg->write_data(p_dri, 0x43);   
    p_cfg->write_data(p_dri, 0x45);   
    p_cfg->write_data(p_dri, 0x18);   
    p_cfg->write_data(p_dri, 0x14);   
    p_cfg->write_data(p_dri, 0x14);   
    p_cfg->write_data(p_dri, 0x27);   
    p_cfg->write_data(p_dri, 0x2D);   

    p_cfg->write_cmd(p_dri, 0x21);    // Display Inversion On
    p_cfg->write_cmd(p_dri, 0x11);    // Sleep Out
    p_cfg->delay_ms(5);
    p_cfg->write_cmd(p_dri, 0x29);    // Display On

    trace_debug("st7789 init end.\r\n");

    return 0;
}

int st7789_set_cursor(tftlcd_cfg_t *p_cfg, uint16_t xpos, uint16_t ypos)
{
    trace_debug("lcd set cursor.\r\n");
    tftlcd_driver_t *p_dri = p_cfg->p_dri;
    tftlcd_info_t   lcd_info = p_cfg->p_dri->lcd_info;

    p_cfg->write_cmd(p_dri, lcd_info.setxcmd);
    p_cfg->write_data(p_dri, (xpos >> 8));
    p_cfg->write_data(p_dri, (xpos & 0xFF));
    p_cfg->write_cmd(p_dri, lcd_info.setycmd);
    p_cfg->write_data(p_dri, (ypos >> 8));
    p_cfg->write_data(p_dri, (ypos & 0xFF));

    return 0;
}

int st7789_write_ram_pre(tftlcd_cfg_t *p_cfg)
{
    trace_debug("this is write ram prepare.\r\n");
    tftlcd_driver_t *p_dri = p_cfg->p_dri;
    tftlcd_info_t   lcd_info = p_cfg->p_dri->lcd_info;

    p_cfg->write_cmd(p_dri, lcd_info.wramcmd);

    return 0;
}

int st7789_write_ram(tftlcd_cfg_t *p_cfg, uint16_t color)
{
    // trace_debug("this is write ram.\r\n");
    tftlcd_driver_t *p_dri = p_cfg->p_dri;

    p_cfg->write_cmd(p_dri, (color >> 8));
    p_cfg->write_cmd(p_dri, (color & 0xff));

    return 0;
}

// 需要根据dir来设置扫描方向
int st7789_set_scan_dir(tftlcd_cfg_t *p_cfg, uint16_t dir)
{
    p_cfg->p_dri->lcd_info.dir = dir;

    return 0;
}

int st7789_clear_screen(tftlcd_cfg_t *p_cfg, struct tftlcd_ops *p_ops)
{
    trace_debug("lcd clear. color is %d\r\n", p_cfg->p_dri->lcd_info.color);
    p_ops->set_cursor(p_cfg, 0, 0);
    p_ops->write_ram_pre(p_cfg);

    trace_info("height = %d, width = %d\r\n", p_cfg->p_dri->lcd_info.height, p_cfg->p_dri->lcd_info.width);
    for (uint16_t y = 0; y < p_cfg->p_dri->lcd_info.height; y++)
    {
        for (uint16_t x = 0; x < p_cfg->p_dri->lcd_info.width; x++)
        {
            // p_ops->write_ram(p_cfg, p_cfg->p_dri->lcd_info.color);
            p_cfg->write_cmd(p_cfg->p_dri, (p_cfg->p_dri->lcd_info.color >> 8));
            p_cfg->write_cmd(p_cfg->p_dri, (p_cfg->p_dri->lcd_info.color & 0xff));
        }
    }

    trace_debug("lcd clear finish.\r\n");
    return 0;
}

// int st7789_fill_area(tftlcd_cfg_t *p_cfg)
// {

// }

