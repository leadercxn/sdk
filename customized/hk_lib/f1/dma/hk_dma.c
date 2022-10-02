#include "hk_dma.h"
#include "stdbool.h"

// DMA_DIR_PeripheralDST ,MEM 2 PER;
// DMA_DIR_PeripheralSRC ,PER 2 MEM;

int hk_dma_obj_init(dma_cfg_t *p_dma_cfg, uint32_t *peri_addr, uint32_t *mem_addr, uint32_t buffsize, uint32_t dir)
{
    DMA_InitTypeDef *DMA_InitStructure = (DMA_InitTypeDef *)p_dma_cfg->dma_param;
    DMA_Channel_TypeDef *chn = (DMA_Channel_TypeDef *)p_dma_cfg->channel;

    RCC_AHBPeriphClockCmd(p_dma_cfg->dma_clk, ENABLE);	    //使能DMA时钟

	DMA_DeInit(chn);   		                                //将指定通道寄存器重设为缺省值
	DMA_Cmd(chn, DISABLE); 	                                //关闭指定通道

	DMA_InitStructure->DMA_PeripheralBaseAddr 	= (uint32_t)peri_addr;  	
	DMA_InitStructure->DMA_MemoryBaseAddr 		= (uint32_t)mem_addr;  	
    DMA_InitStructure->DMA_BufferSize           = buffsize;	
    DMA_InitStructure->DMA_DIR                  = dir;
	DMA_Init(chn, DMA_InitStructure);  
    // DMA_Cmd(chn, ENABLE);               //开启该通道的DMA数据传输

    return 0;
}

int hk_dma_transfer_ctrl(dma_cfg_t *p_dma_cfg, bool ctrl)
{
    if (true == ctrl)
    {
        DMA_Cmd(p_dma_cfg->channel, ENABLE);
    }
    else
    {
        DMA_Cmd(p_dma_cfg->channel, DISABLE);
    }

    return 0;
}


