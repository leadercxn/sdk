#include <stdio.h>
#include "stdbool.h"
#include "usart.h"

#if !defined(__ICCARM__)
struct __FILE
{
  int handle;
};
#endif

FILE __stdout;
FILE __stdin;


#if defined(__CC_ARM) || defined(__ICCARM__)  //armcc
// int fgetc(FILE *p_file)
// {
//   uint8_t input;
//   input = uart_get();
//   return input;
// }

int fputc(int ch, FILE *p_file)
{
  UNUSED_PARAMETER(p_file);

  if((&g_usart_object) && (g_usart_object.usart_ops.usart_put))
  {
      g_usart_object.usart_ops.usart_put((uint8_t)ch)); 
  }

  return ch;
}

#elif defined(__GNUC__) //gcc


int _write(int file, const char *p_char, int len)
{
  int i;

  if(g_usart_object.usart_ops.usart_write)
  {
    for (i = 0; i < len; i++)
    {
      g_usart_object.usart_ops.usart_write(*p_char++);       
    }
  }

  return len;
}

// int _read(int file, char *p_char, int len)
// {
//   UNUSED_PARAMETER(file);
//   uart_get((uint8_t *)p_char) return 1;
// }
// #endif

// #if defined(__ICCARM__)

// __ATTRIBUTES size_t __write(int file, const unsigned char *p_char, size_t len)
// {
//   int i;

//   UNUSED_PARAMETER(file);

//   for (i = 0; i < len; i++)
//   {
//     UNUSED_VARIABLE(uart_put(*p_char++));
//   }
//   return len;
// }

#endif
