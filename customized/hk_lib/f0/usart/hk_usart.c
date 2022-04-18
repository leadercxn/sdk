#include "stdbool.h"
#include "util.h"

#include "boards.h"
#include "hk_usart.h"

void hk_uart_init(hk_uart_info_t uart_info, hk_uart_config_t const *p_config)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStruct;

    /* Enable GPIO clock */
    RCC_AHBPeriphClockCmd(uart_info.tx_clk | uart_info.rx_clk, ENABLE);

    /* Enable USART clock 不同串口不同的外设时钟 */
    //RCC_APB1PeriphClockCmd(uart_info.clk, ENABLE);

    RCC_APB2PeriphClockCmd(uart_info.clk, ENABLE);

    /* Connect PXx to USARTx_Tx */
    GPIO_PinAFConfig(uart_info.tx_port, uart_info.tx_pin_source, uart_info.tx_af);

    /* Connect PXx to USARTx_Rx */
    GPIO_PinAFConfig(uart_info.rx_port, uart_info.rx_pin_source, uart_info.rx_af);

    /* Configure USART Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = uart_info.tx_pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(uart_info.tx_port, &GPIO_InitStructure);

    /* Configure USART Rx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = uart_info.rx_pin;
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
    USART_SendData(uart_info.uart, tx_data);

    /* Loop until transmit data register is empty */
//  while (USART_GetFlagStatus(uart_info.uart, USART_FLAG_TXE) == RESET)  //也可以
    while (USART_GetFlagStatus(uart_info.uart, USART_FLAG_TC) == RESET)
    {
    }
}

/**
 * @brief 获取uart的rx数据
 * 
 * @return 返回数据长度, 0 无数据
 *                     1 读到数据
 */
uint8_t hk_uart_get(hk_uart_info_t uart_info, uint8_t *p_rx_data)
{
/**
 * 弃用，不要阻塞式等待获取参数
 */
#if 0

    /* Loop until receive data register is not empty */
    while (USART_GetFlagStatus(uart_info.uart, USART_FLAG_RXNE) == RESET)
    {
    }

    /* Place your implementation of fgetc here */
    /* e.g. write a character to the USART */
    *p_rx_data = (uint8_t)USART_ReceiveData(uart_info.uart);

    return 1;
#endif

#if 1
    /**
     * 获得rx参数
     */
    if(USART_GetFlagStatus(uart_info.uart, USART_FLAG_RXNE) == SET)
    {
        *p_rx_data = (uint8_t)USART_ReceiveData(uart_info.uart);
         
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

void hk_uart_obj_putstring(const char *s)
{
    while (*s != '\0')
    {
        hk_uart_obj_put(*s++);
    }
}
