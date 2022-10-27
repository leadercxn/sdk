#ifndef __ADC_H
#define __ADC_H

#include "stdint.h"
#include <stdbool.h>

typedef struct
{
    void *p_pin_cfg;
    void *p_adc_cfg;

    uint32_t flag;
    uint32_t channel;

    void *p_context;     //传递给中断处理函数的数据
} adc_cfg_t;

typedef struct
{
    int (* adc_init)        (adc_cfg_t *p_adc_cfg);
    int (* adc_enable)      (adc_cfg_t *p_adc_cfg);
    int (* adc_disable)     (adc_cfg_t *p_adc_cfg);
    int (* adc_mode_set)    (adc_cfg_t *p_adc_cfg);
    int (* adc_value_get)   (adc_cfg_t *p_adc_cfg, uint16_t *p_value);
    int (* adc_dma_ctrl)    (adc_cfg_t *p_adc_cfg, bool ctrl);
    void (* adc_irq_cb)     (adc_cfg_t *p_adc_cfg);
} adc_ops_t;

typedef struct
{
    adc_cfg_t adc_cfg;
    adc_ops_t adc_ops;
} adc_object_t;


#endif
