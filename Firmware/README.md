# Gym Rep Tracker Firmware

This directory contains the embedded firmware for a wrist-mounted rep tracker built around an **STM32F103CBT6 microcontroller**, **MPU-6050 IMU**, and **SSD1306 OLED display**.  
The firmware implements sensor drivers, signal filtering, calibration, and a rep-detection algorithm tailored to multiple exercises.

---

## Features
- **Multi-Exercise Support**: Bicep Curl, Shoulder Press, Bench Press (extendable to more)
- **State Machine Control**: Boot → Exercise Selection → Calibration → Detecting → Running
- **Exercise-Specific Calibration**: Per-exercise baseline mean/std. deviation, with dynamic thresholds
- **Rep Detection Algorithm**: Peak detection with prominence & refractory checks to prevent false counts
- **Low-Pass Filtering**: Smooths accelerometer/gyroscope signals for robust detection
- **OLED UI**: Displays splash, exercise selection, calibration state, rep counts, and status messages
- **UART Debug Logging (optional)**: Real-time streaming of thresholds, IMU samples, and rep detection results
- **Fallback Protection**: I²C bus initialization with automatic downgrade from fast to standard mode if needed

---

## Build & Flash Instructions
1. **Install PlatformIO**: Install the PlatformIO extension in VS Code.  
2. **Open Project**: Open the `Firmware/` directory.  
3. **Build**:  
   platformio run


or click the Build checkmark in the toolbar.

4. Flash: Upload to STM32F103CBT6 via ST-Link:

# Wiring Summary
- **MCU**: STM32F103CBT6  
- **IMU (MPU-6050) & OLED (SSD1306)**: Shared I²C1 bus  
  - PB6 → I2C1_SCL  
  - PB7 → I2C1_SDA  
  - Ensure 3.3 V supply and pull-ups (≈4.7kΩ) on SDA/SCL if not onboard  
- **Optional UART Logging**:  
  - PA2 → TX  
  - PA3 → RX  

---

# Firmware Architecture

## Drivers
- **mpu6050.c**: Initializes sensor, configures DLPF, handles calibration, scaling raw IMU data  
- **ssd1306.c**: Minimal OLED driver with ASCII rendering and UI helpers  
- **i2c_bus.c**: HAL wrapper for I²C; includes fallback from Fast Mode to Standard Mode  

## Core Logic
- **imu_filters.c**: Applies low-pass filters, projects motion onto exercise-specific axes  
- **rep_detect.c**: Maintains rolling mean/std. deviation buffer; detects peaks using thresholds  
- **app_controller.c**: High-level state machine managing boot, calibration, detection, and UI updates  
- **systick.c**: Millisecond tick counter for scheduling  

## UI
- Displays splash, exercise name, calibration, live rep counts  
- Text rendering with auto-centering and truncation  

---

# Adding a New Exercise
1. Add a new entry in `include/exercise_config.h` with thresholds & refractory time.  
2. Implement signal projection in `imu_filters.c`.  
3. Update `app_controller.c` for name display and calibration needs.  
4. Test & tune thresholds using UART logging.  

---

# Debug & Troubleshooting

## Enable Debug Mode
- Define `ENABLE_LOG_UART` in `app_config.h` to stream real-time values at **115200 baud**.  

## Common Issues
- **No reps detected** → lower `thresh_k` or `min_prominence_g`  
- **False positives** → increase `min_prominence_g` or `refractory_ms`  
- **IMU not responding** → check I²C wiring & power  
- **Display issues** → confirm pull-ups on SDA/SCL  
