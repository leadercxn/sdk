#include "stddef.h"
#include "stdbool.h"
#include "stdint.h"

#include "timer.h"
#include "util.h"
#include "hk_timer.h"

#include "lv_hal_tick.h"

#include "trace.h"

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

    /* 产生1ms的时钟 */
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

        /* lvgl心跳节拍 */
        lv_tick_inc(1);

        /* 中断回调函数 */
        g_timer3_object.timer_ops.timer_irq_cb(&g_timer3_object.timer_cfg);
    }
}

void TIM2_Configuration(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef       TIM_OCInitStructure;
    NVIC_InitTypeDef        NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel         = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd      = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_TimeBaseStructure.TIM_Period        = 10000;    //设置100ms一次TIM2比较的周期
    TIM_TimeBaseStructure.TIM_Prescaler     = 719;      //系统主频72M，这里分频720，相当于100K的定时器2时钟
    TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode          = TIM_OCMode_PWM1;          //下面详细说明
    TIM_OCInitStructure.TIM_OutputState     = TIM_OutputState_Enable;   //TIM_OutputState_Disable;
    TIM_OCInitStructure.TIM_Pulse           = 5000;
    TIM_OCInitStructure.TIM_OCPolarity      = TIM_OCPolarity_Low;       //如果是PWM1要为Low，PWM2则为High
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);

    TIM_Cmd(TIM2, ENABLE);
    // TIM_InternalClockConfig(TIM2);
    // TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    // TIM_UpdateDisableConfig(TIM2, DISABLE);
    TIM_CtrlPWMOutputs(TIM2, ENABLE);
}


