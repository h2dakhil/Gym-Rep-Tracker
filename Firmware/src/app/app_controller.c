#include "app_controller.h"
#include "app_config.h"
#include "exercise_config.h"
#include "systick.h"
#include "log_uart.h"
#include "mpu6050.h"
#include "imu_filters.h"
#include "rep_detect.h"
#include <string.h>

// Static application state
static AppControllerState_t app_state;

// IMU data structures
static MPU6050_RawData_t imu_raw_data;
static MPU6050_ScaledData_t imu_scaled_data;
static IMUFilteredData_t imu_filtered_data;

// Calibration data
static float calib_rep_signal_sum = 0.0f;
static float calib_rep_signal_sum_sq = 0.0f;
static uint16_t calib_sample_count = 0;

/**
 * @brief Initializes the application controller.
 */
void app_controller_init(void)
{
    // Initialize state
    app_state.current_state = APP_STATE_BOOT;
    app_state.current_exercise = EX_BICEP_CURL; // Default to bicep curl
    app_state.state_start_time_ms = 0;
    app_state.last_imu_sample_time_ms = 0;
    app_state.last_ui_update_time_ms = 0;
    app_state.rep_count = 0;
    app_state.rep_detected = false;
    app_state.exercise_select_time_ms = 0;
    
    // Initialize subsystems
    imu_filters_init();
    rep_detect_init();
    
    // Show splash screen
    ui_show_splash("Gym Rep Tracker");
    
    // Set initial state start time
    app_state.state_start_time_ms = systick_get_uptime_ms();
}

/**
 * @brief Handles the BOOT state.
 */
static void handle_boot_state(void)
{
    // Wait a bit for system stability
    if (systick_has_elapsed(app_state.state_start_time_ms, 1000))
    {
        // Transition to SELECTING_EXERCISE state
        app_state.current_state = APP_STATE_SELECTING_EXERCISE;
        app_state.state_start_time_ms = systick_get_uptime_ms();
        app_state.exercise_select_time_ms = systick_get_uptime_ms();
        
        // Show first exercise selection
        ui_show_select(EX_CFG[app_state.current_exercise].name);
    }
}

/**
 * @brief Handles the SELECTING_EXERCISE state.
 */
static void handle_selecting_exercise_state(void)
{
    uint32_t current_time = systick_get_uptime_ms();
    
    // Auto-advance through exercises every 2 seconds
    if (systick_has_elapsed(app_state.exercise_select_time_ms, EXERCISE_SELECT_TIMEOUT_MS))
    {
        // Move to next exercise
        app_state.current_exercise = (app_state.current_exercise + 1) % EX_COUNT;
        app_state.exercise_select_time_ms = current_time;
        
        // Show new exercise selection
        ui_show_select(EX_CFG[app_state.current_exercise].name);
    }
    
    // After 6 seconds (3 exercises * 2 seconds), proceed to calibration
    if (systick_has_elapsed(app_state.state_start_time_ms, EX_COUNT * EXERCISE_SELECT_TIMEOUT_MS))
    {
        // Transition to CALIBRATING_EXERCISE state
        app_state.current_state = APP_STATE_CALIBRATING_EXERCISE;
        app_state.state_start_time_ms = current_time;
        
        // Show calibration message
        ui_show_calibrating(EX_CFG[app_state.current_exercise].name);
        
        // Begin calibration for selected exercise
        rep_detect_begin_calibration(app_state.current_exercise);
        
        // Reset calibration accumulators
        calib_rep_signal_sum = 0.0f;
        calib_rep_signal_sum_sq = 0.0f;
        calib_sample_count = 0;
    }
}

/**
 * @brief Handles the CALIBRATING_EXERCISE state.
 */
static void handle_calibrating_exercise_state(void)
{
    uint32_t current_time = systick_get_uptime_ms();
    
    // IMU sampling at specified rate during calibration
    if (systick_has_elapsed(app_state.last_imu_sample_time_ms, IMU_SAMPLE_INTERVAL_MS))
    {
        app_state.last_imu_sample_time_ms = current_time;
        
        // Read IMU data
        if (mpu6050_read_raw(&imu_raw_data) == HAL_OK)
        {
            // Convert to scaled values
            mpu6050_convert_to_scaled(&imu_raw_data, &imu_scaled_data);
            
            // Apply filters and compute rep signal
            float dt = (float)IMU_SAMPLE_INTERVAL_MS / 1000.0f;
            imu_filters_process_all(&imu_scaled_data, &imu_filtered_data, dt, app_state.current_exercise);
            
            // Accumulate calibration data
            float rep_signal = imu_filtered_data.curl_axis_scalar;
            calib_rep_signal_sum += rep_signal;
            calib_rep_signal_sum_sq += rep_signal * rep_signal;
            calib_sample_count++;
            
            // Also accumulate in rep detection system
            rep_detect_accumulate_calibration(app_state.current_exercise, rep_signal);
        }
    }
    
    // Wait for calibration duration
    if (systick_has_elapsed(app_state.state_start_time_ms, CALIBRATION_MS_EX))
    {
        // End calibration
        float mu, sigma;
        rep_detect_end_calibration(app_state.current_exercise, &mu, &sigma);
        
        // For bench press, set horizontal reference vector
        if (app_state.current_exercise == EX_BENCH_PRESS)
        {
            vec3_t horizontal_ref = {
                imu_filtered_data.horizontal_vector.x,
                imu_filtered_data.horizontal_vector.y,
                imu_filtered_data.horizontal_vector.z
            };
            imu_filters_set_horizontal_reference(&horizontal_ref);
        }
        
        // Transition to DETECTING state
        app_state.current_state = APP_STATE_DETECTING;
        app_state.state_start_time_ms = current_time;
        
        // Show detecting message
        ui_show_status("Detecting...");
        
        // Reset rep counter
        rep_detect_reset_count(app_state.current_exercise);
        app_state.rep_count = 0;
    }
}

