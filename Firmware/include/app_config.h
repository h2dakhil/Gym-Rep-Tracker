#ifndef APP_CONFIG_H
#define APP_CONFIG_H

// IMU Configuration
#define IMU_SAMPLE_HZ 200  // IMU sampling frequency in Hz

// Display Configuration
#define OLED_WIDTH 128
#define OLED_HEIGHT 64

// Timing Configuration
#define UI_UPDATE_RATE_HZ 10  // UI update rate in Hz
#define IMU_READ_INTERVAL_MS (1000 / IMU_SAMPLE_HZ)  // Time between IMU reads
#define IMU_SAMPLE_INTERVAL_MS IMU_READ_INTERVAL_MS  // Alias for compatibility
#define UI_REFRESH_INTERVAL_MS (1000 / UI_UPDATE_RATE_HZ)  // UI refresh interval

// Exercise Detection Configuration
#define CALIBRATION_SAMPLES 100  // Number of samples to collect during calibration
#define DETECTION_WARMUP_MS 1000  // Warm-up time before detection starts

// Logging Configuration
#define ENABLE_LOG_UART 0  // Enable/disable UART logging

#endif // APP_CONFIG_H
