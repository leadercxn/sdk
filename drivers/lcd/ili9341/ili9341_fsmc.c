#include "ili9341_fsmc.h"

// basic function
int ili9341_hardware_init(tftlcd_driver_t *p_driver)
{
    ili9341_fsmc_info_t *p_drv_info = (ili9341_fsmc_info_t *)p_driver->p_tft_cfg;

    // 1. gpio config
    p_drv_info->rst_pin.gpio_ops.gpio_init(&p_drv_info->rst_pin.gpio_cfg);
    p_drv_info->bl_pin.gpio_ops.gpio_init(&p_drv_info->bl_pin.gpio_cfg);
    p_drv_info->fsmc_pin1.gpio_ops.gpio_init(&p_drv_info->fsmc_pin1.gpio_cfg);
    p_drv_info->fsmc_pin2.gpio_ops.gpio_init(&p_drv_info->fsmc_pin2.gpio_cfg);

    // 2. fsmc config
    fsmc_conig(0, p_drv_info->buswidth);

    // 3. handler GPIO
    p_drv_info->rst_pin.gpio_ops.gpio_output_set(&p_drv_info->rst_pin.gpio_cfg, 1);

    // 4. set lcd_addr
    if (p_drv_info->buswidth == LCDBUSWIDTH_8B)
    {
        p_drv_info->lcd_addr = ((lcdtype_8b_t *)(p_drv_info->base_addr));
    }
    else if (p_drv_info->buswidth == LCDBUSWIDTH_16B)
    {
        p_drv_info->lcd_addr = ((lcdtype_16b_t *)(p_drv_info->base_addr));
    }

    return 0;
}

int ili9341_write_cmd(tftlcd_driver_t *p_driver, uint16_t cmd)
{
    ili9341_fsmc_info_t *p_drv_info = (ili9341_fsmc_info_t *)p_driver->p_tft_cfg;

    if (p_drv_info->buswidth == LCDBUSWIDTH_8B)
    {
        lcdtype_8b_t *p_lcd_addr = (lcdtype_8b_t *)p_drv_info->lcd_addr;
        p_lcd_addr->lcd_cmd = (uint8_t)cmd;
    }
    else if (p_drv_info->buswidth == LCDBUSWIDTH_16B)
    {
        lcdtype_16b_t *p_lcd_addr = (lcdtype_16b_t *)p_drv_info->lcd_addr;
        p_lcd_addr->lcd_cmd = cmd;
    }

    return 0;
}

int ili9341_write_data(tftlcd_driver_t *p_driver, uint16_t data)
{
    ili9341_fsmc_info_t *p_drv_info = (ili9341_fsmc_info_t *)p_driver->p_tft_cfg;

    if (p_drv_info->buswidth == LCDBUSWIDTH_8B)
    {
        lcdtype_8b_t *p_lcd_addr = (lcdtype_8b_t *)p_drv_info->lcd_addr;
        p_lcd_addr->lcd_data = (uint8_t)data;
    }
    else if (p_drv_info->buswidth == LCDBUSWIDTH_16B)
    {
        lcdtype_16b_t *p_lcd_addr = (lcdtype_16b_t *)p_drv_info->lcd_addr;
        p_lcd_addr->lcd_data = data;
    }

    return 0;
}

int ili9341_read_data(tftlcd_driver_t *p_driver)
{
    ili9341_fsmc_info_t *p_drv_info = (ili9341_fsmc_info_t *)p_driver->p_tft_cfg;
    uint16_t data;

    if (p_drv_info->buswidth == LCDBUSWIDTH_8B)
    {
        lcdtype_8b_t *p_lcd_addr = (lcdtype_8b_t *)p_drv_info->lcd_addr;
        data = p_lcd_addr->lcd_data;
    }
    else if (p_drv_info->buswidth == LCDBUSWIDTH_16B)
    {
        lcdtype_16b_t *p_lcd_addr = (lcdtype_16b_t *)p_drv_info->lcd_addr;
        data = p_lcd_addr->lcd_data;
    }

    return data;
}

