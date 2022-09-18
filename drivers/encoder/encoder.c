#include "encoder.h"
#include "trace.h"
#include "hk_exit.h"
#include "hk_peripheral.h"

void encoder_init(encoder_object_t *p_encoder_obj)
{
    p_encoder_obj->pin_sw->exit_ops.exit_init(&p_encoder_obj->pin_sw->exit_cfg);
    p_encoder_obj->pin_exit->exit_ops.exit_init(&p_encoder_obj->pin_exit->exit_cfg);
    p_encoder_obj->pin_gpio->gpio_ops.gpio_init(&p_encoder_obj->pin_gpio->gpio_cfg);

    p_encoder_obj->step = 0;
    p_encoder_obj->dir = 0;

    trace_info("encoder init is ok\r\n");
}


void encoder_sw_handler(exit_cfg_t *p_exit_cfg)
{
    hk_exit_cfg *p_hk_exit_cfg = (hk_exit_cfg *)p_exit_cfg->p_exit_cfg;
    hk_exit_pin_cfg *p_hk_exit_pin_cfg = (hk_exit_pin_cfg *)p_exit_cfg->p_pin_cfg;
    gpio_object_t *p_exit_gpio = p_hk_exit_pin_cfg->exit_gpio_cfg;
    uint8_t keyval = 0;

    p_exit_cfg->delay_ms(2);
    p_exit_gpio->gpio_ops.gpio_input_get(&p_exit_gpio->gpio_cfg, &keyval);

    if (keyval == 1)
    {
        g_encoder_obj.pressed = 1;
        trace_info("encoder pressed.\r\n");
    }
    EXTI_ClearITPendingBit(p_hk_exit_cfg->exit_line);
}

void encoder_data_handler(exit_cfg_t *p_exit_cfg)
{
    uint8_t keyvalA = 0, keyvalB = 0;

    hk_exit_cfg *p_hk_exit_cfg = (hk_exit_cfg *)p_exit_cfg->p_exit_cfg;
    hk_exit_pin_cfg *p_hk_exit_pin_cfg = (hk_exit_pin_cfg *)p_exit_cfg->p_pin_cfg;
    gpio_object_t *p_exit_gpio = p_hk_exit_pin_cfg->exit_gpio_cfg;

    gpio_object_t *p_gpio = g_encoder_obj.pin_gpio;

    // p_exit_cfg->delay_ms(2);
    p_exit_gpio->gpio_ops.gpio_input_get(&p_exit_gpio->gpio_cfg, &keyvalA);
    p_gpio->gpio_ops.gpio_input_get(&p_gpio->gpio_cfg, &keyvalB);

#if 0       // 滤波更强
    static bool flag = 0;
    static bool CW_1 = 0, CW_2 = 0;

    if (!flag && !keyvalA)
    {
        CW_1 = keyvalB;
        flag = 1;
    }

    if (flag && keyvalA)
    {
        CW_2 = !keyvalB;
        if (CW_1 && CW_2)
        {
            g_encoder_obj.dir = 2;
        }
        else if (!CW_1 && !CW_2)
        {
            g_encoder_obj.dir = 1;
        }
        flag = 0;
    }
#else
    if (keyvalB == 1)
    {
        g_encoder_obj.dir = 1;
    }
    else if (keyvalB == 0)
    {
        g_encoder_obj.dir = 2;
    }
#endif

    EXTI_ClearITPendingBit(p_hk_exit_cfg->exit_line);
}
