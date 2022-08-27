#include "tftlcd.h"
#include "font.h"

int tftlcd_clear_screen(tftlcd_cfg_t *p_cfg, struct tftlcd_ops *p_ops)
{
    p_ops->set_cursor(p_cfg, 0, 0);
    p_ops->write_ram_pre(p_cfg);

    for (uint16_t y = 0; y < p_cfg->p_dri->lcd_info.height; y++)
    {
        for (uint16_t x = 0; x < p_cfg->p_dri->lcd_info.width; x++)
        {
            p_ops->write_ram(p_cfg, p_cfg->p_dri->lcd_info.background_color);
        }
    }

    return 0;
}

int tftlcd_fill_area(tftlcd_cfg_t *p_cfg, struct tftlcd_ops *p_ops, fill_object_t area)
{
    uint16_t xlen = area.coord_e.x - area.coord_s.x;

    for (uint16_t y = area.coord_s.y; y < area.coord_e.y; y++)
    {
        p_ops->set_cursor(p_cfg, area.coord_s.x, y);
        p_ops->write_ram_pre(p_cfg);

        for (uint16_t x = 0; x < xlen; x++)
        {
            p_ops->write_ram(p_cfg, area.color);
        }
    }

    return 0;
}

int tftlcd_fill_area_color(tftlcd_cfg_t *p_cfg, struct tftlcd_ops *p_ops, fill_object_t area, uint16_t *color)
{
    uint16_t width = area.coord_e.x - area.coord_s.x + 1;
    uint16_t height = area.coord_e.y - area.coord_s.y + 1;

    for (uint16_t y = 0; y < height; y++)
    {
        p_ops->set_cursor(p_cfg, area.coord_s.x, area.coord_s.y+y);
        p_ops->write_ram_pre(p_cfg);

        for (uint16_t x = 0; x < width; x++)
        {
            p_ops->write_ram(p_cfg, *color++);
        }
    }

    return 0;
}

int tftlcd_draw_point(tftlcd_cfg_t *p_cfg, struct tftlcd_ops *p_ops, point_object_t point)
{
    p_ops->set_cursor(p_cfg, point.coord.x, point.coord.y);
    p_ops->write_ram_pre(p_cfg);
    p_ops->write_ram(p_cfg, point.color);

    return 0;
}

int tftlcd_show_char(tftlcd_cfg_t *p_cfg, struct tftlcd_ops *p_ops, chars_info_t ch)
{
    uint8_t temp, t1, t;
    point_object_t point;     // 构建点变量
	uint16_t y0;

    //得到字体一个字符对应点阵集所占的字节数	
	uint8_t csize = (ch.size / 8 + ((ch.size % 8) ? 1:0))*(ch.size / 2);	
    tftlcd_info_t lcdinfo = p_cfg->p_dri->lcd_info;
    
    y0 = ch.coord.y;
    point.coord =   ch.coord;

    //得到偏移后的值(ASCII字库是从空格开始取模，所以-' '就是对应字符的字库)
    ch.num = ch.num - ' ';       
    
	for (t = 0; t < csize; t++)
	{   
		if (ch.size == 12)
        {
            temp = asc2_1206[ch.num][t]; 	 	//调用1206字体
        }
		else if (ch.size == 16)
        {
            temp = asc2_1608[ch.num][t];	        //调用1608字体
        }
		else if (ch.size == 24)
        {
            temp = asc2_2412[ch.num][t];	        //调用2412字体
        }
		else 
        {
            return -1;                            //没有的字库
        }

		for (t1 = 0; t1 < 8; t1++)
		{			    
			if (temp & 0x80)
            {
                point.color =   lcdinfo.point_color;
                p_ops->draw_point(p_cfg, p_ops, point);
            }
			else if (ch.mode == 0)
            {
                point.color =   lcdinfo.background_color;
                p_ops->draw_point(p_cfg, p_ops, point);
            }

			temp <<= 1;
			point.coord.y++;
			if (point.coord.y >= lcdinfo.height)
            {
                return -1;		//超区域
            }

			if ((point.coord.y - y0) == ch.size)
			{
				point.coord.y = y0;
				point.coord.x++;
				if (point.coord.x >= lcdinfo.width)
                {
                    return -1;	//超区域
                }
				break;
			}
		}  	 
	} 

    return 0; 	 
}
