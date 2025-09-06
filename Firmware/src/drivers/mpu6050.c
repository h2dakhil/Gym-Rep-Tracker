#include "mpu6050.h"
#include "i2c_bus.h"
#include "app_config.h"
#include <math.h>

// MPU6050 Register Map
#define MPU6050_PWR_MGMT_1      0x6B
#define MPU6050_SMPLRT_DIV      0x19
#define MPU6050_CONFIG          0x1A
#define MPU6050_GYRO_CONFIG     0x1B
#define MPU6050_ACCEL_CONFIG    0x1C
#define MPU6050_ACCEL_XOUT_H    0x3B
#define MPU6050_TEMP_OUT_H      0x41
#define MPU6050_GYRO_XOUT_H     0x43

// MPU6050 Full-Scale Range Factors
static float ACCEL_SCALE_FACTOR = 0.0f;
static float GYRO_SCALE_FACTOR = 0.0f;

// Calibration data
static float accel_bias[3] = {0.0f, 0.0f, 0.0f};
static float gyro_bias[3] = {0.0f, 0.0f, 0.0f};

/**
 * @brief Writes a single byte to an MPU6050 register.
 */
static HAL_StatusTypeDef MPU6050_WriteRegister(uint8_t reg, uint8_t value)
{
    return i2c_mem_write(MPU6050_I2C_ADDR, reg, &value, 1);
}

/**
 * @brief Reads a single byte from an MPU6050 register.
 */
static HAL_StatusTypeDef MPU6050_ReadRegister(uint8_t reg, uint8_t *value)
{
    return i2c_mem_read(MPU6050_I2C_ADDR, reg, value, 1);
}

/**
 * @brief Initializes the MPU-6050 sensor.
 */
HAL_StatusTypeDef mpu6050_init(void)
{
    uint8_t who_am_i;
    if (MPU6050_ReadRegister(0x75, &who_am_i) != HAL_OK || who_am_i != 0x68)
    {
        // LOG("MPU-6050 not found or WHO_AM_I mismatch!\n");
        return HAL_ERROR;
    }

    // Wake up MPU-6050
    if (MPU6050_WriteRegister(MPU6050_PWR_MGMT_1, 0x00) != HAL_OK) return HAL_ERROR;

    // Set sample rate to IMU_SAMPLE_HZ (e.g., 200Hz, with DLPF 42Hz, gives SMPLRT_DIV = 4)
    // Sample Rate = Gyroscope Output Rate / (1 + SMPLRT_DIV)
    // Gyro Output Rate = 1kHz (when DLPF is enabled and set to 42Hz or less)
    // 200 Hz = 1000 Hz / (1 + SMPLRT_DIV) => 1 + SMPLRT_DIV = 5 => SMPLRT_DIV = 4
    if (MPU6050_WriteRegister(MPU6050_SMPLRT_DIV, (1000 / IMU_SAMPLE_HZ) - 1) != HAL_OK) return HAL_ERROR;

    // Configure DLPF (Digital Low Pass Filter)
    // F_EXT_SYNC_SET = 0, DLPF_CFG = 3 (42 Hz) for both accel and gyro
    if (MPU6050_WriteRegister(MPU6050_CONFIG, 0x03) != HAL_OK) return HAL_ERROR;

    // Configure Gyroscope: +/- 250 deg/s (FS_SEL = 0)
    if (MPU6050_WriteRegister(MPU6050_GYRO_CONFIG, 0x00) != HAL_OK) return HAL_ERROR;
    GYRO_SCALE_FACTOR = 131.0f; // 131 LSB/deg/s for +/- 250 deg/s

    // Configure Accelerometer: +/- 2g (AFS_SEL = 0)
    if (MPU6050_WriteRegister(MPU6050_ACCEL_CONFIG, 0x00) != HAL_OK) return HAL_ERROR;
    ACCEL_SCALE_FACTOR = 16384.0f; // 16384 LSB/g for +/- 2g

    return HAL_OK;
}

