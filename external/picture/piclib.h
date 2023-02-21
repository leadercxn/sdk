#ifndef __PICLIB_H
#define __PICLIB_H	  	

#include "little_malloc.h"
#include "ff.h"
#include "exfuns.h"
#include "bmp.h"
// #include "tjpgd.h"
// #include "gif.h"

#include "tftlcd.h"


#define PIC_FORMAT_ERR		0x27	//��ʽ����
#define PIC_SIZE_ERR		0x28	//ͼƬ�ߴ����
#define PIC_WINDOW_ERR		0x29	//�����趨����
#define PIC_MEM_ERR			0x11	//�ڴ����

#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE   0
#endif 

extern tftlcd_object_t *g_tftlcd_lvgl_obj;

//ͼƬ��ʾ�����ӿ�  
//����ֲ��ʱ��,�������û��Լ�ʵ���⼸������
// TODO: �ýӿ���Ҫ�ֶ���װtftlcd�ӿڣ�������ͼ���ļ�����ʱ�Բ���
typedef struct pic_phy
{
	uint16_t	(*read_point)	(uint16_t,uint16_t);								// ���㺯��
	void		(*draw_point)	(uint16_t,uint16_t,uint16_t);						// ���㺯��
 	void		(*fill)			(uint16_t,uint16_t,uint16_t,uint16_t,uint16_t);		// ��ɫ��亯�� 	 
 	void		(*draw_hline)	(uint16_t,uint16_t,uint16_t,uint16_t);				// ��ˮƽ�ߺ���	 
 	void		(*fillcolor)	(uint16_t,uint16_t,uint16_t,uint16_t,uint16_t*);	// ��ɫ���
}pic_phy_t; 

extern pic_phy_t pic_phy;

//ͼ����Ϣ
typedef struct pic_info
{		
	uint16_t lcdwidth;	//LCD�Ŀ��
	uint16_t lcdheight;	//LCD�ĸ߶�
	uint32_t ImgWidth; 	//ͼ���ʵ�ʿ�Ⱥ͸߶�
	uint32_t ImgHeight;

	uint32_t Div_Fac;  	//����ϵ�� (������8192����)
	
	uint32_t S_Height; 	//�趨�ĸ߶ȺͿ��
	uint32_t S_Width;
	
	uint32_t S_XOFF;	  	//x���y���ƫ����
	uint32_t S_YOFF;

	uint32_t staticx; 	//��ǰ��ʾ���ģ�������
	uint32_t staticy;																 	
}pic_info_t;

extern pic_info_t picinfo;//ͼ����Ϣ

void piclib_fill_color(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint16_t *color);
void piclib_init(void);								//��ʼ����ͼ
uint16_t piclib_alpha_blend(uint16_t src,uint16_t dst,uint8_t alpha);	//alphablend����
void ai_draw_init(void);							//��ʼ�����ܻ�ͼ
uint8_t is_element_ok(uint16_t x,uint16_t y,uint8_t chg);				//�ж������Ƿ���Ч
uint8_t ai_load_picfile(const uint8_t *filename,uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t fast);//���ܻ�ͼ
void *pic_memalloc (uint32_t size);	//pic�����ڴ�
void pic_memfree (void* mf);	//pic�ͷ��ڴ�

#endif






























