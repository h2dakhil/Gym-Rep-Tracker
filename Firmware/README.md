# Gym Rep Tracker Firmware

This project provides firmware for a wearable gym rep tracker using an STM32F103CBT6 microcontroller, MPU-6050 IMU, and SSD1306 OLED display.

## Features

- **Multi-Exercise Support**: Bicep Curl, Shoulder Press, and Bench Press
- **Automatic Exercise Selection**: Cycles through exercises on boot with 2-second intervals
- **Exercise-Specific Calibration**: Per-exercise baseline calibration for optimal detection
- **Real-time Rep Counting**: Live rep detection and counting for each exercise
- **OLED Display**: Clear exercise name and rep count display

## Build and Flash Instructions

1. **Install PlatformIO:** If you don't have it already, install the PlatformIO extension in VS Code.
2. **Open Project:** Open this project directory in VS Code.
3. **Build:** Click the "Build" icon (checkmark) in the PlatformIO toolbar, or run `platformio run` in your terminal.
4. **Flash:** Connect your STM32F103CBT6 board via an ST-Link debugger. Click the "Upload" icon (right arrow) in the PlatformIO toolbar, or run `platformio run -t upload`.

## Wiring Summary

- **MCU:** STM32F103CBT6
- **IMU (MPU-6050) & OLED (SSD1306):** Share I2C1 bus.
  - `PB6` -> `I2C1_SCL`
  - `PB7` -> `I2C1_SDA`
  - Connect `3.3V` to `VDD` of both MPU-6050 and SSD1306.
  - Ensure appropriate pull-up resistors (e.g., 4.7kΩ) are connected from SCL and SDA to 3.3V if not already present on your modules.
- **Optional UART Logging:**
  - `PA2` -> `UART2_TX`
  - `PA3` -> `UART2_RX`

## Exercise Selection and Operation

### Boot Sequence
1. **BOOT**: System initialization and splash screen
2. **SELECTING_EXERCISE**: Auto-cycles through exercises every 2 seconds
   - Bicep Curl → Shoulder Press → Bench Press
   - After 6 seconds, automatically selects the current exercise
3. **CALIBRATING_EXERCISE**: 2-second calibration for selected exercise
4. **DETECTING**: 1-second warm-up window
5. **RUNNING**: Live rep counting

### Exercise-Specific Signal Processing

- **Bicep Curl**: Horizontal motion projection (perpendicular to gravity)
- **Shoulder Press**: Vertical motion projection (up-down movement against gravity)
- **Bench Press**: Horizontal anterior-posterior projection (press direction)

## How to Change Exercise Sensitivity

Each exercise has its own tuning parameters in `include/exercise_config.h`:

### Bicep Curl
- `thresh_k`: 1.2 (threshold multiplier)
- `min_prominence_g`: 0.40g (minimum peak prominence)
- `refractory_ms`: 700ms (minimum time between reps)

### Shoulder Press
- `thresh_k`: 1.1 (threshold multiplier)
- `min_prominence_g`: 0.35g (minimum peak prominence)
- `refractory_ms`: 800ms (minimum time between reps)

### Bench Press
- `thresh_k`: 1.1 (threshold multiplier)
- `min_prominence_g`: 0.30g (minimum peak prominence)
- `refractory_ms`: 900ms (minimum time between reps)

### Tuning Guidelines

- **Shoulder Press**: If false positives, increase `min_prominence_g` to 0.45 or raise `refractory_ms` to 1000
- **Bench Press**: If under-counting, lower `thresh_k` to 1.0 or reduce `min_prominence_g` to 0.25
- **Bicep Curl**: Default settings should work well for most users

## How to Add a New Exercise

To add a new exercise:

1. **Update `include/exercise_config.h`**:
   - Add new exercise enum value
   - Add configuration to `EX_CFG` table
   - Define appropriate thresholds and timing

2. **Update `src/sensing/imu_filters.c`**:
   - Add new case in `imu_filters_compute_rep_signal()`
   - Implement appropriate signal projection for the exercise

3. **Update `src/app/app_controller.c`**:
   - Add exercise name display
   - Handle any exercise-specific calibration requirements

4. **Test and Tune**:
   - Use the calibration process to establish baseline
   - Adjust thresholds based on detection accuracy

## Advanced Features

### UART Logging
Enable detailed logging by uncommenting `#define ENABLE_LOG_UART` in `include/app_config.h`. This provides:
- Sample values and thresholds
- Peak detection decisions
- Calibration data

### Custom Calibration
Each exercise gets its own baseline calibration during the `CALIBRATING_EXERCISE` state. The system:
- Collects 2 seconds of IMU data
- Computes mean and standard deviation
- Uses dynamic thresholds: `baseline_mu + k * max(rolling_sigma, min_sigma_floor)`

## Troubleshooting

### Common Issues
- **No reps detected**: Check calibration, reduce `thresh_k` or `min_prominence_g`
- **False positives**: Increase `min_prominence_g` or `refractory_ms`
- **Display issues**: Verify I2C connections and pull-up resistors
- **IMU not responding**: Check power and I2C connections

### Debug Mode
Enable UART logging to see real-time sensor data and detection decisions. Connect a USB-to-UART converter to PA2/PA3 and monitor at 115200 baud.
