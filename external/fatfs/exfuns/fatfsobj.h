#ifndef  __FATFSOBJ_H
#define  __FATFSOBJ_H

#include "ff.h"
#include <stdint.h>

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

typedef uint8_t (*fun_cpymsg)(uint8_t*pname, uint8_t pct, uint8_t mode);

// 公共文件区,使用malloc的时候
typedef struct fatfs_cfg{
    FATFS *fs[_VOLUMES];	//逻辑磁盘工作区.	 
    FIL *file;	  			//文件1
    FIL *ftemp;	  			//文件2.
    UINT br, bw;			//读写变量
    FILINFO fileinfo;		//文件信息
    DIR dir;  				//目录
    uint32_t total;         //总空间
    uint32_t free;          //空闲空间
    uint8_t *fatbuf;		//SD卡数据缓存区
    uint8_t *filetype;      //文件类型

    uint8_t (*f_init)(struct fatfs_cfg      *p_cfg);
    uint8_t (*f_tell)(struct fatfs_cfg      *p_cfg, uint8_t *fname);						   
    uint8_t (*f_getfree)(struct fatfs_cfg   *p_cfg, uint8_t *drv);
    uint8_t (*f_deinit)(struct fatfs_cfg    *p_cfg);

    uint32_t (*f_fdsize)(struct fatfs_cfg *p_cfg, uint8_t *fdname);
    uint8_t* (*f_get_src_dname)(struct fatfs_cfg *p_cfg, uint8_t *dpfn);	
    uint8_t (*f_copy)(struct fatfs_cfg *p_cfg, fun_cpymsg p_fun, uint8_t *psrc, 
                    uint8_t *pdst, uint32_t totsize, uint32_t cpdsize, uint8_t fwmode);	   
    uint8_t (*f_fdcopy)(struct fatfs_cfg *p_cfg, fun_cpymsg p_fun, uint8_t *psrc, 
                    uint8_t *pdst, uint32_t *totsize, uint32_t *cpdsize, uint8_t fwmode); 
}fatfs_cfg_t;

typedef struct{
    uint8_t  (*f_mount)(fatfs_cfg_t *p_cfg, uint8_t *path,uint8_t mt);
    uint8_t  (*f_open)(fatfs_cfg_t *p_cfg, uint8_t*path, uint8_t mode);
    uint8_t  (*f_close)(fatfs_cfg_t *p_cfg);
    uint8_t  (*f_read)(fatfs_cfg_t *p_cfg, uint16_t len);
    uint8_t  (*f_write)(fatfs_cfg_t *p_cfg, uint8_t*dat, uint16_t len);
    uint8_t  (*f_opendir)(fatfs_cfg_t *p_cfg, uint8_t* path);
    uint8_t  (*f_closedir)(fatfs_cfg_t *p_cfg);
    uint8_t  (*f_readdir)(fatfs_cfg_t *p_cfg);
    uint8_t  (*f_scan_files)(fatfs_cfg_t *p_cfg, uint8_t * path);
    uint32_t (*f_showfree)(fatfs_cfg_t *p_cfg, uint8_t *drv);
    uint8_t  (*f_lseek)(fatfs_cfg_t *p_cfg, uint32_t offset);
    uint32_t (*f_tell)(fatfs_cfg_t *p_cfg);
    uint32_t (*f_size)(fatfs_cfg_t *p_cfg);
    uint8_t  (*f_mkdir)(fatfs_cfg_t *p_cfg, uint8_t *pname);
    uint8_t  (*f_fmkfs)(fatfs_cfg_t *p_cfg, uint8_t* path, uint8_t mode, uint16_t au);
    uint8_t  (*f_unlink)(fatfs_cfg_t *p_cfg, uint8_t *pname);
    uint8_t  (*f_rename)(fatfs_cfg_t *p_cfg, uint8_t *oldname, uint8_t* newname);
    void     (*f_getlabel)(fatfs_cfg_t *p_cfg, uint8_t *path);
    void     (*f_setlabel)(fatfs_cfg_t *p_cfg, uint8_t *path); 
    void     (*f_gets)(fatfs_cfg_t *p_cfg, uint16_t size);
    uint8_t  (*f_putc)(fatfs_cfg_t *p_cfg, uint8_t c);
    uint8_t  (*f_puts)(fatfs_cfg_t *p_cfg, uint8_t *c);
}fatfs_ops_t;

typedef struct{
    fatfs_cfg_t fatfs_cfg;
    fatfs_ops_t fatfs_ops;
}fatfs_object_t;

uint8_t     mf_mount(fatfs_cfg_t *p_cfg, uint8_t *path,uint8_t mt);
uint8_t     mf_open(fatfs_cfg_t *p_cfg, uint8_t*path, uint8_t mode);
uint8_t     mf_close(fatfs_cfg_t *p_cfg);
uint8_t     mf_read(fatfs_cfg_t *p_cfg, uint16_t len);
uint8_t     mf_write(fatfs_cfg_t *p_cfg, uint8_t*dat, uint16_t len);
uint8_t     mf_opendir(fatfs_cfg_t *p_cfg, uint8_t* path);
uint8_t     mf_closedir(fatfs_cfg_t *p_cfg);
uint8_t     mf_readdir(fatfs_cfg_t *p_cfg);
uint8_t     mf_scan_files(fatfs_cfg_t *p_cfg, uint8_t * path);
uint32_t    mf_showfree(fatfs_cfg_t *p_cfg, uint8_t *drv);
uint8_t     mf_lseek(fatfs_cfg_t *p_cfg, uint32_t offset);
uint32_t    mf_tell(fatfs_cfg_t *p_cfg);
uint32_t    mf_size(fatfs_cfg_t *p_cfg);
uint8_t     mf_mkdir(fatfs_cfg_t *p_cfg, uint8_t *pname);
uint8_t     mf_fmkfs(fatfs_cfg_t *p_cfg, uint8_t* path, uint8_t mode, uint16_t au);
uint8_t     mf_unlink(fatfs_cfg_t *p_cfg, uint8_t *pname);
uint8_t     mf_rename(fatfs_cfg_t *p_cfg, uint8_t *oldname, uint8_t* newname);
void        mf_getlabel(fatfs_cfg_t *p_cfg, uint8_t *path);
void        mf_setlabel(fatfs_cfg_t *p_cfg, uint8_t *path); 
void        mf_gets(fatfs_cfg_t *p_cfg, uint16_t size);
uint8_t     mf_putc(fatfs_cfg_t *p_cfg, uint8_t c);
uint8_t     mf_puts(fatfs_cfg_t *p_cfg, uint8_t *c);

#endif

