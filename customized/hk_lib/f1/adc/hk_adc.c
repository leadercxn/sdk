#include "hk_adc.h"
#include "gpio.h"
#include "trace.h"

int hk_adc1_init(adc_cfg_t *p_adc_cfg)
{
    gpio_object_t *p_hk_adc_pin_obj = (gpio_object_t *)p_adc_cfg->p_pin_cfg;
    hk_adc_cfg *p_hk_adc_cfg = (hk_adc_cfg *)p_adc_cfg->p_adc_cfg;

    ADC_InitTypeDef     ADC_InitStructure;

    p_hk_adc_pin_obj->gpio_ops.gpio_init(&p_hk_adc_pin_obj->gpio_cfg);

    /* ADC1 Periph clock enable */
    // RCC_APB2Periph_ADC1
    RCC_APB2PeriphClockCmd(p_hk_adc_cfg->adc_clk, ENABLE);

    // 设置ADC分频因子
    // 72M/6=12,ADC最大时间不能超过14M
    RCC_ADCCLKConfig(p_hk_adc_cfg->adc_pclk_div);					

    /* ADCs DeInit */  
    ADC_DeInit(p_hk_adc_cfg->adc_type);

    /* Initialize ADC structure */
    ADC_StructInit(&ADC_InitStructure);

    /* Configure the ADC1 in continuous mode with a resolution equal to 12 bits  */
    ADC_InitStructure.ADC_Mode = p_hk_adc_cfg->adc_mode;
    ADC_InitStructure.ADC_ScanConvMode = p_hk_adc_cfg->adc_scanconv_mode;
    ADC_InitStructure.ADC_ContinuousConvMode = p_hk_adc_cfg->adc_continuous_mode; 
    ADC_InitStructure.ADC_ExternalTrigConv = p_hk_adc_cfg->adc_trigger;
    ADC_InitStructure.ADC_DataAlign = p_hk_adc_cfg->adc_data_align;
    ADC_InitStructure.ADC_NbrOfChannel = p_hk_adc_cfg->adc_channel_num;
    ADC_Init(p_hk_adc_cfg->adc_type, &ADC_InitStructure);

    if (p_hk_adc_cfg->adc_disc_enable == ENABLE)
    {
        p_hk_adc_cfg->adc_type->CR1 &= ~(0x7 << 13);
        p_hk_adc_cfg->adc_type->CR1 &= ~(1 << 11);

        p_hk_adc_cfg->adc_type->CR1 |= ((p_hk_adc_cfg->adc_disc_num) << 13);
        p_hk_adc_cfg->adc_type->CR1 |= (1 << 11);
    }

    for (uint8_t i = 0; i < p_hk_adc_cfg->adc_channel_num; i++)
    {
        ADC_RegularChannelConfig(p_hk_adc_cfg->adc_type, p_hk_adc_cfg->adc_chn_list[i], i+1, ADC_SampleTime_239Cycles5);
    }

    if (p_hk_adc_cfg->adc_irq_enable == ENABLE)
    {
        NVIC_InitTypeDef    NVIC_InitStructure;

        NVIC_InitStructure.NVIC_IRQChannel  = p_hk_adc_cfg->adc_irqn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = p_hk_adc_cfg->adc_irq_pre_prio;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority        = p_hk_adc_cfg->adc_irq_sub_prio;
        NVIC_InitStructure.NVIC_IRQChannelCmd   = ENABLE;
        NVIC_Init(&NVIC_InitStructure);

        ADC_ITConfig(p_hk_adc_cfg->adc_type, ADC_IT_EOC, ENABLE);
    }

    if (p_hk_adc_cfg->adc_dma_enable == ENABLE)
    {
        ADC_DMACmd(p_hk_adc_cfg->adc_type, ENABLE); 
    }

    ADC_Cmd(p_hk_adc_cfg->adc_type, ENABLE);     

    ADC_ResetCalibration(p_hk_adc_cfg->adc_type);					//复位校准
    while(ADC_GetResetCalibrationStatus(p_hk_adc_cfg->adc_type));	//等待复位校准结束
    ADC_StartCalibration(p_hk_adc_cfg->adc_type);					//开启AD校准
    while(ADC_GetCalibrationStatus(p_hk_adc_cfg->adc_type));		//等待校准结束

    if (p_hk_adc_cfg->adc_trigger == ADC_ExternalTrigConv_None)
    {
        ADC_SoftwareStartConvCmd(p_hk_adc_cfg->adc_type, ENABLE );	
    }
    else
    {
        ADC_ExternalTrigConvCmd(p_hk_adc_cfg->adc_type, ENABLE );
    }

    return 0;
}

int hk_adc1_enable(adc_cfg_t *p_adc_cfg)
{
    hk_adc_cfg *p_hk_adc_cfg = (hk_adc_cfg *)p_adc_cfg->p_adc_cfg;
    ADC_Cmd(p_hk_adc_cfg->adc_type, ENABLE);

    return 0;
}

int hk_adc1_disable(adc_cfg_t *p_adc_cfg)
{
    hk_adc_cfg *p_hk_adc_cfg = (hk_adc_cfg *)p_adc_cfg->p_adc_cfg;
    ADC_Cmd(p_hk_adc_cfg->adc_type, DISABLE);

    return 0;
}

int hk_adc_value_get(adc_cfg_t *p_adc_cfg, uint16_t *p_value)
{
    hk_adc_cfg *p_hk_adc_cfg = (hk_adc_cfg *)p_adc_cfg->p_adc_cfg;

    //使能ADCx软件启动转换
    if (p_hk_adc_cfg->adc_trigger == ADC_ExternalTrigConv_None)
    {
        ADC_SoftwareStartConvCmd(p_hk_adc_cfg->adc_type, ENABLE );	
    }					
    while(ADC_GetFlagStatus(p_hk_adc_cfg->adc_type, ADC_FLAG_EOC) == RESET);

    p_hk_adc_cfg->adc_value[0] = ADC_GetConversionValue(p_hk_adc_cfg->adc_type);

    return 0;
}

extern adc_object_t g_adc_obj;
void ADC1_2_IRQHandler(void)
{
    adc_cfg_t *p_adc_cfg = (adc_cfg_t *)&g_adc_obj.adc_cfg;
    hk_adc_cfg *p_hk_adc_cfg = (hk_adc_cfg *)g_adc_obj.adc_cfg.p_adc_cfg;
    
    if (ADC_GetITStatus(p_hk_adc_cfg->adc_type, ADC_IT_EOC) != RESET)
    {
        // trace_info("trigger.\r\n");
        g_adc_obj.adc_ops.adc_irq_cb(p_adc_cfg);
    }

    ADC_ClearITPendingBit(p_hk_adc_cfg->adc_type, ADC_IT_EOC);
}

void adc_handler(adc_cfg_t *p_adc_cfg)
{
    hk_adc_cfg *p_hk_adc_cfg = (hk_adc_cfg *)g_adc_obj.adc_cfg.p_adc_cfg;

    p_hk_adc_cfg->adc_value[0] = ADC_GetConversionValue(p_hk_adc_cfg->adc_type);
}

int hk_adc_dma_ctrl(adc_cfg_t *p_adc_cfg, bool ctrl)
{
    hk_adc_cfg *p_hk_adc_cfg = (hk_adc_cfg *)g_adc_obj.adc_cfg.p_adc_cfg;

    if (ENABLE == ctrl)
    {
        ADC_DMACmd(p_hk_adc_cfg->adc_type, ENABLE);
    }
    else
    {
        ADC_DMACmd(p_hk_adc_cfg->adc_type, DISABLE);
    }
}
