#include "stdbool.h"
#include "string.h"

#ifdef TRACE_ENABLE

#include "usart.h"
#include "trace.h"
/**
 * 
 * @brief 使用此文件必须先提前定义好用做打印串口的 g_usart_object
 */


/**
 * @brief Definition for COM port1, connected to USART2
 */
void trace_init(void)
{
    if(g_usart_object.usart_ops.usart_init)
    {
        g_usart_object.usart_ops.usart_init(&g_usart_object.usart_cfg);
    }
}

void trace_dump(void *p_buffer, uint32_t len)
{
    uint8_t *p = (uint8_t *)p_buffer;

    for (uint32_t index = 0; index < len; index++)
    {
        TRACE_PRINTF("%02X", p[index]);
    }

    TRACE_PRINTF("\r\n");
}

#endif // TRACE_ENABLE
