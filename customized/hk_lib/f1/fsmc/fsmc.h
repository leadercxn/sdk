#ifndef __F1_FSMC_H
#define __F1_FSMC_H

#include "stm32f10x.h"

typedef enum
{
    BUS_WIDTH_8b    =   0,   
    BUS_WIDTH_16b   =   1,
}buswidth_e;

uint16_t fsmc_conig(uint32_t sdram_no, buswidth_e buswidth);


#endif
