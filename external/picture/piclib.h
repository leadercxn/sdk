#ifndef __PICLIB_H
#define __PICLIB_H	  	

#include "little_malloc.h"
#include "ff.h"
#include "exfuns.h"
#include "bmp.h"
// #include "tjpgd.h"
// #include "gif.h"

#include "tftlcd.h"


#define PIC_FORMAT_ERR		0x27	//格式错误
#define PIC_SIZE_ERR		0x28	//图片尺寸错误
#define PIC_WINDOW_ERR		0x29	//窗口设定错误
#define PIC_MEM_ERR			0x11	//内存错误

#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE   0
#endif 

extern tftlcd_object_t *g_tftlcd_lvgl_obj;

//图片显示物理层接口  
//在移植的时候,必须由用户自己实现这几个函数
// TODO: 该接口需要手动封装tftlcd接口，否则在图像文件调用时对不上
typedef struct pic_phy
{
	uint16_t	(*read_point)	(uint16_t,uint16_t);								// 读点函数
	void		(*draw_point)	(uint16_t,uint16_t,uint16_t);						// 画点函数
 	void		(*fill)			(uint16_t,uint16_t,uint16_t,uint16_t,uint16_t);		// 单色填充函数 	 
 	void		(*draw_hline)	(uint16_t,uint16_t,uint16_t,uint16_t);				// 画水平线函数	 
 	void		(*fillcolor)	(uint16_t,uint16_t,uint16_t,uint16_t,uint16_t*);	// 颜色填充
}pic_phy_t; 

extern pic_phy_t pic_phy;

//图像信息
typedef struct pic_info
{		
	uint16_t lcdwidth;	//LCD的宽度
	uint16_t lcdheight;	//LCD的高度
	uint32_t ImgWidth; 	//图像的实际宽度和高度
	uint32_t ImgHeight;

	uint32_t Div_Fac;  	//缩放系数 (扩大了8192倍的)
	
	uint32_t S_Height; 	//设定的高度和宽度
	uint32_t S_Width;
	
	uint32_t S_XOFF;	  	//x轴和y轴的偏移量
	uint32_t S_YOFF;

	uint32_t staticx; 	//当前显示到的ｘｙ坐标
	uint32_t staticy;																 	
}pic_info_t;

extern pic_info_t picinfo;//图像信息

void piclib_fill_color(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint16_t *color);
void piclib_init(void);								//初始化画图
uint16_t piclib_alpha_blend(uint16_t src,uint16_t dst,uint8_t alpha);	//alphablend处理
void ai_draw_init(void);							//初始化智能画图
uint8_t is_element_ok(uint16_t x,uint16_t y,uint8_t chg);				//判断像素是否有效
uint8_t ai_load_picfile(const uint8_t *filename,uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t fast);//智能画图
void *pic_memalloc (uint32_t size);	//pic申请内存
void pic_memfree (void* mf);	//pic释放内存

#endif






























