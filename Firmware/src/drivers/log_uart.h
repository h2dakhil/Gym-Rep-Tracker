#ifndef LOG_UART_H
#define LOG_UART_H

#include "stm32f1xx_hal.h"

#ifdef ENABLE_LOG_UART

/**
 * @brief Initializes the UART for logging.
 * @retval HAL_StatusTypeDef HAL_OK if successful, HAL_ERROR otherwise.
 */
HAL_StatusTypeDef log_uart_init(void);

/**
 * @brief Sends a formatted string over UART.
 * @param format Format string (similar to printf).
 * @param ... Variable arguments.
 * @retval int Number of characters sent.
 */
int log_uart_printf(const char* format, ...);

/**
 * @brief Sends a string over UART.
 * @param str String to send.
 * @retval HAL_StatusTypeDef HAL_OK if successful, HAL_ERROR otherwise.
 */
HAL_StatusTypeDef log_uart_send_string(const char* str);

#else

// No-op macros when UART logging is disabled
#define log_uart_init() HAL_OK
#define log_uart_printf(...) 0
#define log_uart_send_string(str) HAL_OK

#endif // ENABLE_LOG_UART

#endif // LOG_UART_H

