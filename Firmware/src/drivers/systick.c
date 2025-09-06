#include "systick.h"

static volatile uint32_t systick_counter = 0;

/**
 * @brief Systick interrupt handler.
 */
void SysTick_Handler(void)
{
    systick_counter++;
    HAL_IncTick();
}

/**
 * @brief Initializes the system tick timer.
 */
void systick_init(void)
{
    // Systick is already configured by HAL_Init()
    // This function is provided for future extensions if needed
}

/**
 * @brief Gets the current uptime in milliseconds.
 */
uint32_t systick_get_uptime_ms(void)
{
    return systick_counter;
}

/**
 * @brief Delays execution for a specified number of milliseconds.
 */
void systick_delay_ms(uint32_t ms)
{
    uint32_t start = systick_counter;
    while ((systick_counter - start) < ms)
    {
        // Wait for the specified time
    }
}

/**
 * @brief Checks if a specified time interval has elapsed.
 */
bool systick_has_elapsed(uint32_t start_time_ms, uint32_t interval_ms)
{
    uint32_t current_time = systick_counter;
    uint32_t elapsed = current_time - start_time_ms;
    
    if (current_time >= start_time_ms)
    {
        return elapsed >= interval_ms;
    }
    else
    {
        // Handle counter overflow
        return (0xFFFFFFFF - start_time_ms + current_time + 1) >= interval_ms;
    }
}

