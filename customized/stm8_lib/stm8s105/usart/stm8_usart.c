#include "stdbool.h"
#include "stm8_usart.h"
#include "util.h"


int  stm8_uart_obj_init(usart_cfg_t *p_uart_cfg)
{
    stm8_uart_config_t *p_stm8_uart_config = (stm8_uart_config_t *)p_uart_cfg->p_usart_cfg;

    UART2_Init( p_stm8_uart_config->baudrate, 
                p_stm8_uart_config->databits,
                p_stm8_uart_config->stopbit,
                p_stm8_uart_config->parity,
                p_stm8_uart_config->syncmode,
                p_stm8_uart_config->mode);
    UART2_Cmd(ENABLE);

    return 0;
}

void stm8_uart_obj_put(uint8_t ch)
{
    UART2_SendData8(ch);
    while( ! UART2_GetFlagStatus(UART2_FLAG_TC) );
}

uint8_t stm8_uart_obj_get(uint8_t *ch)
{
    if( UART2_GetFlagStatus(UART2_FLAG_RXNE) )
    {
        *ch = UART2_ReceiveData8();
        return 1;
    }

    return 0;
}

void stm8_uart_obj_puts(uint8_t *ch, uint16_t len)
{
    uint16_t i = 0;
    
    for (i = 0; i < len; i++)
    {
        stm8_uart_obj_put(ch[i]);
    }
}

/**
 * @return 返回实际读取到的长度
 */
uint16_t stm8_uart_obj_gets(uint8_t *ch, uint16_t len)
{
    uint16_t i = 0;
    uint16_t rx_len = 0;
    
    for (i = 0; i < len; i++)
    {
        if(stm8_uart_obj_get(&ch[rx_len]))
        {
            rx_len ++;
        }
    }

    return rx_len;
}

void stm8_uart_obj_putstring(const char *s)
{
    while (*s != '\0')
    {
        stm8_uart_obj_put(*s++);
    }
}
