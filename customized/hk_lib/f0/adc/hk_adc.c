#include "hk_adc.h"

int hk_adc1_init(adc_cfg_t *p_adc_cfg)
{
    hk_adc_pin_cfg *p_hk_adc_pin_cfg = (hk_adc_pin_cfg *)p_adc_cfg->p_pin_cfg;
    hk_adc_cfg *p_hk_adc_cfg = (hk_adc_cfg *)p_adc_cfg->p_adc_cfg;

    ADC_InitTypeDef     ADC_InitStructure;
    GPIO_InitTypeDef    GPIO_InitStructure;

    RCC_AHBPeriphClockCmd(p_hk_adc_pin_cfg->adc_pin_port_clk, ENABLE);

    /* ADC1 Periph clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    GPIO_InitStructure.GPIO_Pin  = p_hk_adc_pin_cfg->adc_pin ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(p_hk_adc_pin_cfg->adc_pin_port, &GPIO_InitStructure);

    /* ADCs DeInit */  
    ADC_DeInit(ADC1);

    /* Initialize ADC structure */
    ADC_StructInit(&ADC_InitStructure);

    /* Configure the ADC1 in continuous mode with a resolution equal to 12 bits  */
    ADC_InitStructure.ADC_Resolution = p_hk_adc_cfg->adc_resolution;
    ADC_InitStructure.ADC_ContinuousConvMode = p_hk_adc_cfg->adc_continuous_mode; 
    ADC_InitStructure.ADC_ExternalTrigConvEdge = p_hk_adc_cfg->adc_trigger;
    ADC_InitStructure.ADC_DataAlign = p_hk_adc_cfg->adc_data_align;
    ADC_InitStructure.ADC_ScanDirection = p_hk_adc_cfg->adc_scan_dir;
    ADC_Init(ADC1, &ADC_InitStructure);

    /* ADC Calibration */
    ADC_GetCalibrationFactor(ADC1);
    
    /* Enable the ADC peripheral */
    ADC_Cmd(ADC1, ENABLE);     
    
    /* Wait the ADRDY flag */
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY)); 

    return 0;
}

int hk_adc1_enable(adc_cfg_t *p_adc_cfg)
{
    ADC_Cmd(ADC1, ENABLE);

    return 0;
}

int hk_adc1_disable(adc_cfg_t *p_adc_cfg)
{
    ADC_Cmd(ADC1, DISABLE);

    return 0;
}

int hk_adc_value_get(adc_cfg_t *p_adc_cfg, uint16_t *p_value)
{
    ADC_ChannelConfig(ADC1, p_adc_cfg->channel, ADC_SampleTime_239_5Cycles);

    /**
     * 启动转换
     */
    ADC_StartOfConversion(ADC1);

    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);

    *p_value = ADC_GetConversionValue(ADC1);

    return 0;
}
