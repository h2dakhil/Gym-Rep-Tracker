#include "imu_filters.h"
#include <math.h>

// Simple low-pass filter coefficients
#define ACCEL_ALPHA 0.1f
#define GYRO_ALPHA 0.1f

// Filter state
static float accel_filter_state[3] = {0.0f, 0.0f, 0.0f};
static float gyro_filter_state[3] = {0.0f, 0.0f, 0.0f};
static float horizontal_reference[3] = {0.0f, 0.0f, 1.0f}; // Default to +Z up

void imu_filters_init(void)
{
    // Initialize filter states to zero
    for (int i = 0; i < 3; i++) {
        accel_filter_state[i] = 0.0f;
        gyro_filter_state[i] = 0.0f;
    }
    
    // Set default horizontal reference (assuming +Z is up)
    horizontal_reference[0] = 0.0f;
    horizontal_reference[1] = 0.0f;
    horizontal_reference[2] = 1.0f;
}

void imu_filters_process_all(const MPU6050_ScaledData_t *raw_data, 
                           IMUFilteredData_t *filtered_data, 
                           float dt, 
                           exercise_t exercise)
{
    if (!raw_data || !filtered_data) return;
    
    // Apply low-pass filter to accelerometer data
    accel_filter_state[0] = ACCEL_ALPHA * raw_data->accel_x_g + (1.0f - ACCEL_ALPHA) * accel_filter_state[0];
    accel_filter_state[1] = ACCEL_ALPHA * raw_data->accel_y_g + (1.0f - ACCEL_ALPHA) * accel_filter_state[1];
    accel_filter_state[2] = ACCEL_ALPHA * raw_data->accel_z_g + (1.0f - ACCEL_ALPHA) * accel_filter_state[2];
    
    // Apply low-pass filter to gyroscope data
    gyro_filter_state[0] = GYRO_ALPHA * raw_data->gyro_x_deg_s + (1.0f - GYRO_ALPHA) * gyro_filter_state[0];
    gyro_filter_state[1] = GYRO_ALPHA * raw_data->gyro_y_deg_s + (1.0f - GYRO_ALPHA) * gyro_filter_state[1];
    gyro_filter_state[2] = GYRO_ALPHA * raw_data->gyro_z_deg_s + (1.0f - GYRO_ALPHA) * gyro_filter_state[2];
    
    // Store filtered data
    filtered_data->accel_filtered[0] = accel_filter_state[0];
    filtered_data->accel_filtered[1] = accel_filter_state[1];
    filtered_data->accel_filtered[2] = accel_filter_state[2];
    
    filtered_data->gyro_filtered[0] = gyro_filter_state[0];
    filtered_data->gyro_filtered[1] = gyro_filter_state[1];
    filtered_data->gyro_filtered[2] = gyro_filter_state[2];
    
    // Copy horizontal reference
    filtered_data->horizontal_ref[0] = horizontal_reference[0];
    filtered_data->horizontal_ref[1] = horizontal_reference[1];
    filtered_data->horizontal_ref[2] = horizontal_reference[2];
    
    // Set horizontal vector
    filtered_data->horizontal_vector.x = horizontal_reference[0];
    filtered_data->horizontal_vector.y = horizontal_reference[1];
    filtered_data->horizontal_vector.z = horizontal_reference[2];
    
    // Compute curl axis scalar (simplified - use Y-axis acceleration for bicep curl)
    filtered_data->curl_axis_scalar = raw_data->accel_y_g;
}

void imu_filters_set_horizontal_reference(const vec3_t *ref)
{
    if (!ref) return;
    
    // Normalize the reference vector
    float magnitude = sqrtf(ref->x * ref->x + ref->y * ref->y + ref->z * ref->z);
    if (magnitude > 0.0f) {
        horizontal_reference[0] = ref->x / magnitude;
        horizontal_reference[1] = ref->y / magnitude;
        horizontal_reference[2] = ref->z / magnitude;
    }
}
