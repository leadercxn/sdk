#ifndef __STM8_USART_H
#define __STM8_USART_H

#include "usart.h"
#include "stm8s.h"
#include "stm8s_uart2.h"


/**
 * @brief Structure for UART configuration.
 * 
 */
typedef struct
{
    // void *p_context; ///< Context passed to interrupt handler.
    uint32_t baudrate; ///< Baudrate configuration.
    uint8_t  databits; ///< Data bits for one character configuration.
    uint8_t  stopbit;  ///< Stopbit configuration.
    uint8_t  parity;   ///< Parity configuration.
    uint8_t  mode;
    uint8_t  syncmode;
} stm8_uart_config_t;

/*
int  hk_uart_obj_init(usart_cfg_t *p_uart_cfg);
void hk_uart_obj_put(uint8_t ch);
uint8_t hk_uart_obj_get(uint8_t *ch);

void hk_uart_obj_puts(uint8_t *ch, uint16_t len);
uint16_t hk_uart_obj_gets(uint8_t *ch, uint16_t len);

void hk_uart_obj_putstring(const char *s);
*/
int     stm8_uart_obj_init(usart_cfg_t *p_uart_cfg);
void    stm8_uart_obj_put(uint8_t ch);
uint8_t stm8_uart_obj_get(uint8_t *ch);
void        stm8_uart_obj_puts(uint8_t *ch, uint16_t len);
uint16_t    stm8_uart_obj_gets(uint8_t *ch, uint16_t len);
void        stm8_uart_obj_putstring(const char *s);

#endif