int ili9341_write_reg(tftlcd_driver_t *p_driver, uint16_t cmd, uint16_t data)
{
    ili9341_fsmc_info_t *p_drv_info = (ili9341_fsmc_info_t *)p_driver->p_tft_cfg;
    if (p_drv_info->buswidth == LCDBUSWIDTH_8B)
    {
        lcdtype_8b_t *p_lcd_addr = (lcdtype_8b_t *)p_drv_info->lcd_addr;
        p_lcd_addr->lcd_cmd = (uint8_t)cmd;
        p_lcd_addr->lcd_data = (uint8_t)data;
    }
    else if (p_drv_info->buswidth == LCDBUSWIDTH_16B)
    {
        lcdtype_16b_t *p_lcd_addr = (lcdtype_16b_t *)p_drv_info->lcd_addr;
        p_lcd_addr->lcd_cmd = (uint16_t)cmd;
        p_lcd_addr->lcd_data = data;
    }

    return 0;
}


// private function
int ili9341_read_reg(tftlcd_cfg_t *p_cfg, uint16_t cmd)
{
    ili9341_fsmc_info_t *p_drv_info = (ili9341_fsmc_info_t *)p_cfg->p_dri->p_tft_cfg;
    uint16_t data;

    if (p_drv_info->buswidth == LCDBUSWIDTH_8B)
    {
        lcdtype_8b_t *p_lcd_addr = (lcdtype_8b_t *)p_drv_info->lcd_addr;
        p_lcd_addr->lcd_cmd = (uint8_t)cmd;
        p_cfg->delay_us(5);
        data = p_lcd_addr->lcd_data;
    }
    else if (p_drv_info->buswidth == LCDBUSWIDTH_16B)
    {
        lcdtype_16b_t *p_lcd_addr = (lcdtype_16b_t *)p_drv_info->lcd_addr;
        p_lcd_addr->lcd_cmd = (uint16_t)cmd;
        p_cfg->delay_us(5);
        data = p_lcd_addr->lcd_data;
    }
    return data;
}

