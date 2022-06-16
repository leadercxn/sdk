#ifndef __HK_SDIO_H
#define __HK_SDIO_H

#include "sdio.h"
#include "stm32f10x.h"

typedef struct 
{
    gpio_object_t sd_gpio1;
    gpio_object_t sd_gpio2;
} hk_sdio_hw_cfg_t;


SD_Error hk_sd_init(sdio_cfg_t *p_cfg);
void hk_sdio_clk_set(uint8_t clkdiv);
void hk_sdio_send_cmd(uint8_t cmdindex,uint8_t waitrsp,uint32_t arg);
void hk_sdio_send_data_cfg(uint32_t datatimeout,uint32_t datalen,uint8_t blksize,uint8_t dir);
SD_Error hk_sd_pwr_on(sdio_cfg_t *p_cfg);    
SD_Error hk_sd_pwr_off(void);
SD_Error hk_sd_initial_cards(sdio_cfg_t *p_cfg);
SD_Error hk_sd_get_card_info(sdio_cfg_t *p_cfg, sd_card_info_t *cardinfo);		  
SD_Error hk_sd_enable_widebus_operation(sdio_cfg_t *p_cfg, uint32_t wmode);
SD_Error hk_sd_set_device_mode(sdio_cfg_t *p_cfg, uint32_t mode);
SD_Error hk_sd_select_deselect(uint32_t addr); 
SD_Error hk_sd_send_status(uint32_t *pcardstatus);
SDCardState hk_sd_get_status(void);
SD_Error hk_sd_read_block(sdio_cfg_t *p_cfg, uint8_t *buf,long long addr,uint16_t blksize);  
SD_Error hk_sd_read_multi_blocks(sdio_cfg_t *p_cfg, uint8_t *buf,long long  addr,uint16_t blksize,uint32_t nblks);  
SD_Error hk_sd_write_block(sdio_cfg_t *p_cfg, uint8_t *buf,long long addr,  uint16_t blksize);	
SD_Error hk_sd_write_multi_blocks(sdio_cfg_t *p_cfg, uint8_t *buf,long long addr,uint16_t blksize,uint32_t nblks);
SD_Error hk_sd_process_IRQSrc(void);
void hk_sd_dma_cfg(uint32_t*mbuf,uint32_t bufsize,uint32_t dir); 
uint8_t hk_sd_read_disk(sdio_cfg_t *p_cfg, uint8_t*buf,uint32_t sector,uint8_t cnt); 	//读SD卡,fatfs/usb调用
uint8_t hk_sd_write_disk(sdio_cfg_t *p_cfg, uint8_t*buf,uint32_t sector,uint8_t cnt);	//写SD卡,fatfs/usb调用
void hk_sdio_show_cardinfo(sdio_cfg_t *p_cfg);


#endif
