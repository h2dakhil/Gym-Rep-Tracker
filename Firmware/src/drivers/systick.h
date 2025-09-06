#ifndef SYSTICK_H
#define SYSTICK_H

#include "stm32f1xx_hal.h"
#include <stdint.h>

// Define boolean type if not available
#ifndef bool
typedef uint8_t bool;
#define true 1
#define false 0
#endif

/**
 * @brief Initializes the system tick timer.
 */
void systick_init(void);

/**
 * @brief Gets the current uptime in milliseconds.
 * @retval uint32_t Current uptime in milliseconds.
 */
uint32_t systick_get_uptime_ms(void);

/**
 * @brief Delays execution for a specified number of milliseconds.
 * @param ms Number of milliseconds to delay.
 */
void systick_delay_ms(uint32_t ms);

/**
 * @brief Checks if a specified time interval has elapsed.
 * @param start_time_ms Start time in milliseconds.
 * @param interval_ms Interval to check in milliseconds.
 * @retval bool True if interval has elapsed, false otherwise.
 */
bool systick_has_elapsed(uint32_t start_time_ms, uint32_t interval_ms);

#endif // SYSTICK_H

