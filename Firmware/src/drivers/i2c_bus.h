#ifndef I2C_BUS_H
#define I2C_BUS_H

#include "stm32f1xx_hal.h"

// I2C bus speed configuration
#define I2C_BUS_SPEED_FAST_MODE     400000  // 400 kHz
#define I2C_BUS_SPEED_STANDARD_MODE 100000  // 100 kHz (fallback)

/**
 * @brief Initializes the I2C bus.
 * @retval HAL_StatusTypeDef HAL_OK if initialization is successful, HAL_ERROR otherwise.
 */
HAL_StatusTypeDef i2c_bus_init(void);

/**
 * @brief Reads a sequence of bytes from a device's internal register.
 * @param dev_address Device address (7-bit, left-shifted by 1 for HAL usage).
 * @param reg_address Register address to read from.
 * @param pData Pointer to buffer to store read data.
 * @param Size Number of bytes to read.
 * @retval HAL_StatusTypeDef HAL_OK if successful, HAL_ERROR otherwise.
 */
HAL_StatusTypeDef i2c_mem_read(uint16_t dev_address, uint16_t reg_address, uint8_t *pData, uint16_t Size);

/**
 * @brief Writes a sequence of bytes to a device's internal register.
 * @param dev_address Device address (7-bit, left-shifted by 1 for HAL usage).
 * @param reg_address Register address to write to.
 * @param pData Pointer to buffer containing data to write.
 * @param Size Number of bytes to write.
 * @retval HAL_StatusTypeDef HAL_OK if successful, HAL_ERROR otherwise.
 */
HAL_StatusTypeDef i2c_mem_write(uint16_t dev_address, uint16_t reg_address, uint8_t *pData, uint16_t Size);

#endif // I2C_BUS_H

