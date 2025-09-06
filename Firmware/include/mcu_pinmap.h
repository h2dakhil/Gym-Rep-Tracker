#ifndef MCU_PINMAP_H
#define MCU_PINMAP_H

#include "stm32f1xx_hal.h"

// TODO: Adjust I2C and UART pin assignments if your PCB differs.
// The following assignments are common for STM32F103CB.

// I2C1 Pins
#define I2C1_SCL_PIN        GPIO_PIN_6
#define I2C1_SCL_GPIO_PORT  GPIOB
#define I2C1_SDA_PIN        GPIO_PIN_7
#define I2C1_SDA_GPIO_PORT  GPIOB

extern I2C_HandleTypeDef hi2c1;

// Optional UART2 Pins for logging
#ifdef ENABLE_LOG_UART
#define UART2_TX_PIN        GPIO_PIN_2
#define UART2_TX_GPIO_PORT  GPIOA
#define UART2_RX_PIN        GPIO_PIN_3
#define UART2_RX_GPIO_PORT  GPIOA

extern UART_HandleTypeDef huart2;
#endif /* ENABLE_LOG_UART */

#endif // MCU_PINMAP_H

