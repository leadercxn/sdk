#ifndef __HK_FLASH_H
#define __HK_FLASH_H

#include "hk32f0xx.h"
#include "flash.h"

int hk_flash_write(flash_cfg_t *p_flash_cfg, uint32_t addr, uint16_t len, uint8_t *p_data);

int hk_flash_read(flash_cfg_t *p_flash_cfg, uint32_t addr, uint16_t len, uint8_t *p_data);

#endif
