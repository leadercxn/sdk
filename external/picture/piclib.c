#include "piclib.h"
#include "little_malloc.h"

pic_info_t picinfo;	 	//ͼƬ��Ϣ
pic_phy_t pic_phy;		//ͼƬ��ʾ����ӿ�	

/***********    basic function for piclib    ******************/
// ���㺯��
uint16_t piclib_readpoint(uint16_t x, uint16_t y)
{
	tftlcd_cfg_t p_cfg = g_tftlcd_lvgl_obj->tftlcd_cfg;
	tftlcd_ops_t p_ops = g_tftlcd_lvgl_obj->tftlcd_ops;
	uint16_t point;

	point = p_ops.read_point(&p_cfg, &p_ops, x, y);
	return point;
}								

// ���㺯��
void piclib_drawpoint(uint16_t x0, uint16_t y0, uint16_t color)
{	
	tftlcd_cfg_t p_cfg = g_tftlcd_lvgl_obj->tftlcd_cfg;
	tftlcd_ops_t p_ops = g_tftlcd_lvgl_obj->tftlcd_ops;
	point_object_t point;

	point.coord.x = x0;
	point.coord.y = y0;
	point.color	  = color;

	p_ops.draw_point(&p_cfg, &p_ops, point);
}					

// ��ɫ��亯��
void piclib_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
{
	tftlcd_driver_t *p_dri = g_tftlcd_lvgl_obj->tftlcd_cfg.p_dri;
	fill_object_t area;

	area.coord_s.x = sx;
	area.coord_s.y = sy;
	area.coord_e.x = ex;
	area.coord_e.y = ey;
	area.color = color;

	if ((area.coord_e.x > p_dri->lcd_info.width) || (area.coord_e.y > p_dri->lcd_info.height))
	{
		return;
	}

	g_tftlcd_lvgl_obj->tftlcd_ops.fill_area(&g_tftlcd_lvgl_obj->tftlcd_cfg, &g_tftlcd_lvgl_obj->tftlcd_ops, area);
}	

// ��ˮƽ�ߺ���	
void piclib_draw_hline(uint16_t x0, uint16_t y0, uint16_t len, uint16_t color)
{
	tftlcd_driver_t *p_dri = g_tftlcd_lvgl_obj->tftlcd_cfg.p_dri;
	fill_object_t area;
	area.coord_s.x = x0;
	area.coord_s.y = y0;
	area.coord_e.x = x0 + len - 1;
	area.coord_e.y = y0;
	area.color = color;

	if ((len == 0) || (x0 > p_dri->lcd_info.width) || (y0 > p_dri->lcd_info.height))
	{
		return;
	}

	g_tftlcd_lvgl_obj->tftlcd_ops.fill_area(&g_tftlcd_lvgl_obj->tftlcd_cfg, &g_tftlcd_lvgl_obj->tftlcd_ops, area);
}

//�����ɫ
void piclib_fill_color(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint16_t *color)
{  
	tftlcd_driver_t *p_dri = g_tftlcd_lvgl_obj->tftlcd_cfg.p_dri;
	fill_object_t area;

	area.coord_s.x = x;
	area.coord_s.y = y;
	area.coord_e.x = x + width -1;
	area.coord_e.y = y + height -1;

	if ((area.coord_e.x > p_dri->lcd_info.width) || (area.coord_e.y > p_dri->lcd_info.height))
	{
		return;
	}

	g_tftlcd_lvgl_obj->tftlcd_ops.fill_area_color(&g_tftlcd_lvgl_obj->tftlcd_cfg, &g_tftlcd_lvgl_obj->tftlcd_ops, area, color);
}

//��ͼ��ʼ��,�ڻ�ͼ֮ǰ,�����ȵ��ô˺���
//ָ������/����
void piclib_init(void)
{
	tftlcd_info_t lcd_info = g_tftlcd_lvgl_obj->tftlcd_cfg.p_dri->lcd_info;

	pic_phy.read_point	= piclib_readpoint;  	//���㺯��ʵ��,��BMP��Ҫ
	pic_phy.draw_point	= piclib_drawpoint;		//���㺯��ʵ��
	pic_phy.fill		= piclib_fill;			//��亯��ʵ��,��GIF��Ҫ
	pic_phy.draw_hline	= piclib_draw_hline;  	//���ߺ���ʵ��,��GIF��Ҫ
	pic_phy.fillcolor	= piclib_fill_color;  	//��ɫ��亯��ʵ��,��TJPGD��Ҫ 

	picinfo.lcdwidth	= lcd_info.width;		//�õ�LCD�Ŀ������
	picinfo.lcdheight	= lcd_info.height;		//�õ�LCD�ĸ߶�����

	picinfo.ImgWidth	= 0;					//��ʼ�����Ϊ0
	picinfo.ImgHeight	= 0; 					//��ʼ���߶�Ϊ0
	picinfo.Div_Fac		= 0;					//��ʼ������ϵ��Ϊ0
	picinfo.S_Height	= 0;					//��ʼ���趨�ĸ߶�Ϊ0
	picinfo.S_Width		= 0;					//��ʼ���趨�Ŀ��Ϊ0
	picinfo.S_XOFF		= 0;					//��ʼ��x���ƫ����Ϊ0
	picinfo.S_YOFF		= 0;					//��ʼ��y���ƫ����Ϊ0
	picinfo.staticx		= 0;					//��ʼ����ǰ��ʾ����x����Ϊ0
	picinfo.staticy		= 0;					//��ʼ����ǰ��ʾ����y����Ϊ0
}

