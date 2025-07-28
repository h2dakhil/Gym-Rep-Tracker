#Project Overview

July 17/2025
A wearable gym-rep tracker that:
- Uses an STM32 MCU to read motion from an IMU
- Counts reps and displays them on a small SPI display
- Sends data via BLE (or UART) to a phone/PC
- Packs into a 3D-printed case and logs/calibrates via Python

#Project Plan: Step-by-step
Push everything to git in its step blocks. 
## ✅ 1. Planning & Requirements (2 days?)

- [ ] List all system components
- [ ] Decide on communication protocols for each component:
- [ ] Draw a **block diagram** of how everything connects
- [ ] Sketch a rough 3D layout idea (screen, buttons, ports, casing)

---

## ✅ 2. Part Selection & BOM (2 days)
**Look into prototyping the board using a breadboard and what is needed for that first

- [ ] Choose exact STM32 chip (e.g. STM32F401CE)
- [ ] Select IMU, display, BLE module, power ICs
- [ ] Confirm all voltage compatibility (e.g. 3.3V logic)
- [ ] Create a Bill of Materials (BOM) spreadsheet
- [ ] Write out power budget (mA per component)

---

## ✅ 3. Schematic & PCB Design (5 days?)
- figure out what to do from here. Am I ordering the dev stuff or notfirst? am i doing firmware first?
- [ ] Create schematic in KiCad
- [ ] Annotate nets (3.3V, GND, I2C_SCL, SPI_MOSI, etc.)
- [ ] Double-check all component footprints
- [ ] Route PCB layout (2-layer)
- [ ] Run DRC/ERC checks
- [ ] Export Gerbers + order PCB (Rememebr to also order kicad heart light)

---

## ✅ 4. Firmware Development (5 days)?

- [ ] Set up STM32CubeIDE project
- [ ] Initialize HAL drivers for:
  - I²C (IMU)
  - SPI (Display)
  - UART (BLE + Debug)
- [ ] Write IMU read code (accel + gyro)
- [ ] Implement basic rep detection logic (thresholding)
- [ ] Display rep count + mode on screen
- [ ] Send data over BLE/UART

---

## ✅ 5. Analog Front-End (Optional)

- [ ] If adding EMG or flex sensors:
  - Design op-amp circuit to filter/amplify signal
  - Ensure output is 0–3.3V for ADC
  - Add to PCB + firmware for ADC read
- [ ] If using only MPU6050, skip this section

---

## ✅ 6. Enclosure & CAD (3-4 days?)

- [ ] Measure PCB dimensions
- [ ] Design a 3D-printable case (Fusion 360 or SolidWorks)
- [ ] Include cutouts for screen, USB, buttons
- [ ] Export STL and print prototype

---

## ✅ 7. Python Automation (5 days)

- [ ] Write a Python script to:
  - Read serial data from BLE/UART
  - Save data to CSV
  - Help with sensor calibration
  - Optional: Live graph data

---

## ✅ 8. Final Polish & GitHub (5 days)

- [ ] Create a complete GitHub repo:
  - `README.md` with features + setup
  - `docs/` folder with notes + diagrams
  - `hardware/` (KiCad files)
  - `firmware/` (STM32 project)
  - `mechanical/` (CAD files)
  - `python-logger/` (Python script)
  - `images/` (block diagram, photos)
- [ ] Add demo video or animated GIF
- [ ] Clean up commit history and branches

---
https://github.com/Hrushi-d/Gym-Exercise-RepCounter
https://github.com/calumbruton/Vein

At the end: Streamline all documentation planning into 1.