int ili9341_init(tftlcd_cfg_t *p_cfg, struct tftlcd_ops *p_ops)
{
    tftlcd_driver_t *p_dri = p_cfg->p_dri;
    ili9341_fsmc_info_t *p_drv_info = (ili9341_fsmc_info_t *)p_dri->p_tft_cfg;

    // 1. hardware init
    p_cfg->hardware_init(p_dri);
    p_cfg->delay_ms(5);                 // 该延时不能省略，以防驱动IC尚未复位完成就开始发送命令

    // 2. software init
    /* 2.1 read chip id */    
    // p_cfg->write_cmd(p_dri, 0xD3);
    // p_dri->lcd_info.id = p_cfg->read_data(p_dri);       // 0x00
    // p_dri->lcd_info.id = p_cfg->read_data(p_dri);       // 0x93
    // p_dri->lcd_info.id <<= 8;
    // p_dri->lcd_info.id = p_cfg->read_data(p_dri);       // 0x41
    // p_dri->lcd_info.id |= p_cfg->read_data(p_dri);

    // if (p_dri->lcd_info.id != 0x9341)
    // {
    //     return -1;
    // }

    /* 2.2 init sequence  */
    p_cfg->write_cmd(p_dri, 0xCF);
    p_cfg->write_data(p_dri, 0x00);
    p_cfg->write_data(p_dri, 0xC1);
    p_cfg->write_data(p_dri, 0x30);
    p_cfg->write_cmd(p_dri, 0xED);
    p_cfg->write_data(p_dri, 0x64);
    p_cfg->write_data(p_dri, 0x03);
    p_cfg->write_data(p_dri, 0x12);
    p_cfg->write_data(p_dri, 0x81);
    p_cfg->write_cmd(p_dri, 0xE8);
    p_cfg->write_data(p_dri, 0x85);
    p_cfg->write_data(p_dri, 0x10);
    p_cfg->write_data(p_dri, 0x7A);
    p_cfg->write_cmd(p_dri, 0xCB);
    p_cfg->write_data(p_dri, 0x39);
    p_cfg->write_data(p_dri, 0x2C);
    p_cfg->write_data(p_dri, 0x00);
    p_cfg->write_data(p_dri, 0x34);
    p_cfg->write_data(p_dri, 0x02);
    p_cfg->write_cmd(p_dri, 0xF7);
    p_cfg->write_data(p_dri, 0x20);
    p_cfg->write_cmd(p_dri, 0xEA);
    p_cfg->write_data(p_dri, 0x00);
    p_cfg->write_data(p_dri, 0x00);
    p_cfg->write_cmd(p_dri, 0xC0);       //Power control
    p_cfg->write_data(p_dri, 0x1B);      //VRH[5:0]
    p_cfg->write_cmd(p_dri, 0xC1);       //Power control
    p_cfg->write_data(p_dri, 0x01);      //SAP[2:0];BT[3:0]
    p_cfg->write_cmd(p_dri, 0xC5);       //VCM control
    p_cfg->write_data(p_dri, 0x30);      //3F
    p_cfg->write_data(p_dri, 0x30);      //3C
    p_cfg->write_cmd(p_dri, 0xC7);       //VCM control2
    p_cfg->write_data(p_dri, 0xB7);
    p_cfg->write_cmd(p_dri, 0x3A);
    p_cfg->write_data(p_dri, 0x55);
    p_cfg->write_cmd(p_dri, 0xB1);
    p_cfg->write_data(p_dri, 0x00);
    p_cfg->write_data(p_dri, 0x1A);
    p_cfg->write_cmd(p_dri, 0xB6);       // Display Function Control
    p_cfg->write_data(p_dri, 0x0A);
    p_cfg->write_data(p_dri, 0xA2);
    p_cfg->write_cmd(p_dri, 0xF2);       // 3Gamma Function Disable
    p_cfg->write_data(p_dri, 0x00);
    p_cfg->write_cmd(p_dri, 0x26);       //Gamma curve selected
    p_cfg->write_data(p_dri, 0x01);
    p_cfg->write_cmd(p_dri, 0xE0);       //Set Gamma
    p_cfg->write_data(p_dri, 0x0F);
    p_cfg->write_data(p_dri, 0x2A);
    p_cfg->write_data(p_dri, 0x28);
    p_cfg->write_data(p_dri, 0x08);
    p_cfg->write_data(p_dri, 0x0E);
    p_cfg->write_data(p_dri, 0x08);
    p_cfg->write_data(p_dri, 0x54);
    p_cfg->write_data(p_dri, 0xA9);
    p_cfg->write_data(p_dri, 0x43);
    p_cfg->write_data(p_dri, 0x0A);
    p_cfg->write_data(p_dri, 0x0F);
    p_cfg->write_data(p_dri, 0x00);
    p_cfg->write_data(p_dri, 0x00);
    p_cfg->write_data(p_dri, 0x00);
    p_cfg->write_data(p_dri, 0x00);
    p_cfg->write_cmd(p_dri, 0xE1);       //Set Gamma
    p_cfg->write_data(p_dri, 0x00);
    p_cfg->write_data(p_dri, 0x15);
    p_cfg->write_data(p_dri, 0x17);
    p_cfg->write_data(p_dri, 0x07);
    p_cfg->write_data(p_dri, 0x11);
    p_cfg->write_data(p_dri, 0x06);
    p_cfg->write_data(p_dri, 0x2B);
    p_cfg->write_data(p_dri, 0x56);
    p_cfg->write_data(p_dri, 0x3C);
    p_cfg->write_data(p_dri, 0x05);
    p_cfg->write_data(p_dri, 0x10);
    p_cfg->write_data(p_dri, 0x0F);
    p_cfg->write_data(p_dri, 0x3F);
    p_cfg->write_data(p_dri, 0x3F);
    p_cfg->write_data(p_dri, 0x0F);
    p_cfg->write_cmd(p_dri, 0x2B);
    p_cfg->write_data(p_dri, 0x00);
    p_cfg->write_data(p_dri, 0x00);
    p_cfg->write_data(p_dri, 0x01);
    p_cfg->write_data(p_dri, 0x3f);
    p_cfg->write_cmd(p_dri, 0x2A);
    p_cfg->write_data(p_dri, 0x00);
    p_cfg->write_data(p_dri, 0x00);
    p_cfg->write_data(p_dri, 0x00);
    p_cfg->write_data(p_dri, 0xef);
    p_cfg->write_cmd(p_dri, 0x11);       //Exit Sleep
    p_cfg->delay_ms(120);
    p_cfg->write_cmd(p_dri, 0x29);       //display on

    // 2.3 设置扫描方向、清屏
    p_ops->set_scan_dir(p_cfg, p_dri->lcd_info.dir);
    p_ops->clear_screen(p_cfg, p_ops);

    // 3. 最后亮背光
    p_drv_info->bl_pin.gpio_ops.gpio_output_set(&p_drv_info->bl_pin.gpio_cfg, 1);

    return 0;
}