/**
 * @brief Handles the DETECTING state.
 */
static void handle_detecting_state(void)
{
    // Collect IMU samples for warm-up window
    if (systick_has_elapsed(app_state.state_start_time_ms, DETECT_WARMUP_MS))
    {
        // Transition to RUNNING state
        app_state.current_state = APP_STATE_RUNNING;
        app_state.state_start_time_ms = systick_get_uptime_ms();
        
        // Show exercise start message
        ui_show_exercise_and_count(EX_CFG[app_state.current_exercise].name, 0);
    }
}

/**
 * @brief Handles the RUNNING state.
 */
static void handle_running_state(void)
{
    uint32_t current_time = systick_get_uptime_ms();
    
    // IMU sampling at specified rate
    if (systick_has_elapsed(app_state.last_imu_sample_time_ms, IMU_SAMPLE_INTERVAL_MS))
    {
        app_state.last_imu_sample_time_ms = current_time;
        
        // Read IMU data
        if (mpu6050_read_raw(&imu_raw_data) == HAL_OK)
        {
            // Convert to scaled values
            mpu6050_convert_to_scaled(&imu_raw_data, &imu_scaled_data);
            
            // Apply filters
            float dt = (float)IMU_SAMPLE_INTERVAL_MS / 1000.0f;
            imu_filters_process_all(&imu_scaled_data, &imu_filtered_data, dt, app_state.current_exercise);
            
            // Update rep detection
            app_state.rep_detected = rep_detect_update(app_state.current_exercise, 
                                                     imu_filtered_data.curl_axis_scalar, 
                                                     current_time);
            
            if (app_state.rep_detected)
            {
                app_state.rep_count = rep_detect_get_count(app_state.current_exercise);
                app_state.rep_detected = false; // Reset flag
            }
        }
    }
    
    // UI updates at specified rate
    if (systick_has_elapsed(app_state.last_ui_update_time_ms, UI_REFRESH_INTERVAL_MS))
    {
        app_state.last_ui_update_time_ms = current_time;
        
        // Update display with current rep count
        ui_show_exercise_and_count(EX_CFG[app_state.current_exercise].name, app_state.rep_count);
    }
}

/**
 * @brief Main application loop - should be called continuously.
 */
void app_controller_loop(void)
{
    switch (app_state.current_state)
    {
        case APP_STATE_BOOT:
            handle_boot_state();
            break;
            
        case APP_STATE_SELECTING_EXERCISE:
            handle_selecting_exercise_state();
            break;
            
        case APP_STATE_CALIBRATING_EXERCISE:
            handle_calibrating_exercise_state();
            break;
            
        case APP_STATE_DETECTING:
            handle_detecting_state();
            break;
            
        case APP_STATE_RUNNING:
            handle_running_state();
            break;
            
        default:
            // Invalid state, reset to boot
            app_state.current_state = APP_STATE_BOOT;
            app_state.state_start_time_ms = systick_get_uptime_ms();
            break;
    }
}

/**
 * @brief Gets the current application state.
 */
void app_controller_get_state(AppControllerState_t *state)
{
    if (state != NULL)
    {
        *state = app_state;
    }
}

/**
 * @brief Resets the application to boot state.
 */
void app_controller_reset(void)
{
    app_state.current_state = APP_STATE_BOOT;
    app_state.state_start_time_ms = systick_get_uptime_ms();
    app_state.rep_count = 0;
    app_state.rep_detected = false;
    
    // Reset subsystems
    rep_detect_reset_count(app_state.current_exercise);
    
    // Show splash screen
    ui_show_splash("Gym Rep Tracker");
}

/**
 * @brief Gets the current rep count for the current exercise.
 */
uint16_t app_controller_get_rep_count(void)
{
    return app_state.rep_count;
}

/**
 * @brief Gets the current exercise type.
 */
exercise_t app_controller_get_current_exercise(void)
{
    return app_state.current_exercise;
}
