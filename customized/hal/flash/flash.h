#ifndef __FLASH_H
#define __FLASH_H

#include "stdint.h"

typedef struct
{
    uint32_t flash_start_addr;
    uint16_t flash_page_size;
    uint16_t flash_offset;
    uint16_t flash_max_size;
} flash_cfg_t;

typedef struct
{
    int (* flash_init)  (flash_cfg_t *p_flash_cfg);
    int (* flash_write) (flash_cfg_t *p_flash_cfg, uint32_t addr, uint16_t len, uint8_t *p_data);
    int (* flash_read)  (flash_cfg_t *p_flash_cfg, uint32_t addr, uint16_t len, uint8_t *p_data);
    int (* flash_erase) (flash_cfg_t *p_flash_cfg, uint32_t addr, uint16_t pages);
} flash_ops_t;

typedef struct
{
    flash_cfg_t flash_cfg;
    flash_ops_t flash_ops;
} flash_object_t;



#endif
