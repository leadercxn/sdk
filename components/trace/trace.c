#include "stdbool.h"
#include "string.h"

#ifdef TRACE_ENABLE

#include "usart.h"
#include "trace.h"

/**
 * @brief Definition for COM port1, connected to USART2
 */
void trace_init(void)
{
    if((&g_usart_object) && (g_usart_object.usart_ops.usart_init))
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
