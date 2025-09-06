#ifndef LOG_MACRO_H
#define LOG_MACRO_H

#ifdef ENABLE_LOG_UART
#include "log_uart.h"
#define LOG(fmt, ...) log_uart_printf(fmt, ##__VA_ARGS__)
#else
#define LOG(fmt, ...) ((void)0)
#endif

#endif // LOG_MACRO_H