int ili9341_set_cursor(tftlcd_cfg_t *p_cfg, uint16_t xpos, uint16_t ypos)
{
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

int ili9341_set_scan_dir(tftlcd_cfg_t *p_cfg, uint16_t dir)
{
    tftlcd_driver_t *p_dri = p_cfg->p_dri;
    tftlcd_info_t   lcd_info = p_cfg->p_dri->lcd_info;

    p_cfg->write_reg(p_dri, 0x36, 0x08);        // 设置扫描方向

    p_cfg->p_dri->lcd_info.dir = dir;
    if (dir == 1)           // 横屏时需要对调
    {
        uint16_t tmp = p_cfg->p_dri->lcd_info.width;
        p_cfg->p_dri->lcd_info.width = p_cfg->p_dri->lcd_info.height;
        p_cfg->p_dri->lcd_info.height = tmp;
    }

    p_cfg->write_cmd(p_dri, lcd_info.setxcmd);
    p_cfg->write_data(p_dri, 0);
    p_cfg->write_data(p_dri, 0);
    p_cfg->write_data(p_dri, ((lcd_info.width - 1) >> 8));
    p_cfg->write_data(p_dri, ((lcd_info.width - 1) & 0xff));

    p_cfg->write_cmd(p_dri, lcd_info.setycmd);
    p_cfg->write_data(p_dri, 0);
    p_cfg->write_data(p_dri, 0);
    p_cfg->write_data(p_dri, ((lcd_info.height - 1) >> 8));
    p_cfg->write_data(p_dri, ((lcd_info.height - 1) & 0xff));

    return 0;
}

int ili9341_write_ram_pre(tftlcd_cfg_t *p_cfg)
{
    tftlcd_driver_t *p_dri = p_cfg->p_dri;
    tftlcd_info_t   lcd_info = p_cfg->p_dri->lcd_info;

    p_cfg->write_cmd(p_dri, lcd_info.wramcmd);

    return 0;
}

int ili9341_write_ram(tftlcd_cfg_t *p_cfg, uint16_t color)
{
    tftlcd_driver_t *p_dri = p_cfg->p_dri;
    ili9341_fsmc_info_t *p_drv_info = (ili9341_fsmc_info_t *)p_cfg->p_dri->p_tft_cfg;

    if (p_drv_info->buswidth == LCDBUSWIDTH_8B)
    {
        p_cfg->write_data(p_dri, (color >> 8));
        p_cfg->write_data(p_dri, (color & 0xff));
    }
    else if (p_drv_info->buswidth == LCDBUSWIDTH_16B)
    {
        p_cfg->write_data(p_dri, color);
    }

    return 0;
}