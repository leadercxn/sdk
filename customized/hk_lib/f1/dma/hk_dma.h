#ifndef __HK_DMA_H
#define __HK_DMA_H

#include "dma.h"
#include "stm32f10x.h"

int hk_dma_obj_init(dma_cfg_t *p_dma_cfg, uint32_t *peri_addr, uint32_t *mem_addr, uint32_t buffsize, uint32_t dir);
int hk_dma_transfer_ctrl(dma_cfg_t *p_dma_cfg, bool ctrl);

#endif