/**
 * @brief Reads raw accelerometer and gyroscope data from MPU-6050.
 */
HAL_StatusTypeDef mpu6050_read_raw(MPU6050_RawData_t *rawData)
{
    uint8_t buffer[14];
    if (i2c_mem_read(MPU6050_I2C_ADDR, MPU6050_ACCEL_XOUT_H, buffer, 14) != HAL_OK) return HAL_ERROR;

    rawData->accel_x = (int16_t)(buffer[0] << 8 | buffer[1]);
    rawData->accel_y = (int16_t)(buffer[2] << 8 | buffer[3]);
    rawData->accel_z = (int16_t)(buffer[4] << 8 | buffer[5]);
    // Skip temperature (buffer[6], buffer[7])
    rawData->gyro_x = (int16_t)(buffer[8] << 8 | buffer[9]);
    rawData->gyro_y = (int16_t)(buffer[10] << 8 | buffer[11]);
    rawData->gyro_z = (int16_t)(buffer[12] << 8 | buffer[13]);

    return HAL_OK;
}

/**
 * @brief Calibrates the MPU-6050 sensor.
 */
HAL_StatusTypeDef mpu6050_calibrate(void)
{
    MPU6050_RawData_t raw;
    long sum_accel[3] = {0, 0, 0};
    long sum_gyro[3] = {0, 0, 0};
    const uint16_t num_samples = 500;

    // TODO: Add a delay here for stability before calibration starts

    for (uint16_t i = 0; i < num_samples; i++)
    {
        if (mpu6050_read_raw(&raw) != HAL_OK) return HAL_ERROR;
        sum_accel[0] += raw.accel_x;
        sum_accel[1] += raw.accel_y;
        sum_accel[2] += raw.accel_z;
        sum_gyro[0] += raw.gyro_x;
        sum_gyro[1] += raw.gyro_y;
        sum_gyro[2] += raw.gyro_z;
        HAL_Delay(5); // Small delay between samples
    }

    // Compute biases (accel bias is effectively gravity vector)
    accel_bias[0] = (float)sum_accel[0] / num_samples / ACCEL_SCALE_FACTOR;
    accel_bias[1] = (float)sum_accel[1] / num_samples / ACCEL_SCALE_FACTOR;
    accel_bias[2] = (float)sum_accel[2] / num_samples / ACCEL_SCALE_FACTOR;

    gyro_bias[0] = (float)sum_gyro[0] / num_samples / GYRO_SCALE_FACTOR;
    gyro_bias[1] = (float)sum_gyro[1] / num_samples / GYRO_SCALE_FACTOR;
    gyro_bias[2] = (float)sum_gyro[2] / num_samples / GYRO_SCALE_FACTOR;

    return HAL_OK;
}

/**
 * @brief Converts raw IMU data to scaled values (g for accel, deg/s for gyro).
 */
void mpu6050_convert_to_scaled(const MPU6050_RawData_t *rawData, MPU6050_ScaledData_t *scaledData)
{
    scaledData->accel_x_g = (float)rawData->accel_x / ACCEL_SCALE_FACTOR - accel_bias[0];
    scaledData->accel_y_g = (float)rawData->accel_y / ACCEL_SCALE_FACTOR - accel_bias[1];
    scaledData->accel_z_g = (float)rawData->accel_z / ACCEL_SCALE_FACTOR - accel_bias[2] + 1.0f; // Assume Z is aligned with gravity and remove 1g offset

    scaledData->gyro_x_deg_s = (float)rawData->gyro_x / GYRO_SCALE_FACTOR - gyro_bias[0];
    scaledData->gyro_y_deg_s = (float)rawData->gyro_y / GYRO_SCALE_FACTOR - gyro_bias[1];
    scaledData->gyro_z_deg_s = (float)rawData->gyro_z / GYRO_SCALE_FACTOR - gyro_bias[2];
}

