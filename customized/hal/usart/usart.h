#ifndef __USART_H
#define __USART_H

#include <stdint.h>

typedef struct
{
    void        *p_pin_cfg;
    void        *p_usart_cfg;
    uint32_t    flag;
} usart_cfg_t;

typedef struct
{
    int     (* usart_init)  (usart_cfg_t *p_cfg);
#if defined(__CC_ARM) || defined(__ICCARM__)    //armcc
    void    (* usart_put)   (char ch);
    uint8_t (* usart_get)   (void);
#elif defined(__GNUC__)                         //gcc
    void    (* usart_write) (char ch);
    void    (* usart_read)  (char *ch);
    void    (* usart_write_buffer)  (char *ch, unsigned char len);
    void    (* usart_read_buffer)  (char *ch, unsigned char len);
    void    (* usart_put_string)   (const char *ch);                              // 专用ntshell输出string的接口
#endif
} usart_ops_t;


typedef struct
{
    usart_cfg_t usart_cfg;
    usart_ops_t usart_ops;
} usart_object_t;

// 工程引用此头文件得定义一个全局变量 g_usart_object
extern usart_object_t g_usart_object;

#endif
