#include "fatfsobj.h"
#include "trace.h"
#include <stdio.h>
#include <stdlib.h>

uint8_t     mf_mount(fatfs_cfg_t *p_cfg, uint8_t* path, uint8_t mt)
{
    return f_mount(p_cfg->fs[0], (const TCHAR*)path, mt);
}

//打开文件夹
uint8_t     mf_open(fatfs_cfg_t *p_cfg, uint8_t*path, uint8_t mode)
{
	uint8_t res;	 
	res = f_open(p_cfg->file, (const TCHAR*)path, mode);
	return res;
}

uint8_t     mf_close(fatfs_cfg_t *p_cfg)
{
	f_close(p_cfg->file);
	return 0;
}

uint8_t     mf_read(fatfs_cfg_t *p_cfg, uint16_t len)
{
	uint16_t i, t;
	uint8_t res = 0;
	uint16_t tlen = 0;

	printf("\r\nRead file data is:\r\n");
	for (i = 0; i < len / 512; i++)
	{
		res = f_read(p_cfg->file, p_cfg->fatbuf, 512, &p_cfg->br);
		if (res)
		{
			printf("Read Error:%d\r\n",res);
			break;
		}
        else
		{
			tlen += p_cfg->br;
			for (t = 0; t < p_cfg->br; t++)
            {
                printf("%c", p_cfg->fatbuf[t]); 
            }
		}
	}

	if (len % 512)
	{
		res = f_read(p_cfg->file, p_cfg->fatbuf, len % 512, &p_cfg->br);
		if (res)	//读数据出错了
		{
			printf("\r\nRead Error:%d\r\n",res);   
		}
        else
		{
			tlen += p_cfg->br;
			for (t = 0; t < p_cfg->br; t++)
            {
                printf("%c",p_cfg->fatbuf[t]); 
            }
		}	 
	}

	p_cfg->br = tlen;		// 实际读的字节数

	if (tlen)
    {
        printf("\r\nReaded data len:%d\r\n", tlen); //读到的数据长度
    }
	printf("Read data over\r\n");	
    
	return res;
}

uint8_t     mf_write(fatfs_cfg_t *p_cfg, uint8_t*dat,uint16_t len)
{
	uint8_t res;	   					   

	printf("\r\nBegin Write file...\r\n");
	printf("Write data len:%d\r\n", len);	 
	res = f_write(p_cfg->file, dat, len, &p_cfg->bw);

	if (res)
	{
		printf("Write Error:%d\r\n",res);   
	}
    else 
    {
        printf("Writed data len:%d\r\n",p_cfg->bw);
    }

	printf("Write data over.\r\n");
	return res;
}

uint8_t     mf_opendir(fatfs_cfg_t *p_cfg, uint8_t* path)
{
    return f_opendir(&p_cfg->dir,(const TCHAR*)path);	
}

uint8_t     mf_closedir(fatfs_cfg_t *p_cfg)
{
    return f_closedir(&p_cfg->dir);
}

uint8_t     mf_readdir(fatfs_cfg_t *p_cfg)
{
	uint8_t res;
	char *fn;	
    
#if _USE_LFN
 	p_cfg->fileinfo.lfsize = _MAX_LFN * 2 + 1;
	p_cfg->fileinfo.lfname = malloc(p_cfg->fileinfo.lfsize);
#endif	
    
	res = f_readdir(&p_cfg->dir, &p_cfg->fileinfo);     //读取一个文件的信息
	if (res != FR_OK || p_cfg->fileinfo.fname[0] == 0)         //读完了
	{
		free(p_cfg->fileinfo.lfname);
		return res;
	}

#if _USE_LFN
	fn = *p_cfg->fileinfo.lfname ? p_cfg->fileinfo.lfname : p_cfg->fileinfo.fname;
#else
	fn = p_cfg->fileinfo.fname;;
#endif

	printf("\r\n DIR info:\r\n");
	printf("dir.id:%d\r\n",     p_cfg->dir.id);
	printf("dir.index:%d\r\n",  p_cfg->dir.index);
	printf("dir.sclust:%d\r\n", p_cfg->dir.sclust);
	printf("dir.clust:%d\r\n",  p_cfg->dir.clust);
	printf("dir.sect:%d\r\n",   p_cfg->dir.sect);	  

	printf("\r\n");
	printf("File Name is:%s\r\n",   fn);
	printf("File Size is:%d\r\n",   p_cfg->fileinfo.fsize);
	printf("File data is:%d\r\n",   p_cfg->fileinfo.fdate);
	printf("File time is:%d\r\n",   p_cfg->fileinfo.ftime);
	printf("File Attr is:%d\r\n",   p_cfg->fileinfo.fattrib);
	printf("\r\n");
	free(p_cfg->fileinfo.lfname);
	return 0;
}

