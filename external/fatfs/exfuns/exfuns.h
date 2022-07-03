#ifndef __EXFUNS_H
#define __EXFUNS_H 			

#include "stm32f10x.h"
#include "ff.h"
#include <stdint.h>

extern FATFS *fs[_VOLUMES];  
extern FIL *file;	 
extern FIL *ftemp;	 
extern UINT br,bw;
extern FILINFO fileinfo;
extern DIR dir;
extern uint8_t *fatbuf;              //SD卡数据缓存区




//f_typetell返回的类型定义
//根据表FILE_TYPE_TBL获得.在exfuns.c里面定义
#define T_BIN		0X00	//bin文件
#define T_LRC		0X10	//lrc文件
#define T_NES		0X20	//nes文件

#define T_TEXT		0X30	//.txt文件
#define T_C			0X31	//.c文件
#define T_H			0X32    //.h文件

#define T_MP1		0X40	//MP1文件
#define T_MP2		0X41	//MP2文件 
#define T_MP3		0X42	//MP3文件
#define T_MP4		0X43	//MP4文件
#define T_M4A		0X44	//M4A文件
#define T_3GP		0X45	//3GP文件
#define T_3G2		0X46	//3G2文件
#define T_OGG		0X47	//OGG文件
#define T_AAC		0X48	//AAC文件
#define T_WMA		0X49	//WMA文件
#define T_WAV		0X4A	//WAV文件
#define T_MID		0X4B	//MIDI文件
#define T_FLAC		0X4C	//FLAC文件 

#define T_BMP		0X50	//bmp文件
#define T_JPG		0X51	//jpg文件
#define T_JPEG		0X52	//jpeg文件		 
#define T_GIF		0X53	//gif文件   

#define T_AVI		0X60	//avi文件  


uint8_t exfuns_init(void);							       
uint8_t f_typetell(uint8_t *fname);						   
uint8_t exf_getfree(uint8_t *drv,uint32_t *total,uint32_t *free);	
uint32_t exf_fdsize(uint8_t *fdname);					
uint8_t* exf_get_src_dname(uint8_t* dpfn);																		   
uint8_t exf_copy(uint8_t(*fcpymsg)(uint8_t*pname, uint8_t pct, uint8_t mode), uint8_t *psrc, uint8_t *pdst, uint32_t totsize, uint32_t cpdsize, uint8_t fwmode);	   
uint8_t exf_fdcopy(uint8_t(*fcpymsg)(uint8_t*pname, uint8_t pct, uint8_t mode), uint8_t *psrc, uint8_t *pdst, uint32_t *totsize, uint32_t *cpdsize, uint8_t fwmode); 

#endif


