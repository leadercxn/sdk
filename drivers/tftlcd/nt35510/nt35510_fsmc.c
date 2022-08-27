#include "nt35510_fsmc.h"
#include "fsmc.h"
#include "lib_error.h"
#include "trace.h"

int nt35510_hardware_init(tftlcd_driver_t *p_driver)
{
    nt35510_fsmc_info_t *p_drv_info = (nt35510_fsmc_info_t *)p_driver->p_tft_cfg;

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
    // p_drv_info->base_addr = ((uint32_t)((FSMC_BASE_ADDR + BANK1_SECTOR1_OFFSET) | BANK_16B_A16_OFFSET));
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

int nt35510_write_cmd(tftlcd_driver_t *p_driver, uint16_t cmd)
{
    nt35510_fsmc_info_t *p_drv_info = (nt35510_fsmc_info_t *)p_driver->p_tft_cfg;

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

int nt35510_write_data(tftlcd_driver_t *p_driver, uint16_t data)
{
    nt35510_fsmc_info_t *p_drv_info = (nt35510_fsmc_info_t *)p_driver->p_tft_cfg;

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

int nt35510_read_data(tftlcd_driver_t *p_driver)
{
    nt35510_fsmc_info_t *p_drv_info = (nt35510_fsmc_info_t *)p_driver->p_tft_cfg;
    uint16_t data = 0;

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

int nt35510_write_reg(tftlcd_driver_t *p_driver, uint16_t cmd, uint16_t data)
{
    nt35510_fsmc_info_t *p_drv_info = (nt35510_fsmc_info_t *)p_driver->p_tft_cfg;
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

int nt35510_read_reg(tftlcd_cfg_t *p_cfg, uint16_t cmd)
{
    nt35510_fsmc_info_t *p_drv_info = (nt35510_fsmc_info_t *)p_cfg->p_dri->p_tft_cfg;
    uint16_t data = 0;

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

int nt35510_init(tftlcd_cfg_t *p_cfg, struct tftlcd_ops *p_ops)
{
    tftlcd_driver_t *p_dri = p_cfg->p_dri;
    nt35510_fsmc_info_t *p_drv_info = (nt35510_fsmc_info_t *)p_dri->p_tft_cfg;

    // 1. hardware init
    p_cfg->hardware_init(p_dri);
    p_cfg->delay_ms(5);                 // 该延时不能省略，以防驱动IC尚未复位完成就开始发送命令

    // 2. software init
    /* 2.1 read chip id */
    p_cfg->write_cmd(p_dri, 0XDA00);
    p_dri->lcd_info.id = p_cfg->read_data(p_dri);     //读回0X00	 
    p_cfg->write_cmd(p_dri, 0XDB00);
    p_dri->lcd_info.id = p_cfg->read_data(p_dri);     //读回0X80
    p_dri->lcd_info.id <<= 8;
    p_cfg->write_cmd(p_dri, 0XDC00);
    p_dri->lcd_info.id |= p_cfg->read_data(p_dri);    //读回0X00

    if (p_dri->lcd_info.id == 0x8000)
    {
        p_dri->lcd_info.id = 0x5510;
    }
    else
    {
        return -EINVAL;
    }

    /* 2.2 init sequence  */
    // 5.2 init sequence
    p_cfg->write_reg(p_dri, 0xF000, 0x55);
    p_cfg->write_reg(p_dri, 0xF001, 0xAA);
    p_cfg->write_reg(p_dri, 0xF002, 0x52);
    p_cfg->write_reg(p_dri, 0xF003, 0x08);
    p_cfg->write_reg(p_dri, 0xF004, 0x01);
    //AVDD Set AVDD 5.2V
    p_cfg->write_reg(p_dri, 0xB000, 0x0D);
    p_cfg->write_reg(p_dri, 0xB001, 0x0D);
    p_cfg->write_reg(p_dri, 0xB002, 0x0D);
    //AVDD ratio
    p_cfg->write_reg(p_dri, 0xB600, 0x34);
    p_cfg->write_reg(p_dri, 0xB601, 0x34);
    p_cfg->write_reg(p_dri, 0xB602, 0x34);
    //AVEE -5.2V
    p_cfg->write_reg(p_dri, 0xB100, 0x0D);
    p_cfg->write_reg(p_dri, 0xB101, 0x0D);
    p_cfg->write_reg(p_dri, 0xB102, 0x0D);
    //AVEE ratio
    p_cfg->write_reg(p_dri, 0xB700, 0x34);
    p_cfg->write_reg(p_dri, 0xB701, 0x34);
    p_cfg->write_reg(p_dri, 0xB702, 0x34);
    //VCL -2.5V
    p_cfg->write_reg(p_dri, 0xB200, 0x00);
    p_cfg->write_reg(p_dri, 0xB201, 0x00);
    p_cfg->write_reg(p_dri, 0xB202, 0x00);
    //VCL ratio
    p_cfg->write_reg(p_dri, 0xB800, 0x24);
    p_cfg->write_reg(p_dri, 0xB801, 0x24);
    p_cfg->write_reg(p_dri, 0xB802, 0x24);
    //VGH 15V (Free pump)
    p_cfg->write_reg(p_dri, 0xBF00, 0x01);
    p_cfg->write_reg(p_dri, 0xB300, 0x0F);
    p_cfg->write_reg(p_dri, 0xB301, 0x0F);
    p_cfg->write_reg(p_dri, 0xB302, 0x0F);
    //VGH ratio
    p_cfg->write_reg(p_dri, 0xB900, 0x34);
    p_cfg->write_reg(p_dri, 0xB901, 0x34);
    p_cfg->write_reg(p_dri, 0xB902, 0x34);
    //VGL_REG -10V
    p_cfg->write_reg(p_dri, 0xB500, 0x08);
    p_cfg->write_reg(p_dri, 0xB501, 0x08);
    p_cfg->write_reg(p_dri, 0xB502, 0x08);
    p_cfg->write_reg(p_dri, 0xC200, 0x03);
    //VGLX ratio
    p_cfg->write_reg(p_dri, 0xBA00, 0x24);
    p_cfg->write_reg(p_dri, 0xBA01, 0x24);
    p_cfg->write_reg(p_dri, 0xBA02, 0x24);
    //VGMP/VGSP 4.5V/0V
    p_cfg->write_reg(p_dri, 0xBC00, 0x00);
    p_cfg->write_reg(p_dri, 0xBC01, 0x78);
    p_cfg->write_reg(p_dri, 0xBC02, 0x00);
    //VGMN/VGSN -4.5V/0V
    p_cfg->write_reg(p_dri, 0xBD00, 0x00);
    p_cfg->write_reg(p_dri, 0xBD01, 0x78);
    p_cfg->write_reg(p_dri, 0xBD02, 0x00);
    //VCOM
    p_cfg->write_reg(p_dri, 0xBE00, 0x00);
    p_cfg->write_reg(p_dri, 0xBE01, 0x64);
    //Gamma Setting
    p_cfg->write_reg(p_dri, 0xD100, 0x00);
    p_cfg->write_reg(p_dri, 0xD101, 0x33);
    p_cfg->write_reg(p_dri, 0xD102, 0x00);
    p_cfg->write_reg(p_dri, 0xD103, 0x34);
    p_cfg->write_reg(p_dri, 0xD104, 0x00);
    p_cfg->write_reg(p_dri, 0xD105, 0x3A);
    p_cfg->write_reg(p_dri, 0xD106, 0x00);
    p_cfg->write_reg(p_dri, 0xD107, 0x4A);
    p_cfg->write_reg(p_dri, 0xD108, 0x00);
    p_cfg->write_reg(p_dri, 0xD109, 0x5C);
    p_cfg->write_reg(p_dri, 0xD10A, 0x00);
    p_cfg->write_reg(p_dri, 0xD10B, 0x81);
    p_cfg->write_reg(p_dri, 0xD10C, 0x00);
    p_cfg->write_reg(p_dri, 0xD10D, 0xA6);
    p_cfg->write_reg(p_dri, 0xD10E, 0x00);
    p_cfg->write_reg(p_dri, 0xD10F, 0xE5);
    p_cfg->write_reg(p_dri, 0xD110, 0x01);
    p_cfg->write_reg(p_dri, 0xD111, 0x13);
    p_cfg->write_reg(p_dri, 0xD112, 0x01);
    p_cfg->write_reg(p_dri, 0xD113, 0x54);
    p_cfg->write_reg(p_dri, 0xD114, 0x01);
    p_cfg->write_reg(p_dri, 0xD115, 0x82);
    p_cfg->write_reg(p_dri, 0xD116, 0x01);
    p_cfg->write_reg(p_dri, 0xD117, 0xCA);
    p_cfg->write_reg(p_dri, 0xD118, 0x02);
    p_cfg->write_reg(p_dri, 0xD119, 0x00);
    p_cfg->write_reg(p_dri, 0xD11A, 0x02);
    p_cfg->write_reg(p_dri, 0xD11B, 0x01);
    p_cfg->write_reg(p_dri, 0xD11C, 0x02);
    p_cfg->write_reg(p_dri, 0xD11D, 0x34);
    p_cfg->write_reg(p_dri, 0xD11E, 0x02);
    p_cfg->write_reg(p_dri, 0xD11F, 0x67);
    p_cfg->write_reg(p_dri, 0xD120, 0x02);
    p_cfg->write_reg(p_dri, 0xD121, 0x84);
    p_cfg->write_reg(p_dri, 0xD122, 0x02);
    p_cfg->write_reg(p_dri, 0xD123, 0xA4);
    p_cfg->write_reg(p_dri, 0xD124, 0x02);
    p_cfg->write_reg(p_dri, 0xD125, 0xB7);
    p_cfg->write_reg(p_dri, 0xD126, 0x02);
    p_cfg->write_reg(p_dri, 0xD127, 0xCF);
    p_cfg->write_reg(p_dri, 0xD128, 0x02);
    p_cfg->write_reg(p_dri, 0xD129, 0xDE);
    p_cfg->write_reg(p_dri, 0xD12A, 0x02);
    p_cfg->write_reg(p_dri, 0xD12B, 0xF2);
    p_cfg->write_reg(p_dri, 0xD12C, 0x02);
    p_cfg->write_reg(p_dri, 0xD12D, 0xFE);
    p_cfg->write_reg(p_dri, 0xD12E, 0x03);
    p_cfg->write_reg(p_dri, 0xD12F, 0x10);
    p_cfg->write_reg(p_dri, 0xD130, 0x03);
    p_cfg->write_reg(p_dri, 0xD131, 0x33);
    p_cfg->write_reg(p_dri, 0xD132, 0x03);
    p_cfg->write_reg(p_dri, 0xD133, 0x6D);
    p_cfg->write_reg(p_dri, 0xD200, 0x00);
    p_cfg->write_reg(p_dri, 0xD201, 0x33);
    p_cfg->write_reg(p_dri, 0xD202, 0x00);
    p_cfg->write_reg(p_dri, 0xD203, 0x34);
    p_cfg->write_reg(p_dri, 0xD204, 0x00);
    p_cfg->write_reg(p_dri, 0xD205, 0x3A);
    p_cfg->write_reg(p_dri, 0xD206, 0x00);
    p_cfg->write_reg(p_dri, 0xD207, 0x4A);
    p_cfg->write_reg(p_dri, 0xD208, 0x00);
    p_cfg->write_reg(p_dri, 0xD209, 0x5C);
    p_cfg->write_reg(p_dri, 0xD20A, 0x00);

    p_cfg->write_reg(p_dri, 0xD20B, 0x81);
    p_cfg->write_reg(p_dri, 0xD20C, 0x00);
    p_cfg->write_reg(p_dri, 0xD20D, 0xA6);
    p_cfg->write_reg(p_dri, 0xD20E, 0x00);
    p_cfg->write_reg(p_dri, 0xD20F, 0xE5);
    p_cfg->write_reg(p_dri, 0xD210, 0x01);
    p_cfg->write_reg(p_dri, 0xD211, 0x13);
    p_cfg->write_reg(p_dri, 0xD212, 0x01);
    p_cfg->write_reg(p_dri, 0xD213, 0x54);
    p_cfg->write_reg(p_dri, 0xD214, 0x01);
    p_cfg->write_reg(p_dri, 0xD215, 0x82);
    p_cfg->write_reg(p_dri, 0xD216, 0x01);
    p_cfg->write_reg(p_dri, 0xD217, 0xCA);
    p_cfg->write_reg(p_dri, 0xD218, 0x02);
    p_cfg->write_reg(p_dri, 0xD219, 0x00);
    p_cfg->write_reg(p_dri, 0xD21A, 0x02);
    p_cfg->write_reg(p_dri, 0xD21B, 0x01);
    p_cfg->write_reg(p_dri, 0xD21C, 0x02);
    p_cfg->write_reg(p_dri, 0xD21D, 0x34);
    p_cfg->write_reg(p_dri, 0xD21E, 0x02);
    p_cfg->write_reg(p_dri, 0xD21F, 0x67);
    p_cfg->write_reg(p_dri, 0xD220, 0x02);
    p_cfg->write_reg(p_dri, 0xD221, 0x84);
    p_cfg->write_reg(p_dri, 0xD222, 0x02);
    p_cfg->write_reg(p_dri, 0xD223, 0xA4);
    p_cfg->write_reg(p_dri, 0xD224, 0x02);
    p_cfg->write_reg(p_dri, 0xD225, 0xB7);
    p_cfg->write_reg(p_dri, 0xD226, 0x02);
    p_cfg->write_reg(p_dri, 0xD227, 0xCF);
    p_cfg->write_reg(p_dri, 0xD228, 0x02);
    p_cfg->write_reg(p_dri, 0xD229, 0xDE);
    p_cfg->write_reg(p_dri, 0xD22A, 0x02);
    p_cfg->write_reg(p_dri, 0xD22B, 0xF2);
    p_cfg->write_reg(p_dri, 0xD22C, 0x02);
    p_cfg->write_reg(p_dri, 0xD22D, 0xFE);
    p_cfg->write_reg(p_dri, 0xD22E, 0x03);
    p_cfg->write_reg(p_dri, 0xD22F, 0x10);
    p_cfg->write_reg(p_dri, 0xD230, 0x03);
    p_cfg->write_reg(p_dri, 0xD231, 0x33);
    p_cfg->write_reg(p_dri, 0xD232, 0x03);
    p_cfg->write_reg(p_dri, 0xD233, 0x6D);
    p_cfg->write_reg(p_dri, 0xD300, 0x00);
    p_cfg->write_reg(p_dri, 0xD301, 0x33);
    p_cfg->write_reg(p_dri, 0xD302, 0x00);
    p_cfg->write_reg(p_dri, 0xD303, 0x34);
    p_cfg->write_reg(p_dri, 0xD304, 0x00);
    p_cfg->write_reg(p_dri, 0xD305, 0x3A);
    p_cfg->write_reg(p_dri, 0xD306, 0x00);
    p_cfg->write_reg(p_dri, 0xD307, 0x4A);
    p_cfg->write_reg(p_dri, 0xD308, 0x00);
    p_cfg->write_reg(p_dri, 0xD309, 0x5C);
    p_cfg->write_reg(p_dri, 0xD30A, 0x00);

    p_cfg->write_reg(p_dri, 0xD30B, 0x81);
    p_cfg->write_reg(p_dri, 0xD30C, 0x00);
    p_cfg->write_reg(p_dri, 0xD30D, 0xA6);
    p_cfg->write_reg(p_dri, 0xD30E, 0x00);
    p_cfg->write_reg(p_dri, 0xD30F, 0xE5);
    p_cfg->write_reg(p_dri, 0xD310, 0x01);
    p_cfg->write_reg(p_dri, 0xD311, 0x13);
    p_cfg->write_reg(p_dri, 0xD312, 0x01);
    p_cfg->write_reg(p_dri, 0xD313, 0x54);
    p_cfg->write_reg(p_dri, 0xD314, 0x01);
    p_cfg->write_reg(p_dri, 0xD315, 0x82);
    p_cfg->write_reg(p_dri, 0xD316, 0x01);
    p_cfg->write_reg(p_dri, 0xD317, 0xCA);
    p_cfg->write_reg(p_dri, 0xD318, 0x02);
    p_cfg->write_reg(p_dri, 0xD319, 0x00);
    p_cfg->write_reg(p_dri, 0xD31A, 0x02);
    p_cfg->write_reg(p_dri, 0xD31B, 0x01);
    p_cfg->write_reg(p_dri, 0xD31C, 0x02);
    p_cfg->write_reg(p_dri, 0xD31D, 0x34);
    p_cfg->write_reg(p_dri, 0xD31E, 0x02);
    p_cfg->write_reg(p_dri, 0xD31F, 0x67);
    p_cfg->write_reg(p_dri, 0xD320, 0x02);
    p_cfg->write_reg(p_dri, 0xD321, 0x84);
    p_cfg->write_reg(p_dri, 0xD322, 0x02);
    p_cfg->write_reg(p_dri, 0xD323, 0xA4);
    p_cfg->write_reg(p_dri, 0xD324, 0x02);
    p_cfg->write_reg(p_dri, 0xD325, 0xB7);
    p_cfg->write_reg(p_dri, 0xD326, 0x02);
    p_cfg->write_reg(p_dri, 0xD327, 0xCF);
    p_cfg->write_reg(p_dri, 0xD328, 0x02);
    p_cfg->write_reg(p_dri, 0xD329, 0xDE);
    p_cfg->write_reg(p_dri, 0xD32A, 0x02);
    p_cfg->write_reg(p_dri, 0xD32B, 0xF2);
    p_cfg->write_reg(p_dri, 0xD32C, 0x02);
    p_cfg->write_reg(p_dri, 0xD32D, 0xFE);
    p_cfg->write_reg(p_dri, 0xD32E, 0x03);
    p_cfg->write_reg(p_dri, 0xD32F, 0x10);
    p_cfg->write_reg(p_dri, 0xD330, 0x03);
    p_cfg->write_reg(p_dri, 0xD331, 0x33);
    p_cfg->write_reg(p_dri, 0xD332, 0x03);
    p_cfg->write_reg(p_dri, 0xD333, 0x6D);
    p_cfg->write_reg(p_dri, 0xD400, 0x00);
    p_cfg->write_reg(p_dri, 0xD401, 0x33);
    p_cfg->write_reg(p_dri, 0xD402, 0x00);
    p_cfg->write_reg(p_dri, 0xD403, 0x34);
    p_cfg->write_reg(p_dri, 0xD404, 0x00);
    p_cfg->write_reg(p_dri, 0xD405, 0x3A);
    p_cfg->write_reg(p_dri, 0xD406, 0x00);
    p_cfg->write_reg(p_dri, 0xD407, 0x4A);
    p_cfg->write_reg(p_dri, 0xD408, 0x00);
    p_cfg->write_reg(p_dri, 0xD409, 0x5C);
    p_cfg->write_reg(p_dri, 0xD40A, 0x00);
    p_cfg->write_reg(p_dri, 0xD40B, 0x81);

    p_cfg->write_reg(p_dri, 0xD40C, 0x00);
    p_cfg->write_reg(p_dri, 0xD40D, 0xA6);
    p_cfg->write_reg(p_dri, 0xD40E, 0x00);
    p_cfg->write_reg(p_dri, 0xD40F, 0xE5);
    p_cfg->write_reg(p_dri, 0xD410, 0x01);
    p_cfg->write_reg(p_dri, 0xD411, 0x13);
    p_cfg->write_reg(p_dri, 0xD412, 0x01);
    p_cfg->write_reg(p_dri, 0xD413, 0x54);
    p_cfg->write_reg(p_dri, 0xD414, 0x01);
    p_cfg->write_reg(p_dri, 0xD415, 0x82);
    p_cfg->write_reg(p_dri, 0xD416, 0x01);
    p_cfg->write_reg(p_dri, 0xD417, 0xCA);
    p_cfg->write_reg(p_dri, 0xD418, 0x02);
    p_cfg->write_reg(p_dri, 0xD419, 0x00);
    p_cfg->write_reg(p_dri, 0xD41A, 0x02);
    p_cfg->write_reg(p_dri, 0xD41B, 0x01);
    p_cfg->write_reg(p_dri, 0xD41C, 0x02);
    p_cfg->write_reg(p_dri, 0xD41D, 0x34);
    p_cfg->write_reg(p_dri, 0xD41E, 0x02);
    p_cfg->write_reg(p_dri, 0xD41F, 0x67);
    p_cfg->write_reg(p_dri, 0xD420, 0x02);
    p_cfg->write_reg(p_dri, 0xD421, 0x84);
    p_cfg->write_reg(p_dri, 0xD422, 0x02);
    p_cfg->write_reg(p_dri, 0xD423, 0xA4);
    p_cfg->write_reg(p_dri, 0xD424, 0x02);
    p_cfg->write_reg(p_dri, 0xD425, 0xB7);
    p_cfg->write_reg(p_dri, 0xD426, 0x02);
    p_cfg->write_reg(p_dri, 0xD427, 0xCF);
    p_cfg->write_reg(p_dri, 0xD428, 0x02);
    p_cfg->write_reg(p_dri, 0xD429, 0xDE);
    p_cfg->write_reg(p_dri, 0xD42A, 0x02);
    p_cfg->write_reg(p_dri, 0xD42B, 0xF2);
    p_cfg->write_reg(p_dri, 0xD42C, 0x02);
    p_cfg->write_reg(p_dri, 0xD42D, 0xFE);
    p_cfg->write_reg(p_dri, 0xD42E, 0x03);
    p_cfg->write_reg(p_dri, 0xD42F, 0x10);
    p_cfg->write_reg(p_dri, 0xD430, 0x03);
    p_cfg->write_reg(p_dri, 0xD431, 0x33);
    p_cfg->write_reg(p_dri, 0xD432, 0x03);
    p_cfg->write_reg(p_dri, 0xD433, 0x6D);
    p_cfg->write_reg(p_dri, 0xD500, 0x00);
    p_cfg->write_reg(p_dri, 0xD501, 0x33);
    p_cfg->write_reg(p_dri, 0xD502, 0x00);
    p_cfg->write_reg(p_dri, 0xD503, 0x34);
    p_cfg->write_reg(p_dri, 0xD504, 0x00);
    p_cfg->write_reg(p_dri, 0xD505, 0x3A);
    p_cfg->write_reg(p_dri, 0xD506, 0x00);
    p_cfg->write_reg(p_dri, 0xD507, 0x4A);
    p_cfg->write_reg(p_dri, 0xD508, 0x00);
    p_cfg->write_reg(p_dri, 0xD509, 0x5C);
    p_cfg->write_reg(p_dri, 0xD50A, 0x00);
    p_cfg->write_reg(p_dri, 0xD50B, 0x81);

    p_cfg->write_reg(p_dri, 0xD50C, 0x00);
    p_cfg->write_reg(p_dri, 0xD50D, 0xA6);
    p_cfg->write_reg(p_dri, 0xD50E, 0x00);
    p_cfg->write_reg(p_dri, 0xD50F, 0xE5);
    p_cfg->write_reg(p_dri, 0xD510, 0x01);
    p_cfg->write_reg(p_dri, 0xD511, 0x13);
    p_cfg->write_reg(p_dri, 0xD512, 0x01);
    p_cfg->write_reg(p_dri, 0xD513, 0x54);
    p_cfg->write_reg(p_dri, 0xD514, 0x01);
    p_cfg->write_reg(p_dri, 0xD515, 0x82);
    p_cfg->write_reg(p_dri, 0xD516, 0x01);
    p_cfg->write_reg(p_dri, 0xD517, 0xCA);
    p_cfg->write_reg(p_dri, 0xD518, 0x02);
    p_cfg->write_reg(p_dri, 0xD519, 0x00);
    p_cfg->write_reg(p_dri, 0xD51A, 0x02);
    p_cfg->write_reg(p_dri, 0xD51B, 0x01);
    p_cfg->write_reg(p_dri, 0xD51C, 0x02);
    p_cfg->write_reg(p_dri, 0xD51D, 0x34);
    p_cfg->write_reg(p_dri, 0xD51E, 0x02);
    p_cfg->write_reg(p_dri, 0xD51F, 0x67);
    p_cfg->write_reg(p_dri, 0xD520, 0x02);
    p_cfg->write_reg(p_dri, 0xD521, 0x84);
    p_cfg->write_reg(p_dri, 0xD522, 0x02);
    p_cfg->write_reg(p_dri, 0xD523, 0xA4);
    p_cfg->write_reg(p_dri, 0xD524, 0x02);
    p_cfg->write_reg(p_dri, 0xD525, 0xB7);
    p_cfg->write_reg(p_dri, 0xD526, 0x02);
    p_cfg->write_reg(p_dri, 0xD527, 0xCF);
    p_cfg->write_reg(p_dri, 0xD528, 0x02);
    p_cfg->write_reg(p_dri, 0xD529, 0xDE);
    p_cfg->write_reg(p_dri, 0xD52A, 0x02);
    p_cfg->write_reg(p_dri, 0xD52B, 0xF2);
    p_cfg->write_reg(p_dri, 0xD52C, 0x02);
    p_cfg->write_reg(p_dri, 0xD52D, 0xFE);
    p_cfg->write_reg(p_dri, 0xD52E, 0x03);
    p_cfg->write_reg(p_dri, 0xD52F, 0x10);
    p_cfg->write_reg(p_dri, 0xD530, 0x03);
    p_cfg->write_reg(p_dri, 0xD531, 0x33);
    p_cfg->write_reg(p_dri, 0xD532, 0x03);
    p_cfg->write_reg(p_dri, 0xD533, 0x6D);
    p_cfg->write_reg(p_dri, 0xD600, 0x00);
    p_cfg->write_reg(p_dri, 0xD601, 0x33);
    p_cfg->write_reg(p_dri, 0xD602, 0x00);
    p_cfg->write_reg(p_dri, 0xD603, 0x34);
    p_cfg->write_reg(p_dri, 0xD604, 0x00);
    p_cfg->write_reg(p_dri, 0xD605, 0x3A);
    p_cfg->write_reg(p_dri, 0xD606, 0x00);
    p_cfg->write_reg(p_dri, 0xD607, 0x4A);
    p_cfg->write_reg(p_dri, 0xD608, 0x00);
    p_cfg->write_reg(p_dri, 0xD609, 0x5C);
    p_cfg->write_reg(p_dri, 0xD60A, 0x00);
    p_cfg->write_reg(p_dri, 0xD60B, 0x81);

    p_cfg->write_reg(p_dri, 0xD60C, 0x00);
    p_cfg->write_reg(p_dri, 0xD60D, 0xA6);
    p_cfg->write_reg(p_dri, 0xD60E, 0x00);
    p_cfg->write_reg(p_dri, 0xD60F, 0xE5);
    p_cfg->write_reg(p_dri, 0xD610, 0x01);
    p_cfg->write_reg(p_dri, 0xD611, 0x13);
    p_cfg->write_reg(p_dri, 0xD612, 0x01);
    p_cfg->write_reg(p_dri, 0xD613, 0x54);
    p_cfg->write_reg(p_dri, 0xD614, 0x01);
    p_cfg->write_reg(p_dri, 0xD615, 0x82);
    p_cfg->write_reg(p_dri, 0xD616, 0x01);
    p_cfg->write_reg(p_dri, 0xD617, 0xCA);
    p_cfg->write_reg(p_dri, 0xD618, 0x02);
    p_cfg->write_reg(p_dri, 0xD619, 0x00);
    p_cfg->write_reg(p_dri, 0xD61A, 0x02);
    p_cfg->write_reg(p_dri, 0xD61B, 0x01);
    p_cfg->write_reg(p_dri, 0xD61C, 0x02);
    p_cfg->write_reg(p_dri, 0xD61D, 0x34);
    p_cfg->write_reg(p_dri, 0xD61E, 0x02);
    p_cfg->write_reg(p_dri, 0xD61F, 0x67);
    p_cfg->write_reg(p_dri, 0xD620, 0x02);
    p_cfg->write_reg(p_dri, 0xD621, 0x84);
    p_cfg->write_reg(p_dri, 0xD622, 0x02);
    p_cfg->write_reg(p_dri, 0xD623, 0xA4);
    p_cfg->write_reg(p_dri, 0xD624, 0x02);
    p_cfg->write_reg(p_dri, 0xD625, 0xB7);
    p_cfg->write_reg(p_dri, 0xD626, 0x02);
    p_cfg->write_reg(p_dri, 0xD627, 0xCF);
    p_cfg->write_reg(p_dri, 0xD628, 0x02);
    p_cfg->write_reg(p_dri, 0xD629, 0xDE);
    p_cfg->write_reg(p_dri, 0xD62A, 0x02);
    p_cfg->write_reg(p_dri, 0xD62B, 0xF2);
    p_cfg->write_reg(p_dri, 0xD62C, 0x02);
    p_cfg->write_reg(p_dri, 0xD62D, 0xFE);
    p_cfg->write_reg(p_dri, 0xD62E, 0x03);
    p_cfg->write_reg(p_dri, 0xD62F, 0x10);
    p_cfg->write_reg(p_dri, 0xD630, 0x03);
    p_cfg->write_reg(p_dri, 0xD631, 0x33);
    p_cfg->write_reg(p_dri, 0xD632, 0x03);
    p_cfg->write_reg(p_dri, 0xD633, 0x6D);
    //LV2 Page 0 enable
    p_cfg->write_reg(p_dri, 0xF000, 0x55);
    p_cfg->write_reg(p_dri, 0xF001, 0xAA);
    p_cfg->write_reg(p_dri, 0xF002, 0x52);
    p_cfg->write_reg(p_dri, 0xF003, 0x08);
    p_cfg->write_reg(p_dri, 0xF004, 0x00);
    //Display control
    p_cfg->write_reg(p_dri, 0xB100, 0xCC);
    p_cfg->write_reg(p_dri, 0xB101, 0x00);
    //Source hold time
    p_cfg->write_reg(p_dri, 0xB600, 0x05);
    //Gate EQ control
    p_cfg->write_reg(p_dri, 0xB700, 0x70);
    p_cfg->write_reg(p_dri, 0xB701, 0x70);
    //Source EQ control (Mode 2)
    p_cfg->write_reg(p_dri, 0xB800, 0x01);
    p_cfg->write_reg(p_dri, 0xB801, 0x03);
    p_cfg->write_reg(p_dri, 0xB802, 0x03);
    p_cfg->write_reg(p_dri, 0xB803, 0x03);
    //Inversion mode (2-dot)
    p_cfg->write_reg(p_dri, 0xBC00, 0x02);
    p_cfg->write_reg(p_dri, 0xBC01, 0x00);
    p_cfg->write_reg(p_dri, 0xBC02, 0x00);
    //Timing control 4H w/ 4-delay
    p_cfg->write_reg(p_dri, 0xC900, 0xD0);
    p_cfg->write_reg(p_dri, 0xC901, 0x02);
    p_cfg->write_reg(p_dri, 0xC902, 0x50);
    p_cfg->write_reg(p_dri, 0xC903, 0x50);
    p_cfg->write_reg(p_dri, 0xC904, 0x50);
    p_cfg->write_reg(p_dri, 0x3500, 0x00);
    p_cfg->write_reg(p_dri, 0x3A00, 0x55);  //16-bit/pixel
    
    p_cfg->write_cmd(p_dri, 0x1100);
    p_cfg->delay_us(120);
    p_cfg->write_cmd(p_dri, 0x2900);

    // 2.3 设置扫描方向、清屏
    p_ops->set_scan_dir(p_cfg, p_dri->lcd_info.dir);
    p_ops->clear_screen(p_cfg, p_ops);

    // 3. 最后亮背光
    p_drv_info->bl_pin.gpio_ops.gpio_output_set(&p_drv_info->bl_pin.gpio_cfg, 1);

    trace_info("nt35510 init.\r\n");
    return 0;
}

int nt35510_set_cursor(tftlcd_cfg_t *p_cfg, uint16_t xpos, uint16_t ypos)
{
    tftlcd_driver_t *p_dri = p_cfg->p_dri;
    tftlcd_info_t   lcd_info = p_cfg->p_dri->lcd_info;

    p_cfg->write_cmd(p_dri, lcd_info.setxcmd);
    p_cfg->write_data(p_dri, (xpos >> 8));
    p_cfg->write_cmd(p_dri, lcd_info.setxcmd+1);
    p_cfg->write_data(p_dri, (xpos & 0xFF));
    p_cfg->write_cmd(p_dri, lcd_info.setycmd);
    p_cfg->write_data(p_dri, (ypos >> 8));
    p_cfg->write_cmd(p_dri, lcd_info.setycmd+1);
    p_cfg->write_data(p_dri, (ypos & 0xFF));

    return 0;
}

int nt35510_write_ram_pre(tftlcd_cfg_t *p_cfg)
{
    tftlcd_driver_t *p_dri = p_cfg->p_dri;
    tftlcd_info_t   lcd_info = p_cfg->p_dri->lcd_info;

    p_cfg->write_cmd(p_dri, lcd_info.wramcmd);

    return 0;
}

int nt35510_write_ram(tftlcd_cfg_t *p_cfg, uint16_t color)
{
    tftlcd_driver_t *p_dri = p_cfg->p_dri;
    nt35510_fsmc_info_t *p_drv_info = (nt35510_fsmc_info_t *)p_cfg->p_dri->p_tft_cfg;

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

// dir = 0 --> 竖屏
// dir = 1 --> 横屏
int nt35510_set_scan_dir(tftlcd_cfg_t *p_cfg, uint16_t dir)
{
    tftlcd_driver_t *p_dri = p_cfg->p_dri;
    tftlcd_info_t   lcd_info = p_cfg->p_dri->lcd_info;
    uint16_t regval = 0;

    if (dir == 0)
    {
        regval |= (0<<7)|(0<<6)|(0<<5);     //从左到右,从上到下
    }
    else if (dir == 1)
    {
        regval |= (1<<7)|(0<<6)|(1<<5);     //从右到左,从上到下
    }

    p_cfg->p_dri->lcd_info.dir = dir;
    if (dir == 1)           // 横屏时需要对调
    {
        uint16_t tmp = p_cfg->p_dri->lcd_info.width;
        p_cfg->p_dri->lcd_info.width = p_cfg->p_dri->lcd_info.height;
        p_cfg->p_dri->lcd_info.height = tmp;
    }
    
    p_cfg->write_reg(p_dri, 0x3600, regval);
    p_cfg->write_cmd(p_dri, lcd_info.setxcmd);
    p_cfg->write_data(p_dri, 0);
    p_cfg->write_cmd(p_dri, lcd_info.setxcmd+1);
    p_cfg->write_data(p_dri, 0);
    p_cfg->write_cmd(p_dri, lcd_info.setxcmd+2);
    p_cfg->write_data(p_dri, ((p_cfg->p_dri->lcd_info.width - 1) >> 8));
    p_cfg->write_cmd(p_dri, lcd_info.setxcmd+3);
    p_cfg->write_data(p_dri, ((p_cfg->p_dri->lcd_info.width - 1) & 0xff));

    p_cfg->write_cmd(p_dri, lcd_info.setycmd);
    p_cfg->write_data(p_dri, 0);
    p_cfg->write_cmd(p_dri, lcd_info.setycmd+1);
    p_cfg->write_data(p_dri, 0);
    p_cfg->write_cmd(p_dri, lcd_info.setycmd+2);
    p_cfg->write_data(p_dri, ((p_cfg->p_dri->lcd_info.height - 1) >> 8));
    p_cfg->write_cmd(p_dri, lcd_info.setycmd+3);
    p_cfg->write_data(p_dri, ((p_cfg->p_dri->lcd_info.height - 1) & 0xff));

    return 0;
}