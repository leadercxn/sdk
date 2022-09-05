#include "hk_exit.h"
#include "hk_peripheral.h"
#include "trace.h"

int hk_exit_init(exit_cfg_t *p_exit_cfg)
{
    hk_exit_pin_cfg *p_hk_exit_pin_cfg = (hk_exit_pin_cfg *)p_exit_cfg->p_pin_cfg;
    hk_exit_cfg *p_hk_exit_cfg = (hk_exit_cfg *)p_exit_cfg->p_exit_cfg;

    EXTI_InitTypeDef   EXTI_InitStructure;
    GPIO_InitTypeDef   GPIO_InitStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;

    // 1. gpio init
    p_hk_exit_pin_cfg->exit_gpio_cfg->gpio_ops.gpio_init(&p_hk_exit_pin_cfg->exit_gpio_cfg->gpio_cfg);

    // 2. exit init
    RCC_APB2PeriphClockCmd(p_hk_exit_pin_cfg->exit_clk, ENABLE);

    GPIO_EXTILineConfig(p_hk_exit_pin_cfg->exit_pin_port_source, p_hk_exit_pin_cfg->exit_pin_source);
    EXTI_InitStructure.EXTI_Line    = p_hk_exit_cfg->exit_line;  
    EXTI_InitStructure.EXTI_Mode    = p_hk_exit_cfg->exit_mode;
    EXTI_InitStructure.EXTI_Trigger = p_hk_exit_cfg->exit_trigger;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // 3. nvic init
  	NVIC_InitStructure.NVIC_IRQChannel = p_hk_exit_cfg->exit_irqn;			//使能按键WK_UP所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = p_hk_exit_cfg->exit_pre_prio;	//抢占优先级2， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = p_hk_exit_cfg->exit_sub_prio;					//子优先级3
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
    NVIC_Init(&NVIC_InitStructure); 

    return 0;
}

int hk_exit_enable(exit_cfg_t *p_exit_cfg)
{
    EXTI_InitTypeDef   EXTI_InitStructure;

    hk_exit_cfg *p_hk_exit_cfg = (hk_exit_cfg *)p_exit_cfg->p_exit_cfg;

    EXTI_InitStructure.EXTI_Line = p_hk_exit_cfg->exit_line;  
    EXTI_InitStructure.EXTI_Mode = p_hk_exit_cfg->exit_mode;
    EXTI_InitStructure.EXTI_Trigger = p_hk_exit_cfg->exit_trigger;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    return 0;
}

int hk_exit_disable(exit_cfg_t *p_exit_cfg)
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


void exit0_irq_handler(exit_cfg_t *p_exit_cfg)
{
    hk_exit_cfg *p_hk_exit_cfg = (hk_exit_cfg *)g_exit0_obj.exit_cfg.p_exit_cfg;
    hk_exit_pin_cfg *p_hk_exit_pin_cfg = (hk_exit_pin_cfg *)g_exit0_obj.exit_cfg.p_pin_cfg;
    gpio_object_t *p_exit_gpio = p_hk_exit_pin_cfg->exit_gpio_cfg;
    uint8_t keyval = 0;
    static bool led_status = true;

    p_exit_cfg->delay_ms(2);
    p_exit_gpio->gpio_ops.gpio_input_get(&p_exit_gpio->gpio_cfg, &keyval);

    if (keyval == 1)
    {
        // led_status = !led_status;
        // g_led_obj.gpio_ops.gpio_output_set(&g_led_obj.gpio_cfg, led_status);
        p_hk_exit_pin_cfg->press_cnt++;
    }
    EXTI_ClearITPendingBit(p_hk_exit_cfg->exit_line);
}

void EXTI0_IRQHandler(void)
{
    hk_exit_cfg *p_hk_exit_cfg = (hk_exit_cfg *)g_exit0_obj.exit_cfg.p_exit_cfg;
    g_exit0_obj.exit_ops.exit_irq_cb(&g_exit0_obj.exit_cfg);
}

void exit1_irq_handler(exit_cfg_t *p_exit_cfg)
{
    hk_exit_cfg *p_hk_exit_cfg = (hk_exit_cfg *)g_exit1_obj.exit_cfg.p_exit_cfg;
    hk_exit_pin_cfg *p_hk_exit_pin_cfg = (hk_exit_pin_cfg *)g_exit1_obj.exit_cfg.p_pin_cfg;
    gpio_object_t *p_exit_gpio = p_hk_exit_pin_cfg->exit_gpio_cfg;
    uint8_t keyval = 0;

    p_exit_cfg->delay_ms(2);
    p_exit_gpio->gpio_ops.gpio_input_get(&p_exit_gpio->gpio_cfg, &keyval);

    if (keyval == 1)
    {
        p_hk_exit_pin_cfg->press_cnt++;
    }
    EXTI_ClearITPendingBit(p_hk_exit_cfg->exit_line);
}

void EXTI1_IRQHandler(void)
{
    hk_exit_cfg *p_hk_exit_cfg = (hk_exit_cfg *)g_exit1_obj.exit_cfg.p_exit_cfg;
    g_exit1_obj.exit_ops.exit_irq_cb(&g_exit1_obj.exit_cfg);
}

void exit13_irq_handler(exit_cfg_t *p_exit_cfg)
{
    hk_exit_cfg *p_hk_exit_cfg = (hk_exit_cfg *)g_exit13_obj.exit_cfg.p_exit_cfg;
    hk_exit_pin_cfg *p_hk_exit_pin_cfg = (hk_exit_pin_cfg *)g_exit13_obj.exit_cfg.p_pin_cfg;
    gpio_object_t *p_exit_gpio = p_hk_exit_pin_cfg->exit_gpio_cfg;
    uint8_t keyval = 0;
    static bool led_status = true;

    p_exit_cfg->delay_ms(2);
    p_exit_gpio->gpio_ops.gpio_input_get(&p_exit_gpio->gpio_cfg, &keyval);

    if (keyval == 1)
    {
        led_status = !led_status;
        g_led_obj.gpio_ops.gpio_output_set(&g_led_obj.gpio_cfg, led_status);
    }
    EXTI_ClearITPendingBit(p_hk_exit_cfg->exit_line);
}

void EXTI15_10_IRQHandler(void)
{
    
    hk_exit_cfg *p_hk_exit_cfg = (hk_exit_cfg *)g_exit13_obj.exit_cfg.p_exit_cfg;
    if (EXTI_GetITStatus(p_hk_exit_cfg->exit_line) != RESET)
    {
        g_exit13_obj.exit_ops.exit_irq_cb(&g_exit13_obj.exit_cfg);
    }
}
