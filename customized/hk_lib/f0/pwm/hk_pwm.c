#include "stddef.h"
#include "stdbool.h"
#include "stdint.h"

#include "pwm.h"
#include "hk_pwm.h"

int hk_timer15_ch1_pwm_init(pwm_cfg_t *p_pwm_cfg)
{
    uint32_t timer_period = 0;
    uint32_t chan1_pulse = 0;

    hk_pwm_pin_cfg *p_hk_pwm_pin = (hk_pwm_pin_cfg *)p_pwm_cfg->p_pin_cfg;
    hk_pwm_cfg *p_hk_pwm_cfg = (hk_pwm_cfg *)p_pwm_cfg->p_pwm_cfg;

    TIM_TimeBaseInitTypeDef     TIM_TimeBaseStructure;
    TIM_OCInitTypeDef           TIM_OCInitStructure;
    GPIO_InitTypeDef            GPIO_InitStructure;

    RCC_AHBPeriphClockCmd(p_hk_pwm_pin->pwm_pin_port_clk, ENABLE);

    GPIO_InitStructure.GPIO_Pin = p_hk_pwm_pin->pwm_pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(p_hk_pwm_pin->pwm_pin_port, &GPIO_InitStructure);  

    GPIO_PinAFConfig(p_hk_pwm_pin->pwm_pin_port, p_hk_pwm_pin->pwm_pin_source, p_hk_pwm_pin->pwm_pin_af);

    timer_period = (SystemCoreClock / p_hk_pwm_cfg->freq ) - 1;
    chan1_pulse = (uint16_t) (((uint32_t) p_hk_pwm_cfg->duty_cycle * (timer_period - 1)) / 100);

    /* TIM15 clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15 , ENABLE);

    /* Time Base configuration */
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = timer_period;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM15, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode =        TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_OutputState =   TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState =  TIM_OutputNState_Enable;
    TIM_OCInitStructure.TIM_Pulse =         chan1_pulse;
    TIM_OCInitStructure.TIM_OCPolarity =    p_hk_pwm_cfg->output_polarity;
    TIM_OCInitStructure.TIM_OCNPolarity =   p_hk_pwm_cfg->output_n_polarity;
    TIM_OCInitStructure.TIM_OCIdleState =   TIM_OCIdleState_Set;
    TIM_OCInitStructure.TIM_OCNIdleState =  TIM_OCIdleState_Reset;

    TIM_OC1Init(TIM15, &TIM_OCInitStructure);

    TIM_Cmd(TIM15, ENABLE);

    return 0;
}

int hk_timer15_ch1_pwm_enable(pwm_cfg_t *p_pwm_cfg)
{
    TIM_CtrlPWMOutputs(TIM15, ENABLE);

    return 0;
}

int hk_timer15_ch1_pwm_disable(pwm_cfg_t *p_pwm_cfg)
{
    TIM_CtrlPWMOutputs(TIM15, ENABLE);

    return 0;
}
