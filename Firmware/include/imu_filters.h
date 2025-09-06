#ifndef IMU_FILTERS_H
#define IMU_FILTERS_H

#include "mpu6050.h"
#include "exercise_config.h"

// Vector type for 3D operations
typedef struct {
    float x, y, z;
} vec3_t;

// IMU filter state structure
typedef struct {
    float accel_filtered[3];
    float gyro_filtered[3];
    float horizontal_ref[3];
    float curl_axis_scalar;  // Scalar value for curl detection
    struct {
        float x, y, z;
    } horizontal_vector;  // Vector for horizontal reference
} IMUFilteredData_t;

// Function declarations
void imu_filters_init(void);
void imu_filters_process_all(const MPU6050_ScaledData_t *raw_data, 
                           IMUFilteredData_t *filtered_data, 
                           float dt, 
                           exercise_t exercise);
void imu_filters_set_horizontal_reference(const vec3_t *ref);

#endif // IMU_FILTERS_H
