#include "boards.h"

#include "hk_gpio.h"
#include "util.h"

/**
 * @param  ahbperiph: specifies the AHB peripheral to gates its clock.
 *          This parameter can be any combination of the following values:
 *             @arg RCC_AHBPeriph_GPIOA: GPIOA clock
 *             @arg RCC_AHBPeriph_GPIOB: GPIOB clock
 *             @arg RCC_AHBPeriph_GPIOC: GPIOC clock
 *             @arg RCC_AHBPeriph_GPIOD: GPIOD clock
 *             @arg RCC_AHBPeriph_GPIOE: GPIOE clock (no support)
 *             @arg RCC_AHBPeriph_GPIOF: GPIOF clock
 * 
 * @param  gpio_port: 
 *             @arg GPIOA
 *             @arg GPIOB
 *             @arg GPIOC
 *             @arg GPIOD
 *             @arg GPIOF
 * 
 * @param  gpio_pin:
 *             @arg GPIO_Pin_0
 *             @arg GPIO_Pin_1
 *             @arg ...
 *
 */
void conf_gpio_output(uint32_t ahbperiph, GPIO_TypeDef *gpio_port, uint32_t gpio_pin)
{
    GPIO_InitTypeDef        GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(ahbperiph, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = gpio_pin;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(gpio_port, &GPIO_InitStructure);
}

/**
 * @brief 设置引脚电平
 * 
 * @param  gpio_pin:
 *             @arg GPIO_Pin_0
 *             @arg GPIO_Pin_1
 *             @arg ...
 */
void set_gpio_value(GPIO_TypeDef *gpio_port , uint32_t gpio_pin ,uint8_t value)
{
    if(value)
    {
        gpio_port->BSRR = gpio_pin;
    }
    else
    {
        gpio_port->BRR = gpio_pin;
    }
}

/**
 * @param  ahbperiph: specifies the AHB peripheral to gates its clock.
 *          This parameter can be any combination of the following values:
 *             @arg RCC_AHBPeriph_GPIOA: GPIOA clock
 *             @arg RCC_AHBPeriph_GPIOB: GPIOB clock
 *             @arg RCC_AHBPeriph_GPIOC: GPIOC clock
 *             @arg RCC_AHBPeriph_GPIOD: GPIOD clock
 *             @arg RCC_AHBPeriph_GPIOE: GPIOE clock (no support)
 *             @arg RCC_AHBPeriph_GPIOF: GPIOF clock
 * 
 * @param  gpio_port: 
 *             @arg GPIOA
 *             @arg GPIOB
 *             @arg GPIOC
 *             @arg GPIOD
 *             @arg GPIOF
 * 
 * @param  gpio_pin:
 *             @arg GPIO_Pin_0
 *             @arg GPIO_Pin_1
 *             @arg ...
 *
 */
void conf_gpio_input(uint32_t ahbperiph, GPIO_TypeDef *gpio_port, uint32_t gpio_pin)
{
    GPIO_InitTypeDef        GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(ahbperiph, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = gpio_pin;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;

    GPIO_Init(gpio_port, &GPIO_InitStructure);
}

/**
 * @brief 获取引脚电平
 * 
 * @param  gpio_pin:
 *             @arg GPIO_Pin_0
 *             @arg GPIO_Pin_1
 *             @arg ...
 */
void get_gpio_value(GPIO_TypeDef *gpio_port , uint32_t gpio_pin ,uint8_t *p_value)
{
    *p_value = (gpio_port->IDR & gpio_pin) ? 1 : 0;
}


/**
 * @brief 配置整组IO为输出模式
 * 
 * @param  gpio_pins: 对应引脚的bit位要置1
 *           
 */
void conf_whole_gpios_output(uint32_t ahbperiph, GPIO_TypeDef *gpio_port, uint16_t gpio_pins)
{
    GPIO_InitTypeDef        GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(ahbperiph, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = gpio_pins;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(gpio_port, &GPIO_InitStructure);
}

/**
 * @brief 设置半组IO输出
 * 
 * @param lsb:  true 低8位     false: 高8位
 * 
 */
void set_halt_gpios_value(GPIO_TypeDef *gpio_port ,uint8_t value, bool lsb)
{
    if(lsb)
    {
        gpio_port->BSRR = value;
        gpio_port->BRR = ~value;
    }
    else
    {
        uint16_t msb_value = 0;

        msb_value = (uint16_t)(value << 8);

        gpio_port->BSRR = msb_value;

        msb_value = ~msb_value;
        msb_value &= 0xFF00;

        gpio_port->BRR = msb_value;
    }
}

/**
 * @brief 配置整组IO为输入模式
 * 
 * @param  gpio_pins: 对应引脚的bit位要置1
 *           
 */
void conf_whole_gpios_input(uint32_t ahbperiph, GPIO_TypeDef *gpio_port, uint16_t gpio_pins)
{
    GPIO_InitTypeDef        GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(ahbperiph, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = gpio_pins;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;

    GPIO_Init(gpio_port, &GPIO_InitStructure);
}

/**
 * @brief 获取半组IO输出
 * 
 * @param lsb:  true 低8位     false: 高8位
 */
void get_halt_gpios_value(GPIO_TypeDef *gpio_port ,bool lsb, uint8_t *p_value)
{
    uint16_t get_value = 0;
    get_value = gpio_port->IDR;

    if(!lsb)
    {
        get_value = get_value >> 8;
    }

    *p_value = (uint8_t)get_value;
}


/**
 * @brief 输出模式转为输入
 */
void output_shift_to_input(GPIO_TypeDef *gpio_port, uint16_t gpio_pin)
{
    GPIO_InitTypeDef        GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin   = gpio_pin;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;

    GPIO_Init(gpio_port, &GPIO_InitStructure);
}

/**
 * @brief 输入模式转为输出
 */
void input_shift_to_output(GPIO_TypeDef *gpio_port, uint16_t gpio_pin)
{
    GPIO_InitTypeDef        GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin   = gpio_pin;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(gpio_port, &GPIO_InitStructure);
}

/**
 * @brief 配置GPIO复用
 */
void conf_gpio_af(uint32_t ahbperiph, GPIO_TypeDef *gpio_port, uint16_t gpio_pins)
{
    GPIO_InitTypeDef        GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(ahbperiph, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = gpio_pins;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(gpio_port, &GPIO_InitStructure);
}

/**
 * @brief 配置整组GPIO复用
 */
void conf_whole_gpios_af(uint32_t ahbperiph, GPIO_TypeDef *gpio_port, uint16_t gpio_pins)
{
    GPIO_InitTypeDef        GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(ahbperiph, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = gpio_pins;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(gpio_port, &GPIO_InitStructure);
}

/**
 * @brief 对gpio对象的初始化
 */
int hk_gpio_obj_init(gpio_cfg_t *p_cfg)
{
    if(p_cfg)
    {
        if (p_cfg->flag == GPIO_TYPE_IO)
        {
            if(p_cfg->gpio_dir == GPIO_DIR_OUTPUR)
            {
                conf_gpio_output(p_cfg->gpio_clk, (GPIO_TypeDef *)p_cfg->p_port, p_cfg->gpio_pin);
            }
            else
            {
                conf_gpio_input(p_cfg->gpio_clk, (GPIO_TypeDef *)p_cfg->p_port, p_cfg->gpio_pin);
            }
        }
        else
        {
            conf_gpio_af(p_cfg->gpio_clk, (GPIO_TypeDef *)p_cfg->p_port, p_cfg->gpio_pin);
        }
    }

    return 0;
}

/**
 * @brief 对gpio对象的输出设置
 */
int hk_gpio_obj_out_set(gpio_cfg_t *p_cfg, uint8_t value)
{
    if(p_cfg)
    {
        set_gpio_value((GPIO_TypeDef *)p_cfg->p_port, p_cfg->gpio_pin , value);
    }

    return 0;
}

/**
 * @brief 获取gpio输入
 */
int hk_gpio_obj_in_get(gpio_cfg_t *p_cfg, uint8_t *p_value)
{
    if(p_cfg)
    {
        get_gpio_value((GPIO_TypeDef *)p_cfg->p_port, p_cfg->gpio_pin , p_value);
    }

    return 0;
}

/**
 * @brief 固定gpio对象为输入模式
 */
int hk_gpio_fix_input(gpio_cfg_t *p_cfg)
{
    GPIO_InitTypeDef        GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin   = p_cfg->gpio_pin;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;

    GPIO_Init((GPIO_TypeDef *)p_cfg->p_port, &GPIO_InitStructure);

    return 0;
}

/**
 * @brief 固定gpio对象为输出模式
 */
int hk_gpio_fix_output(gpio_cfg_t *p_cfg)
{
    GPIO_InitTypeDef        GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin   = p_cfg->gpio_pin;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init((GPIO_TypeDef *)p_cfg->p_port, &GPIO_InitStructure);

    return 0;
}
