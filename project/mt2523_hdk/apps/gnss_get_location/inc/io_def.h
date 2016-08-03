#ifndef __IO_DEF_H__
#define __IO_DEF_H__


#ifndef __GNUC__
#include <stdlib.h> /* for FILE */
#endif

#include "hal_uart.h"

#ifdef __cplusplus
extern "C" {
#endif


#if !defined(MTK_MAIN_CONSOLE_UART2_ENABLE)
#define CONSOLE_UART    HAL_UART_0
#else
#define CONSOLE_UART    HAL_UART_1
#endif



#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar(void)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */


/*****************************************************************************
 * API Functions
 *****************************************************************************/


PUTCHAR_PROTOTYPE;

GETCHAR_PROTOTYPE;

void io_def_uart_init(void);


#ifdef __cplusplus
}
#endif

#endif /* __IO_DEF_H__ */
