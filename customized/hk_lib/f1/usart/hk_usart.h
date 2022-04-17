#ifndef __HK_USART_H
#define __HK_USART_H

#include "stdbool.h"
#include "usart.h"

typedef enum 
{
    HK_UART_1 = 0, ///< UART 1.
    HK_UART_2,     ///< UART 2.
    HK_UART_3,     ///< UART 3.
    HK_UART_4,     ///< UART 4.
    HK_UART_5,     ///< UART 5.
    HK_UART_MAX
}hk_uart_id_t;

typedef struct
{
    USART_TypeDef *uart;

    GPIO_TypeDef *tx_port;

    GPIO_TypeDef *rx_port;

    uint32_t clk;
    uint32_t tx_clk;
    uint32_t rx_clk;

    uint16_t tx_pin;
    uint16_t rx_pin;

    uint8_t uart_id;
} hk_uart_info_t;

/**
 * @brief Structure for UART configuration.
 * 
 */
typedef struct
{
    // void *p_context; ///< Context passed to interrupt handler.
    uint32_t baudrate; ///< Baudrate configuration.
    uint32_t databits; ///< Data bits for one character configuration.
    uint32_t stopbit;  ///< Stopbit configuration.
    uint32_t parity;   ///< Parity configuration.
    uint32_t hwfc;     ///< Flow control configuration.
    uint32_t mode;
    uint8_t interrupt_priority; ///< Interrupt priority.
} hk_uart_config_t;

void hk_uart_init(hk_uart_info_t uart_info, hk_uart_config_t const *p_config);
void hk_uart_put(hk_uart_info_t uart_info, char ch);
char hk_uart_get(hk_uart_info_t uart_info);

int  hk_uart_obj_init(usart_cfg_t *p_uart_cfg);
void hk_uart_obj_put(char ch);
void hk_uart_obj_get(char *ch); 

void hk_uart_obj_putstring(const char *s);

void hk_uart_obj_puts(char *ch, unsigned char len);
void hk_uart_obj_gets(char *ch, unsigned char len);

#endif
