#include "st7789_fsmc.h"

// basic function
int st7789_hw_fsmc_init(tftlcd_driver_t *p_driver)
{
    st7789_fsmc_info_t *p_drv_info = (st7789_fsmc_info_t *)p_driver->p_tft_cfg;

    // 1. gpio config
    p_drv_info->rst_pin.gpio_ops.gpio_init(&p_drv_info->rst_pin.gpio_cfg);
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

int st7789_write_cmd_fsmc(tftlcd_driver_t *p_driver, uint16_t cmd)
{
    st7789_fsmc_info_t *p_drv_info = (st7789_fsmc_info_t *)p_driver->p_tft_cfg;

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

int st7789_write_data_fsmc(tftlcd_driver_t *p_driver, uint16_t data)
{
    st7789_fsmc_info_t *p_drv_info = (st7789_fsmc_info_t *)p_driver->p_tft_cfg;

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

int st7789_read_data_fsmc(tftlcd_driver_t *p_driver)
{
    st7789_fsmc_info_t *p_drv_info = (st7789_fsmc_info_t *)p_driver->p_tft_cfg;
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

int st7789_write_reg_fsmc(tftlcd_driver_t *p_driver, uint16_t cmd, uint16_t data)
{
    st7789_fsmc_info_t *p_drv_info = (st7789_fsmc_info_t *)p_driver->p_tft_cfg;

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

int st7789_read_reg(tftlcd_cfg_t *p_cfg, uint16_t cmd)
{
    st7789_fsmc_info_t *p_drv_info = (st7789_fsmc_info_t *)p_cfg->p_dri->p_tft_cfg;
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


