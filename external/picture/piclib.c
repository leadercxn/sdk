#include "piclib.h"
#include "little_malloc.h"

pic_info_t picinfo;	 	//图片信息
pic_phy_t pic_phy;		//图片显示物理接口	

/***********    basic function for piclib    ******************/
// 读点函数
uint16_t piclib_readpoint(uint16_t x, uint16_t y)
{
	tftlcd_cfg_t p_cfg = g_tftlcd_lvgl_obj->tftlcd_cfg;
	tftlcd_ops_t p_ops = g_tftlcd_lvgl_obj->tftlcd_ops;
	uint16_t point;

	point = p_ops.read_point(&p_cfg, &p_ops, x, y);
	return point;
}								

// 画点函数
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

// 单色填充函数
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

// 画水平线函数	
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

//填充颜色
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

//画图初始化,在画图之前,必须先调用此函数
//指定画点/读点
void piclib_init(void)
{
	tftlcd_info_t lcd_info = g_tftlcd_lvgl_obj->tftlcd_cfg.p_dri->lcd_info;

	pic_phy.read_point	= piclib_readpoint;  	//读点函数实现,仅BMP需要
	pic_phy.draw_point	= piclib_drawpoint;		//画点函数实现
	pic_phy.fill		= piclib_fill;			//填充函数实现,仅GIF需要
	pic_phy.draw_hline	= piclib_draw_hline;  	//画线函数实现,仅GIF需要
	pic_phy.fillcolor	= piclib_fill_color;  	//颜色填充函数实现,仅TJPGD需要 

	picinfo.lcdwidth	= lcd_info.width;		//得到LCD的宽度像素
	picinfo.lcdheight	= lcd_info.height;		//得到LCD的高度像素

	picinfo.ImgWidth	= 0;					//初始化宽度为0
	picinfo.ImgHeight	= 0; 					//初始化高度为0
	picinfo.Div_Fac		= 0;					//初始化缩放系数为0
	picinfo.S_Height	= 0;					//初始化设定的高度为0
	picinfo.S_Width		= 0;					//初始化设定的宽度为0
	picinfo.S_XOFF		= 0;					//初始化x轴的偏移量为0
	picinfo.S_YOFF		= 0;					//初始化y轴的偏移量为0
	picinfo.staticx		= 0;					//初始化当前显示到的x坐标为0
	picinfo.staticy		= 0;					//初始化当前显示到的y坐标为0
}

//快速ALPHA BLENDING算法.
//src:源颜色
//dst:目标颜色
//alpha:透明程度(0~32)
//返回值:混合后的颜色.
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

//初始化智能画点, 内部调用
void ai_draw_init(void)
{
	float temp, temp1;	   
	temp = (float)picinfo.S_Width / picinfo.ImgWidth;
	temp1 = (float)picinfo.S_Height / picinfo.ImgHeight;	
	
	if (temp < temp1)		//取较小的那个	
	{
		temp1 = temp; 
	}
	if (temp1 > 1)
	{
		temp1 = 1;
	}
	
	//使图片处于所给区域的中间
	picinfo.S_XOFF += (picinfo.S_Width - temp1 * picinfo.ImgWidth) / 2;
	picinfo.S_YOFF += (picinfo.S_Height - temp1 * picinfo.ImgHeight) / 2;

	temp1 *= 8192;							//扩大8192倍	 
	picinfo.Div_Fac = temp1;
	picinfo.staticx = 0xffff;
	picinfo.staticy = 0xffff;				//放到一个不可能的值上面			 										    
}   

//判断这个像素是否可以显示
//(x,y) :像素原始坐标
//chg   :功能变量. 
//返回值:0,不需要显示.1,需要显示
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

//智能画图
//FileName:要显示的图片文件  BMP/JPG/JPEG/GIF
//x,y,width,height:坐标及显示区域尺寸
//fast:使能jpeg/jpg小图片(图片尺寸小于等于液晶分辨率)快速解码,0,不使能;1,使能.
//图片在开始和结束的坐标点范围内显示
uint8_t ai_load_picfile(const uint8_t *filename, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t fast)
{	
	tftlcd_info_t lcd_info = g_tftlcd_lvgl_obj->tftlcd_cfg.p_dri->lcd_info;
	uint8_t	res = 0;//返回值
	uint8_t temp;	

	if ((x + width) > picinfo.lcdwidth)		//x坐标超范围了.
	{
		return PIC_WINDOW_ERR;		
	}

	if ((y + height) > picinfo.lcdheight)	//y坐标超范围了.  
	{
		return PIC_WINDOW_ERR;		
	}

	//得到显示方框大小	  	 
	if (width == 0|| height ==0 )			//窗口设定错误
	{
		return PIC_WINDOW_ERR;	
	}

	picinfo.S_Height = height;
	picinfo.S_Width = width;

	//显示区域无效
	if (picinfo.S_Height == 0 || picinfo.S_Width == 0)
	{
		picinfo.S_Height = lcd_info.height;
		picinfo.S_Width = lcd_info.width;
		return FALSE;   
	}

	if (pic_phy.fillcolor == NULL)			//颜色填充函数未实现,不能快速显示
	{
		fast = 0;
	}

	//显示的开始坐标点
	picinfo.S_YOFF = y;
	picinfo.S_XOFF = x;

	//文件名传递		 
	temp = f_typetell((uint8_t*)filename);		//得到文件的类型

	switch (temp)
	{											  
		case T_BMP:
			res = stdbmp_decode(filename);		//解码bmp	  	  
			break;
		case T_JPG:
		case T_JPEG:
			res = jpg_decode(filename, fast);	//解码JPG/JPEG	  	  
			break;
		case T_GIF:
			res = gif_decode(filename, x, y, width, height);	//解码gif  	  
			break;
		default:
	 		res = PIC_FORMAT_ERR;  						//非图片格式!!!  
			break;
	}  											   
	return res;
}

//动态分配内存
void *pic_memalloc (uint32_t size)			
{
	return (void*)little_malloc(SRAMIN,size);
}

//释放内存
void pic_memfree (void* mf)		 
{
	little_free(SRAMIN,mf);
}

