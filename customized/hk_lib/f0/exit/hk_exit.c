#include "hk_exit.h"

int hk_exit4_15_init(exit_cfg_t *p_exit_cfg)
{
    hk_exit_pin_cfg *p_hk_exit_pin_cfg = (hk_exit_pin_cfg *)p_exit_cfg->p_pin_cfg;
    hk_exit_cfg *p_hk_exit_cfg = (hk_exit_cfg *)p_exit_cfg->p_exit_cfg;

    EXTI_InitTypeDef   EXTI_InitStructure;
    GPIO_InitTypeDef   GPIO_InitStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;

    RCC_AHBPeriphClockCmd(p_hk_exit_pin_cfg->exit_pin_port_clk, ENABLE);

    /* Enable SYSCFG clock */
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    GPIO_InitStructure.GPIO_Pin = p_hk_exit_pin_cfg->exit_pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(p_hk_exit_pin_cfg->exit_pin_port, &GPIO_InitStructure);

    SYSCFG_EXTILineConfig(p_hk_exit_pin_cfg->exit_pin_port_source, p_hk_exit_pin_cfg->exit_pin_source);

    EXTI_InitStructure.EXTI_Line = p_hk_exit_cfg->exit_line;  
    EXTI_InitStructure.EXTI_Mode = p_hk_exit_cfg->exit_mode;
    EXTI_InitStructure.EXTI_Trigger = p_hk_exit_cfg->exit_trigger;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable and set EXTI4_15 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    return 0;
}

int hk_exit4_15_enable(exit_cfg_t *p_exit_cfg)
{
    EXTI_InitTypeDef   EXTI_InitStructure;

    hk_exit_cfg *p_hk_exit_cfg = (hk_exit_cfg *)p_exit_cfg->p_exit_cfg;

    EXTI_InitStructure.EXTI_Line = p_hk_exit_cfg->exit_line;  
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = p_hk_exit_cfg->exit_trigger;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    return 0;
}

int hk_exit4_15_disable(exit_cfg_t *p_exit_cfg)
{
    EXTI_InitTypeDef   EXTI_InitStructure;

    hk_exit_cfg *p_hk_exit_cfg = (hk_exit_cfg *)p_exit_cfg->p_exit_cfg;

    EXTI_InitStructure.EXTI_Line = p_hk_exit_cfg->exit_line;  
    EXTI_InitStructure.EXTI_Mode = p_hk_exit_cfg->exit_mode;
    EXTI_InitStructure.EXTI_Trigger = p_hk_exit_cfg->exit_trigger;
    EXTI_InitStructure.EXTI_LineCmd = DISABLE;
    EXTI_Init(&EXTI_InitStructure);

    return 0;
}

extern exit_object_t g_exit4_15_obj;

void EXTI4_15_IRQHandler(void)
{
  hk_exit_cfg *p_hk_exit_cfg = (hk_exit_cfg *)g_exit4_15_obj.exit_cfg.p_exit_cfg;

  if(EXTI_GetITStatus(p_hk_exit_cfg->exit_line) != RESET)
  {
        EXTI_ClearITPendingBit(p_hk_exit_cfg->exit_line);

        g_exit4_15_obj.exit_ops.exit_irq_cb(&g_exit4_15_obj.exit_cfg);
  }
}

