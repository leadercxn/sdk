#ifndef __HK_ADC_H
#define __HK_ADC_H

#include "adc.h"

#include "stm32f10x.h"

#define ADC_CHANNEL_NUM     6

typedef struct
{
    GPIO_TypeDef *adc_pin_port;
    uint32_t     adc_pin_port_clk;

    uint32_t     adc_pin;
} hk_adc_pin_cfg;

typedef struct
{
    FunctionalState adc_scanconv_mode;      // 单通道/多通道
    FunctionalState adc_continuous_mode;    // 单次触发/多次触发
    FunctionalState adc_dma_enable;         // dma开关
    FunctionalState adc_irq_enable;         // 中断相关配置
    IRQn_Type       adc_irqn;
    uint8_t         adc_irq_pre_prio;
    uint8_t         adc_irq_sub_prio;

    uint8_t         adc_channel_num;        // 通道数
    uint32_t        adc_mode;               
    uint32_t        adc_trigger;            // 触发方式
    uint32_t        adc_data_align;         // 数据对齐

    uint32_t        adc_clk;                // RCC_APB2Periph_ADC1
    uint32_t        adc_pclk_div;           // adc时钟分频因子， RCC_PCLK2_Div6
    ADC_TypeDef     *adc_type;              // ADC1/ADC2/...

    uint32_t        *adc_chn_list;          // adc channel list, channel次序与定义次序一致
    uint16_t        adc_value[ADC_CHANNEL_NUM];             // 存储channel序列的采样结果
    uint8_t         adc_value_idx;      
} hk_adc_cfg;

int hk_adc1_init(adc_cfg_t *p_adc_cfg);

int hk_adc1_enable(adc_cfg_t *p_adc_cfg);

int hk_adc1_disable(adc_cfg_t *p_adc_cfg);

int hk_adc_value_get(adc_cfg_t *p_adc_cfg, uint16_t *p_value);

int hk_adc_dma_ctrl(adc_cfg_t *p_adc_cfg, bool ctrl);

void adc_handler(adc_cfg_t *p_adc_cfg);

void ADCx_Init(void);

#endif
