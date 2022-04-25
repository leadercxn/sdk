#include "stddef.h"
#include "stdbool.h"
#include "stdint.h"

#include "timer.h"
#include "util.h"
#include "hk_timer.h"

/**
 * @brief 
 * 
 * 定时器中端周期 period = SystemCoreClock/TIM_Prescaler * TIM_Period;
 * 48000000/48*1000 = 1K = 1ms
 */
int hk_timer3_init(timer_cfg_t *p_timer_cfg)
{
    IS_NULL(p_timer_cfg);

    static uint16_t prescaler_value = 0;

    /* 产生1M的时钟 */
    prescaler_value = (SystemCoreClock / p_timer_cfg->prescaler_freq) - 1;

    TIM_TimeBaseInitTypeDef     TIM_TimeBaseStructure;
    NVIC_InitTypeDef            NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel         = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd      = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_TimeBaseStructure.TIM_Period        = p_timer_cfg->period;
    TIM_TimeBaseStructure.TIM_Prescaler     = prescaler_value;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);

    TIM_Cmd(TIM3, ENABLE);

    return 0;
}

int hk_timer3_start(timer_cfg_t *p_timer_cfg)
{
    TIM_Cmd(TIM3, ENABLE);

    return 0;
}

int hk_timer3_stop (timer_cfg_t *p_timer_cfg)
{
    TIM_Cmd(TIM3, DISABLE);

    return 0;
}

int hk_timer3_irq_enable (timer_cfg_t *p_timer_cfg)
{
    TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);

    return 0;
}

int hk_timer3_irq_disable (timer_cfg_t *p_timer_cfg)
{
    TIM_ITConfig(TIM3, TIM_IT_CC1, DISABLE);

    return 0;
}

extern timer_object_t g_timer3_object;
/**
 * @brief 只能靠编程人员把对应的定时器全局变量放到对应的中断函数中
 */
void TIM3_IRQHandler(void)
{
    /* TIM3 通道1产生定时器更新 */
    if(TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);

        /* ticks +1 */
        g_timer3_object.timer_cfg.ticks++;

        /* 中断回调函数 */
        g_timer3_object.timer_ops.timer_irq_cb(&g_timer3_object.timer_cfg);
    }
}



