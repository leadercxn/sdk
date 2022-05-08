#include "stddef.h"
#include "stdbool.h"
#include "stdint.h"

#include "flash.h"
#include "lib_error.h"
#include "util.h"
#include "hk_flash.h"

/**
 * @brief 往flash 写入 len 个 字
 * 
 * @param len   字（4B） 长度
 */
int hk_flash_write(flash_cfg_t *p_flash_cfg, uint32_t addr, uint16_t len, uint8_t *p_data)
{
    IS_NULL(p_flash_cfg);

    if(addr > (p_flash_cfg->flash_start_addr + p_flash_cfg->flash_max_size))
    {
        return -EINVAL;
    }

    if( (addr + len ) > (p_flash_cfg->flash_start_addr + p_flash_cfg->flash_max_size))
    {
        return -EFBIG;
    }

    /**
     * 检查字节对齐
     */
    if(len % sizeof(uint32_t))
    {
        return -EIO;
    }

    uint32_t *p = (uint32_t *)p_data;

    uint16_t number_of_page = 0;
    uint8_t i = 0;

    uint32_t write_address = 0x00;

    FLASH_Unlock();

    /* Clear pending flags (if any) */  
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR); 

    number_of_page = len / p_flash_cfg->flash_page_size;
    if(len % p_flash_cfg->flash_page_size)
    {
        number_of_page++;
    }

    do
    {
        if(FLASH_ErasePage(addr + (p_flash_cfg->flash_page_size * i)) != FLASH_COMPLETE)
        {
            break;
        }

        i++;
    } while (i < number_of_page);

    write_address = addr;
    for(i = 0; i < (len / 4); i++)
    {
        if(FLASH_ProgramWord(write_address, p[i]) == FLASH_COMPLETE)
        {
            
        }

        write_address += sizeof(uint32_t);
    }

    FLASH_Lock();

    return ENONE;
}

/**
 * @brief 往flash 读出 len 个 字节
 * 
 * @param len   字节（1B） 长度
 */
int hk_flash_read(flash_cfg_t *p_flash_cfg, uint32_t addr, uint16_t len, uint8_t *p_data)
{
    IS_NULL(p_flash_cfg);
    IS_NULL(p_data);

    if(addr > (p_flash_cfg->flash_start_addr + p_flash_cfg->flash_max_size))
    {
        return -EINVAL;
    }

    if( (addr + len ) > (p_flash_cfg->flash_start_addr + p_flash_cfg->flash_max_size))
    {
        return -EFBIG;
    }

    uint8_t     i = 0;
    uint32_t    read_address = addr;

    for(i = 0; i < len; i++)
    {
        p_data[i] = *(__IO uint8_t *)read_address;

        read_address += sizeof(uint8_t);
    }

    return 0;
}


