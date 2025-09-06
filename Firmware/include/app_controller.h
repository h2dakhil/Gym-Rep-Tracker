#ifndef APP_CONTROLLER_H
#define APP_CONTROLLER_H

#include "app_state.h"
#include "exercise_config.h"
#include "mpu6050.h"
#include "imu_filters.h"
#include "rep_detect.h"
#include "ui_oled.h"

// Application configuration
#define EXERCISE_BICEP_CURL 1  // Currently only bicep curl is implemented

typedef struct {
    AppState_t current_state;
    exercise_t current_exercise;
    uint32_t state_start_time_ms;
    uint32_t last_imu_sample_time_ms;
    uint32_t last_ui_update_time_ms;
    uint16_t rep_count;
    bool rep_detected;
    uint32_t exercise_select_time_ms;
} AppControllerState_t;

/**
 * @brief Initializes the application controller.
 */
void app_controller_init(void);

/**
 * @brief Main application loop - should be called continuously.
 */
void app_controller_loop(void);

/**
 * @brief Gets the current application state.
 * @param state Pointer to AppControllerState_t to fill with current state.
 */
void app_controller_get_state(AppControllerState_t *state);

/**
 * @brief Resets the application to boot state.
 */
void app_controller_reset(void);

/**
 * @brief Gets the current rep count for the current exercise.
 * @retval uint16_t Current rep count.
 */
uint16_t app_controller_get_rep_count(void);

/**
 * @brief Gets the current exercise type.
 * @retval exercise_t Current exercise type.
 */
exercise_t app_controller_get_current_exercise(void);

#endif // APP_CONTROLLER_H
