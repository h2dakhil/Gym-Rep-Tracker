#ifndef MPU6050_H
#define MPU6050_H

#include "stm32f1xx_hal.h"

#define MPU6050_I2C_ADDR    (0x68 << 1) // 0xD0

typedef struct {
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
} MPU6050_RawData_t;

typedef struct {
    float accel_x_g;
    float accel_y_g;
    float accel_z_g;
    float gyro_x_deg_s;
    float gyro_y_deg_s;
    float gyro_z_deg_s;
} MPU6050_ScaledData_t;

/**
 * @brief Initializes the MPU-6050 sensor.
 * @retval HAL_StatusTypeDef HAL_OK if successful, HAL_ERROR otherwise.
 */
HAL_StatusTypeDef mpu6050_init(void);

/**
 * @brief Reads raw accelerometer and gyroscope data from MPU-6050.
 * @param rawData Pointer to MPU6050_RawData_t struct to store data.
 * @retval HAL_StatusTypeDef HAL_OK if successful, HAL_ERROR otherwise.
 */
HAL_StatusTypeDef mpu6050_read_raw(MPU6050_RawData_t *rawData);

/**
 * @brief Calibrates the MPU-6050 sensor by reading multiple samples.
 *        Computes gyro bias and average acceleration (gravity vector).
 * @retval HAL_StatusTypeDef HAL_OK if successful, HAL_ERROR otherwise.
 */
HAL_StatusTypeDef mpu6050_calibrate(void);

/**
 * @brief Converts raw IMU data to scaled values (g for accel, deg/s for gyro).
 * @param rawData Pointer to raw MPU6050_RawData_t struct.
 * @param scaledData Pointer to MPU6050_ScaledData_t struct to store scaled data.
 */
void mpu6050_convert_to_scaled(const MPU6050_RawData_t *rawData, MPU6050_ScaledData_t *scaledData);

#endif // MPU6050_H

