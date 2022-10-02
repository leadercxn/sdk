#ifndef __DMA_H
#define __DMA_H

#include "stdint.h"
#include "stdbool.h"

typedef struct 
{
    // uint32_t perbaseaddr;
    // uint32_t membaseaddr;
    // uint32_t dir;
    // uint32_t buffersize;
    // uint32_t perinc;
    // uint32_t meminc;
    // uint32_t perdatasize;
    // uint32_t memdatasize;
    // uint32_t mode;
    // uint32_t enable_m2m;
    uint32_t dma_clk;
    void *dma_param;
    void *channel;

} dma_cfg_t;

typedef struct 
{
    int (* dma_init)        (dma_cfg_t *p_dma_cfg, uint32_t *peri_addr, uint32_t *mem_addr, uint32_t buffsize, uint32_t dir);
    int (* dma_transfer_ctrl)(dma_cfg_t *p_dma_cfg, bool ctrl);
} dma_ops_t;


typedef struct 
{
    dma_cfg_t dma_cfg;
    dma_ops_t dma_ops;
} dma_object_t;


#endif
