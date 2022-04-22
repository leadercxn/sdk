#include "stdbool.h"
#include "util.h"

#include "boards.h"
#include "hk_usart.h"

static USART_TypeDef *m_usart_array[HK_UART_MAX] = 
{USART1, USART2, USART3, UART4, UART5};

void hk_uart_init(hk_uart_info_t uart_info, hk_uart_config_t const *p_config)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStruct;

    /* Enable GPIO clock */
    RCC_APB2PeriphClockCmd(uart_info.tx_clk | uart_info.rx_clk, ENABLE);

    /* Enable USART clock 不同串口不同的外设时钟 */
    if (HK_UART_1 == uart_info.uart_id)
    {
        RCC_APB2PeriphClockCmd(uart_info.clk, ENABLE);
    }
    else
    {
        RCC_APB1PeriphClockCmd(uart_info.clk, ENABLE);
    }

    /* Configure USART Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = uart_info.tx_pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(uart_info.tx_port, &GPIO_InitStructure);

    /* Configure USART Rx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = uart_info.rx_pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(uart_info.rx_port, &GPIO_InitStructure);

    /* USART configuration */
    USART_InitStruct.USART_BaudRate = p_config->baudrate;
    USART_InitStruct.USART_WordLength = p_config->databits;
    USART_InitStruct.USART_StopBits = p_config->stopbit;
    USART_InitStruct.USART_Parity = p_config->parity;
    USART_InitStruct.USART_Mode = p_config->mode;
    USART_InitStruct.USART_HardwareFlowControl = p_config->hwfc;

    USART_Init(uart_info.uart, &USART_InitStruct);

    /* Enable USART */
    USART_Cmd(uart_info.uart, ENABLE);
}

void hk_uart_put(hk_uart_info_t uart_info, uint8_t tx_data)
{
    USART_SendData(m_usart_array[uart_info.uart_id], tx_data);

    /* Loop until transmit data register is empty */
    // while (USART_GetFlagStatus(m_usart_array[uart_info.uart_id], USART_FLAG_TXE) == RESET)
    while (USART_GetFlagStatus(uart_info.uart, USART_FLAG_TC) == RESET)
    {
    }
}

uint8_t hk_uart_get(hk_uart_info_t uart_info, uint8_t *p_rx_data)
{
#if 0
    // 阻塞方式
    char data;

    /* Loop until receive data register is not empty */
    while (USART_GetFlagStatus(m_usart_array[uart_info.uart_id], USART_FLAG_RXNE) == RESET)
    {
    }

    /* Place your implementation of fgetc here */
    /* e.g. write a character to the USART */
    *p_rx_data = (uint8_t)USART_ReceiveData(m_usart_array[uart_info.uart_id]);
#else
    // 非阻塞方式
    if(USART_GetFlagStatus(uart_info.uart, USART_FLAG_RXNE) == SET)
    {
        *p_rx_data = (uint8_t)USART_ReceiveData(uart_info.uart);
        // USART_ClearFlag(uart_info.uart, USART_FLAG_RXNE);
         
        return 1;
    }

    return 0;
#endif
}



int hk_uart_obj_init(usart_cfg_t *p_uart_cfg)
{
    hk_uart_info_t *p_hk_uart_info = (hk_uart_info_t *)p_uart_cfg->p_pin_cfg;
    hk_uart_config_t *p_hk_uart_config = (hk_uart_config_t *)p_uart_cfg->p_usart_cfg;

    hk_uart_init( *(hk_uart_info_t *)p_hk_uart_info, p_hk_uart_config);

    return 0;
}

void hk_uart_obj_put(uint8_t ch)
{
    hk_uart_put(*(hk_uart_info_t *)g_usart_object.usart_cfg.p_pin_cfg, ch);
}

uint8_t hk_uart_obj_get(uint8_t *ch)
{
    return hk_uart_get(*(hk_uart_info_t *)g_usart_object.usart_cfg.p_pin_cfg, ch);
}

void hk_uart_obj_putstring(const char *s)
{
    while (*s != '\0')
    {
        hk_uart_obj_put(*s++);
    }
}

void hk_uart_obj_puts(uint8_t *ch, uint16_t len)
{
    uint16_t i = 0;
    
    for (i = 0; i < len; i++)
    {
        hk_uart_obj_put(ch[i]);
    }
}

/**
 * @return 返回实际读取到的长度
 */
uint16_t hk_uart_obj_gets(uint8_t *ch, uint16_t len)
{
    uint16_t i = 0;
    uint16_t rx_len = 0;
    
    for (i = 0; i < len; i++)
    {
        if(hk_uart_obj_get(&ch[rx_len]))
        {
            rx_len ++;
        }
    }

    return rx_len;
}