//����ALPHA BLENDING�㷨.
//src:Դ��ɫ
//dst:Ŀ����ɫ
//alpha:͸���̶�(0~32)
//����ֵ:��Ϻ����ɫ.
uint16_t piclib_alpha_blend(uint16_t src,uint16_t dst,uint8_t alpha)
{
	uint32_t src2;
	uint32_t dst2;	 

	//Convert to 32bit |-----GGGGGG-----RRRRR------BBBBB|
	src2 = ((src << 16) | src) & 0x07E0F81F;
	dst2 = ((dst << 16) | dst) & 0x07E0F81F;   

	//Perform blending R:G:B with alpha in range 0..32
	//Note that the reason that alpha may not exceed 32 is that there are only
	//5bits of space between each R:G:B value, any higher value will overflow
	//into the next component and deliver ugly result.
	dst2 = ((((dst2 - src2) * alpha) >> 5) + src2) & 0x07E0F81F;
	return (dst2 >> 16) | dst2;  
}

//��ʼ�����ܻ���, �ڲ�����
void ai_draw_init(void)
{
	float temp, temp1;	   
	temp = (float)picinfo.S_Width / picinfo.ImgWidth;
	temp1 = (float)picinfo.S_Height / picinfo.ImgHeight;	
	
	if (temp < temp1)		//ȡ��С���Ǹ�	
	{
		temp1 = temp; 
	}
	if (temp1 > 1)
	{
		temp1 = 1;
	}
	
	//ʹͼƬ��������������м�
	picinfo.S_XOFF += (picinfo.S_Width - temp1 * picinfo.ImgWidth) / 2;
	picinfo.S_YOFF += (picinfo.S_Height - temp1 * picinfo.ImgHeight) / 2;

	temp1 *= 8192;							//����8192��	 
	picinfo.Div_Fac = temp1;
	picinfo.staticx = 0xffff;
	picinfo.staticy = 0xffff;				//�ŵ�һ�������ܵ�ֵ����			 										    
}   

//�ж���������Ƿ������ʾ
//(x,y) :����ԭʼ����
//chg   :���ܱ���. 
//����ֵ:0,����Ҫ��ʾ.1,��Ҫ��ʾ
uint8_t is_element_ok(uint16_t x, uint16_t y, uint8_t chg)
{				  
	if (x != picinfo.staticx || y != picinfo.staticy)
	{
		if (chg == 1)
		{
			picinfo.staticx = x;
			picinfo.staticy = y;
		} 
		return 1;
	}
	else 
	{
		return 0;
	}
}

//���ܻ�ͼ
//FileName:Ҫ��ʾ��ͼƬ�ļ�  BMP/JPG/JPEG/GIF
//x,y,width,height:���꼰��ʾ����ߴ�
//fast:ʹ��jpeg/jpgСͼƬ(ͼƬ�ߴ�С�ڵ���Һ���ֱ���)���ٽ���,0,��ʹ��;1,ʹ��.
//ͼƬ�ڿ�ʼ�ͽ���������㷶Χ����ʾ
uint8_t ai_load_picfile(const uint8_t *filename, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t fast)
{	
	tftlcd_info_t lcd_info = g_tftlcd_lvgl_obj->tftlcd_cfg.p_dri->lcd_info;
	uint8_t	res = 0;//����ֵ
	uint8_t temp;	

	if ((x + width) > picinfo.lcdwidth)		//x���곬��Χ��.
	{
		return PIC_WINDOW_ERR;		
	}

	if ((y + height) > picinfo.lcdheight)	//y���곬��Χ��.  
	{
		return PIC_WINDOW_ERR;		
	}

	//�õ���ʾ�����С	  	 
	if (width == 0|| height ==0 )			//�����趨����
	{
		return PIC_WINDOW_ERR;	
	}

	picinfo.S_Height = height;
	picinfo.S_Width = width;

	//��ʾ������Ч
	if (picinfo.S_Height == 0 || picinfo.S_Width == 0)
	{
		picinfo.S_Height = lcd_info.height;
		picinfo.S_Width = lcd_info.width;
		return FALSE;   
	}

	if (pic_phy.fillcolor == NULL)			//��ɫ��亯��δʵ��,���ܿ�����ʾ
	{
		fast = 0;
	}

	//��ʾ�Ŀ�ʼ�����
	picinfo.S_YOFF = y;
	picinfo.S_XOFF = x;

	//�ļ�������		 
	temp = f_typetell((uint8_t*)filename);		//�õ��ļ�������

	switch (temp)
	{											  
		case T_BMP:
			res = stdbmp_decode(filename);		//����bmp	  	  
			break;
		case T_JPG:
		case T_JPEG:
			res = jpg_decode(filename, fast);	//����JPG/JPEG	  	  
			break;
		case T_GIF:
			res = gif_decode(filename, x, y, width, height);	//����gif  	  
			break;
		default:
	 		res = PIC_FORMAT_ERR;  						//��ͼƬ��ʽ!!!  
			break;
	}  											   
	return res;
}

//��̬�����ڴ�
void *pic_memalloc (uint32_t size)			
{
	return (void*)little_malloc(SRAMIN,size);
}

//�ͷ��ڴ�
void pic_memfree (void* mf)		 
{
	little_free(SRAMIN,mf);
}

