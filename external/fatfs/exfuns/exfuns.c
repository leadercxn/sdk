#include "string.h"
#include "exfuns.h"
#include "usart.h"
#include <stdlib.h>

#define FILE_MAX_TYPE_NUM		6	//最多FILE_MAX_TYPE_NUM个大类
#define FILE_MAX_SUBT_NUM		13	//最多FILE_MAX_SUBT_NUM个小类

 //文件类型列表
uint8_t *const FILE_TYPE_TBL[FILE_MAX_TYPE_NUM][FILE_MAX_SUBT_NUM]=
{
	{"BIN"},			//BIN文件
	{"LRC"},			//LRC文件
	{"NES"},			//NES文件
	{"TXT","C","H"},	//文本文件
	{"MP1","MP2","MP3","MP4","M4A","3GP","3G2","OGG","AAC","WMA","WAV","MID","FLAC"},//音乐文件
	{"BMP","JPG","JPEG","GIF"},//图片文件 
};

// 为exfuns申请内存
// 返回值:0,成功
// 1,失败
uint8_t exfuns_init(struct fatfs_cfg *p_cfg)
{
	uint8_t i;
	for (i = 0; i < _VOLUMES; i++)
	{
		p_cfg->fs[i] = (FATFS*)malloc(sizeof(FATFS));	//为磁盘i工作区申请内存	
		if (!p_cfg->fs[i])
		{
			break;
		}
	}

	p_cfg->file = (FIL*)malloc(sizeof(FIL));			//为file申请内存
	p_cfg->ftemp = (FIL*)malloc(sizeof(FIL));			//为ftemp申请内存
	p_cfg->fatbuf = (uint8_t*)malloc(512);				//为fatbuf申请内存
	if (i == _VOLUMES && p_cfg->file && p_cfg->ftemp && p_cfg->fatbuf)
	{
		return 0;  		
	}
	else
	{
		return 1;
	}
}

//将小写字母转为大写字母,如果是数字,则保持不变.
uint8_t char_upper(uint8_t c)
{
	if (c < 'A')
	{
		return c;		//数字,保持不变.
	}
	if (c >= 'a')
	{
		return (c - 0x20);	//变为大写.
	}
	else 
	{
		return c;		//大写,保持不变
	}
}	      

//报告文件的类型
//fname:文件名
//返回值:0XFF,表示无法识别的文件类型编号.
//		 其他,高四位表示所属大类,低四位表示所属小类.
uint8_t f_typetell(struct fatfs_cfg *p_cfg, uint8_t *fname)
{
	uint8_t tbuf[5];
	uint8_t *attr = '\0';	//后缀名
	uint8_t i = 0,j;

	while (i < 250)
	{
		i++;
		if (*fname == '\0')
		{
			break;			
		}
		fname++;
	}

	if (i == 250)
	{
		return 0XFF;		
	}

	for (i = 0; i < 5; i++)		//得到后缀名
	{
		fname--;
		if (*fname == '.')
		{
			fname++;
			attr = fname;
			break;
		}
	}

	strcpy((char *)tbuf, (const char*)attr);		
	for (i = 0; i < 4; i++)
	{
		tbuf[i] = char_upper(tbuf[i]);				//全部变为大写 
	}

	for (i = 0; i < FILE_MAX_TYPE_NUM; i++)			//大类对比
	{
		for (j = 0; j < FILE_MAX_SUBT_NUM; j++)		//子类对比
		{
			if (*FILE_TYPE_TBL[i][j] == 0)
			{
				break;							
			}

			if (strcmp((const char *)FILE_TYPE_TBL[i][j], (const char *)tbuf) == 0)		
			{
				p_cfg->filetype = (uint8_t*)malloc(10);
				strcpy(p_cfg->filetype, (const char *)FILE_TYPE_TBL[i][j]);
				
				return (i<<4)|j;
			}
		}
	}

	return 0XFF;//没找到		 			   
}	 

//得到磁盘剩余容量
//drv:磁盘编号("0:"/"1:")
//total:总容量	 （单位KB）
//free:剩余容量	 （单位KB）
//返回值:0,正常.其他,错误代码
uint8_t exf_getfree(struct fatfs_cfg *p_cfg, uint8_t *drv)
{
	FATFS *fs1;
	uint8_t res;
    uint32_t fre_clust=0, fre_sect=0, tot_sect=0;

    //得到磁盘信息及空闲簇数量
    res = (uint32_t)f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
    if (res == 0)
	{											   
	    tot_sect = (fs1->n_fatent-2)*fs1->csize;	//得到总扇区数
	    fre_sect = fre_clust*fs1->csize;			//得到空闲扇区数	   
#if _MAX_SS!=512				  					//扇区大小不是512字节,则转换为512字节
		tot_sect *= fs1->ssize/512;
		fre_sect *= fs1->ssize/512;
#endif	  
		p_cfg->total = tot_sect >> 1;	//单位为KB
		p_cfg->free = fre_sect >> 1;	//单位为KB 
	}
	return res;
}	

// 退出文件系统，销毁申请的内存
uint8_t exfuns_deinit(struct fatfs_cfg *p_cfg)
{
	if (p_cfg->filetype)
	{
		free(p_cfg->filetype);
	}

	free(p_cfg->file);
	free(p_cfg->ftemp);
	free(p_cfg->fatbuf);

	return 0;
}
