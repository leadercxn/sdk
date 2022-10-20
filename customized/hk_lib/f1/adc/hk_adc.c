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
    // ADC_InitStructure.ADC_Resolution = p_hk_adc_cfg->adc_resolution;
    ADC_InitStructure.ADC_Mode = p_hk_adc_cfg->adc_mode;
    ADC_InitStructure.ADC_ScanConvMode = p_hk_adc_cfg->adc_scanconv_mode;
    ADC_InitStructure.ADC_ContinuousConvMode = p_hk_adc_cfg->adc_continuous_mode; 
    ADC_InitStructure.ADC_ExternalTrigConv = p_hk_adc_cfg->adc_trigger;
    ADC_InitStructure.ADC_DataAlign = p_hk_adc_cfg->adc_data_align;
    ADC_InitStructure.ADC_NbrOfChannel = p_hk_adc_cfg->adc_channel_num;
    ADC_Init(p_hk_adc_cfg->adc_type, &ADC_InitStructure);
    
    /* Enable the ADC peripheral */
    ADC_Cmd(p_hk_adc_cfg->adc_type, ENABLE);     

    ADC_ResetCalibration(p_hk_adc_cfg->adc_type);					//复位校准
    while(ADC_GetResetCalibrationStatus(p_hk_adc_cfg->adc_type));	//等待复位校准结束
    ADC_StartCalibration(p_hk_adc_cfg->adc_type);					//开启AD校准
    while(ADC_GetCalibrationStatus(p_hk_adc_cfg->adc_type));		//等待校准结束


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
    trace_info("adc value get.\r\n");
    hk_adc_cfg *p_hk_adc_cfg = (hk_adc_cfg *)p_adc_cfg->p_adc_cfg;
    // TODO: argu 3(rank) need to be confirmed.
    ADC_RegularChannelConfig(p_hk_adc_cfg->adc_type, p_adc_cfg->channel, 1, ADC_SampleTime_239Cycles5);

    //使能ADCx软件启动转换
    ADC_SoftwareStartConvCmd(p_hk_adc_cfg->adc_type, ENABLE );							

    while(ADC_GetFlagStatus(p_hk_adc_cfg->adc_type, ADC_FLAG_EOC) == RESET);

    *p_value = ADC_GetConversionValue(p_hk_adc_cfg->adc_type);

    return 0;
}
