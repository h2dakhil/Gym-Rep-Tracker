# Fitness Rep Tracker 🏋️‍♂️

A **wrist-mounted STM32 wearable** that detects and classifies **4+ gym exercises** with real-time feedback.  
The project was built completely from scratch, including **custom 4-layer PCB design, embedded firmware, IMU motion sensing, and OLED feedback display.**

---

## 🚀 Features
- **Hardware**
  - Custom **4-layer PCB** designed in KiCad
  - MCU: **STM32F103**
  - IMU: **MPU-6050** (3-axis accelerometer + 3-axis gyroscope, I²C)
  - Display: **SSD1306 OLED** (128×64, I²C)
  - Power: **MCP73831 Li-ion charger**, 3.3 V LDO, battery-sense divider, USB-C input
  - Debug: **SWD header**, status LEDs

- **Firmware (C)**
  - Written in **C** using **PlatformIO** and STM32 HAL drivers
  - **IMU data acquisition** and calibration
  - **Rep detection algorithm** using peak prominence + refractory period filtering
  - Rolling buffer statistics & low-pass signal filters
  - State machine: boot → exercise selection → calibration → running
  - **OLED driver** for real-time rep count & status display
  - UART logging for debugging
  - Peripheral initialization with fallback & error handling

- **Testing & Debugging**
  - SMT soldered & validated using **DMM** and **oscilloscope**
  - Verified stable 3.3 V rail and signal nets