uint8_t     mf_scan_files(fatfs_cfg_t *p_cfg, uint8_t * path)
{
	FRESULT res;	  
    char *fn;   /* This function is assuming non-Unicode cfg. */

#if _USE_LFN
 	p_cfg->fileinfo.lfsize = _MAX_LFN * 2 + 1;
	p_cfg->fileinfo.lfname = malloc(p_cfg->fileinfo.lfsize);
#endif		  

    res = f_opendir(&p_cfg->dir,(const TCHAR*)path); //打开一个目录
    if (res == FR_OK) 
	{	
		printf("\r\n"); 

		while(1)
		{
	        res = f_readdir(&p_cfg->dir, &p_cfg->fileinfo);      //读取目录下的一个文件
	        if (res != FR_OK || p_cfg->fileinfo.fname[0] == 0)   //错误了/到末尾了,退出
            {
                break;
            }
	        //if (fileinfo.fname[0] == '.') continue;             //忽略上级目录
#if _USE_LFN
        	fn = *p_cfg->fileinfo.lfname ? p_cfg->fileinfo.lfname : p_cfg->fileinfo.fname;
#else							   
            fn = p_cfg->fileinfo.fname;
#endif	                                              /* It is a file. */
			printf("%s/",       path);                    //打印路径	
			printf("%s\r\n",    fn);                  //打印文件名	  
		} 
    }	  
	free(p_cfg->fileinfo.lfname);
    return res;	  
}

//显示剩余容量
//drv:盘符
//返回值:剩余容量(字节)
uint32_t    mf_showfree(fatfs_cfg_t *p_cfg, uint8_t *drv)
{
	FATFS *fs1;
	uint8_t res;
    uint32_t fre_clust=0, fre_sect=0, tot_sect=0;

    //得到磁盘信息及空闲簇数量
    res = f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect = (fs1->n_fatent - 2) * fs1->csize;//得到总扇区数
	    fre_sect = fre_clust * fs1->csize;			//得到空闲扇区数	   
#if _MAX_SS!=512
		tot_sect *= fs1->ssize/512;
		fre_sect *= fs1->ssize/512;
#endif	  
		if (tot_sect < 20480)//总容量小于10M
		{
		    /* Print free space in unit of KB (assuming 512 bytes/sector) */
            printf("\r\n磁盘总容量:%d KB\r\n"
                    "可用空间:%d KB\r\n",
                    tot_sect >> 1, fre_sect >> 1);
		}
        else
		{
		    /* Print free space in unit of KB (assuming 512 bytes/sector) */
            printf("\r\n磁盘总容量:%d MB\r\n"
                    "可用空间:%d MB\r\n",
                    tot_sect >> 11, fre_sect >> 11);
		}
	}
	return fre_sect;
}

uint8_t     mf_lseek(fatfs_cfg_t *p_cfg, uint32_t offset)
{
    return f_lseek(p_cfg->file, offset);
}

uint32_t    mf_tell(fatfs_cfg_t *p_cfg)
{
    return f_tell(p_cfg->file);
}

uint32_t    mf_size(fatfs_cfg_t *p_cfg)
{
    return f_size(p_cfg->file);
}

uint8_t     mf_mkdir(fatfs_cfg_t *p_cfg, uint8_t*pname)
{
    return f_mkdir((const TCHAR *)pname);
}

uint8_t     mf_fmkfs(fatfs_cfg_t *p_cfg, uint8_t* path, uint8_t mode, uint16_t au)
{
    return f_mkfs((const TCHAR*)path, mode, au);
}

uint8_t     mf_unlink(fatfs_cfg_t *p_cfg, uint8_t *pname)
{
    return  f_unlink((const TCHAR *)pname);
}

uint8_t     mf_rename(fatfs_cfg_t *p_cfg, uint8_t *oldname, uint8_t* newname)
{
    return  f_rename((const TCHAR *)oldname,(const TCHAR *)newname);
}

void        mf_getlabel(fatfs_cfg_t *p_cfg, uint8_t *path)
{
	uint8_t buf[20];
	uint32_t sn=0;
	uint8_t res;

	res = f_getlabel((const TCHAR *)path, (TCHAR *)buf, (DWORD*)&sn);
	if (res == FR_OK)
	{
		printf("\r\n磁盘%s 的盘符为:%s\r\n", path, buf);
		printf("磁盘%s 的序列号:%X\r\n\r\n", path, sn); 
	}
    else
    {
        printf("\r\n获取失败，错误码:%X\r\n", res);
    } 
}

void        mf_setlabel(fatfs_cfg_t *p_cfg, uint8_t *path)
{
	uint8_t res;

	res = f_setlabel((const TCHAR *)path);
	if (res == FR_OK)
	{
		printf("\r\n磁盘盘符设置成功:%s\r\n", path);
	}
    else 
    {
        printf("\r\n磁盘盘符设置失败，错误码:%X\r\n",res);
    }
}

void        mf_gets(fatfs_cfg_t *p_cfg, uint16_t size)
{
    TCHAR* rbuf;
	rbuf = f_gets((TCHAR*)p_cfg->fatbuf, size, p_cfg->file);

	if (*rbuf == 0)             //没有数据读到
    {
        return  ;
    }
	else
	{
		printf("\r\nThe String Readed Is:%s\r\n", rbuf);  	  
	}	
}

uint8_t     mf_putc(fatfs_cfg_t *p_cfg, uint8_t c)
{
	return f_putc((TCHAR)c, p_cfg->file);
}

uint8_t     mf_puts(fatfs_cfg_t *p_cfg, uint8_t *c)
{
	return f_puts((TCHAR*)c, p_cfg->file);
}
