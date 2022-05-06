#ifndef __HK_ADC_H
#define __HK_ADC_H

#include "adc.h"

#include "hk32f0xx.h"

typedef struct
{
    uint32_t     adc_clk;

    GPIO_TypeDef *adc_pin_port;
    uint32_t     adc_pin_port_clk;

    uint32_t     adc_pin;
} hk_adc_pin_cfg;

typedef struct
{
    uint8_t     adc_continuous_mode;
    uint32_t    adc_trigger;
    uint32_t    adc_resolution;
    uint32_t    adc_data_align;
    uint32_t    adc_scan_dir;
} hk_adc_cfg;

int hk_adc1_init(adc_cfg_t *p_adc_cfg);

int hk_adc1_enable(adc_cfg_t *p_adc_cfg);

int hk_adc1_disable(adc_cfg_t *p_adc_cfg);

int hk_adc_value_get(adc_cfg_t *p_adc_cfg, uint16_t *p_value);

#endif
