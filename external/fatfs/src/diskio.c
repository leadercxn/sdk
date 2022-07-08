/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/
#include "diskio.h"			/* FatFs lower layer API */
#include "hk_peripheral.h"

#include <stdint.h>
#include <stdlib.h>


#define SD_CARD	 0  //SD卡，卷标为0  


//获得磁盘状态
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{ 
	return RES_OK;
}  

//初始化磁盘
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	uint8_t res=0;	    
	switch (pdrv)
	{
		case SD_CARD:				//SD卡
			// res = SD_Init();			
			res = g_sdio_obj.sdio_ops.sd_init(&g_sdio_obj.sdio_cfg);		//SD卡初始化
			break;

		default:
			res = 1; 
	}		 
	if (res)
	{
		return  STA_NOINIT;
	}
	else 
	{
		return 0; 			
	}
} 

//读扇区
//pdrv:磁盘编号0~9
//*buff:数据接收缓冲首地址
//sector:扇区地址
//count:需要读取的扇区数
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
	uint8_t res=0; 
    if (!count)
	{
		return RES_PARERR;		//count不能等于0，否则返回参数错误		
	}
	
	switch (pdrv)
	{
		case SD_CARD:					//SD卡
			// res = SD_ReadDisk(buff,sector,count);	
			res = g_sdio_obj.sdio_ops.sd_read_disk(&g_sdio_obj.sdio_cfg, buff, sector, count);
			while (res)					//读出错
			{
				g_sdio_obj.sdio_ops.sd_init(&g_sdio_obj.sdio_cfg);		//重新初始化SD卡
				res = g_sdio_obj.sdio_ops.sd_read_disk(&g_sdio_obj.sdio_cfg, buff, sector, count);
				//printf("sd rd error:%d\r\n",res);
			}
			break;

		default:
			res=1; 
	}

	//处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
    if (res==0x00)
	{
		return RES_OK;	 
	}
    else 
	{
		return RES_ERROR;	   
	}
}

//写扇区
//pdrv:磁盘编号0~9
//*buff:发送数据首地址
//sector:扇区地址
//count:需要写入的扇区数
#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{
	uint8_t res=0;  
    if (!count)
	{
		return RES_PARERR;		//count不能等于0，否则返回参数错误	
	}
	
	switch (pdrv)
	{
		case SD_CARD:		//SD卡
			// res = SD_WriteDisk((uint8_t*)buff,sector,count);
			res = g_sdio_obj.sdio_ops.sd_write_disk(&g_sdio_obj.sdio_cfg, (uint8_t*)buff, sector, count);
			while (res)		//写出错
			{
				g_sdio_obj.sdio_ops.sd_init(&g_sdio_obj.sdio_cfg);		//重新初始化SD卡
				res = g_sdio_obj.sdio_ops.sd_write_disk(&g_sdio_obj.sdio_cfg, (uint8_t*)buff, sector, count);	
				
// uint8_t hk_sd_write_disk(sdio_cfg_t *p_cfg, uint8_t*buf, uint32_t sector, uint8_t cnt)
				printf("sd wr error:%d\r\n",res);
			}
			break;

		default:
			res=1; 
	}

    if (res == 0x00)
	{
		return RES_OK;	 
	}
    else 
	{
		return RES_ERROR;	
	}
}
#endif

//其他表参数的获得
//pdrv:磁盘编号0~9
//ctrl:控制代码
//*buff:发送/接收缓冲区指针
#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;	

	if (pdrv == SD_CARD)				//SD卡
	{
		switch(cmd)
		{
			case CTRL_SYNC:
				res = RES_OK; 
				break;	 

			case GET_SECTOR_SIZE:
				*(DWORD*)buff = 512; 
				res = RES_OK;
				break;	 

			case GET_BLOCK_SIZE:
				*(WORD*)buff = g_sdio_obj.sdio_cfg.cardinfo->card_blocksize;
				res = RES_OK;
				break;	 

			case GET_SECTOR_COUNT:
				*(WORD*)buff = g_sdio_obj.sdio_cfg.cardinfo->card_capacity/512;
				res = RES_OK;
				break;

			default:
				res = RES_PARERR;
				break;
		}
	}
	else 
	{
		res = RES_ERROR;	// 不支持其他卡
	}

    return res;
}
#endif

//获得时间
//User defined function to give a current time to fatfs module      */
//31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */                                                                                                                                                                                                                                          
//15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */                                                                                                                                                                                                                                                 
DWORD get_fattime (void)
{				 
	return 0;
}	

//动态分配内存
void *ff_memalloc(UINT size)			
{
	return (void*)malloc(size);
}

//动态分配内存
void ff_memfree(void* mf)		 
{
	free(mf);
}

