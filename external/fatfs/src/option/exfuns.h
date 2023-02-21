#ifndef __EXFUNS_H
#define __EXFUNS_H 		

#include "ff.h"
#include <stdint.h>


extern FATFS *fs[_VOLUMES];  
extern FIL *file;	 
extern FIL *ftemp;	 
extern UINT br,bw;
extern FILINFO fileinfo;
extern DIR dir;
extern uint8_t *fatbuf;//SD�����ݻ�����

//f_typetell���ص����Ͷ���
//���ݱ�FILE_TYPE_TBL���.��exfuns.c���涨��
#define T_BIN		0X00	//bin�ļ�
#define T_LRC		0X10	//lrc�ļ�
#define T_NES		0X20	//nes�ļ�

#define T_TEXT		0X30	//.txt�ļ�
#define T_C			0X31	//.c�ļ�
#define T_H			0X32    //.h�ļ�

#define T_MP1		0X40	//MP1�ļ�
#define T_MP2		0X41	//MP2�ļ� 
#define T_MP3		0X42	//MP3�ļ�
#define T_MP4		0X43	//MP4�ļ�
#define T_M4A		0X44	//M4A�ļ�
#define T_3GP		0X45	//3GP�ļ�
#define T_3G2		0X46	//3G2�ļ�
#define T_OGG		0X47	//OGG�ļ�
#define T_AAC		0X48	//AAC�ļ�
#define T_WMA		0X49	//WMA�ļ�
#define T_WAV		0X4A	//WAV�ļ�
#define T_MID		0X4B	//MIDI�ļ�
#define T_FLAC		0X4C	//FLAC�ļ� 

#define T_BMP		0X50	//bmp�ļ�
#define T_JPG		0X51	//jpg�ļ�
#define T_JPEG		0X52	//jpeg�ļ�		 
#define T_GIF		0X53	//gif�ļ�   

#define T_AVI		0X60	//avi�ļ�  

uint8_t exfuns_init(void);							//�����ڴ�
uint8_t f_typetell(uint8_t *fname);						//ʶ���ļ�����
uint8_t exf_getfree(uint8_t *drv,uint32_t *total,uint32_t *free);	//�õ�������������ʣ������
uint32_t exf_fdsize(uint8_t *fdname);						//�õ��ļ��д�С
uint8_t* exf_get_src_dname(uint8_t* dpfn);																		   
uint8_t exf_copy(uint8_t(*fcpymsg)(uint8_t*pname,uint8_t pct,uint8_t mode),uint8_t *psrc,uint8_t *pdst,uint32_t totsize,uint32_t cpdsize,uint8_t fwmode);	   //�ļ�����
uint8_t exf_fdcopy(uint8_t(*fcpymsg)(uint8_t*pname,uint8_t pct,uint8_t mode),uint8_t *psrc,uint8_t *pdst,uint32_t *totsize,uint32_t *cpdsize,uint8_t fwmode);//�ļ��и���	  

#endif


