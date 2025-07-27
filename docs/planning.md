# Gym Rep Tracker — PCB Planning & Requirements

## Table of Contents
1. [Form Factor & Wearability](#form-factor--wearability)  
2. [Power & Battery](#power--battery)  
3. [User Interface](#user-interface)  
4. [Sensor Science & Counting Logic](#sensor-science-and-counting-logic)  
5. [Physical Controls](#physical-controls)  
6. [Test Points](#test-points)  
7. [Communication Protocols](#communication-protocols)  
8. [System Logic & Block Diagram](#system-logic--block-diagram)  

---

## Form Factor & Wearability
- **Wrist Strap**  
  - Athletic compression sleeve (e.g. Nike) with integrated Velcro pocket.  
- **PCB Pocket**  
  - Laser-cut neoprene pouch in the sleeve so the PCB slides in snugly against the forearm.  
- **Sensor Placement**  
  - IMU footprint on the underside of the PCB, pressing directly on skin to reduce motion noise.  

---

## Power & Battery
- **Battery**  
  - Single-cell Li-Po (3.7 V nominal, 500–1000 mAh) with built-in protection circuit.  
  - JST-PH 2-pin connector footprint on PCB.  
  - ~800 mAh → ~8 hr runtime at ~100 mA draw.  
- **PMIC**  
  - USB-C VBUS (5 V) → PMIC for USB-C CC negotiation, battery charging, and power-path switching.  
  - PMIC output (VBAT) feeds an LDO.  
- **LDO**  
  - Input = 3.7–4.2 V from PMIC → regulated 3.3 V → powers MCU, OLED, IMU.  
- **Decoupling & Bulk Caps**  
  - 0.1 µF + 4.7 µF at every power pin.  
  - 10 µF at PMIC input/output rails.  

---

## User Interface
### OLED Display (Main UI)
- **Type:** 0.96″ SPI-driven OLED  
- **Connections:**  
  - SCK → MCU_SPI_SCK  
  - MOSI → MCU_SPI_MOSI  
  - CS  → MCU_GPIO_CS  
  - DC  → MCU_GPIO_DC  
  - RST → MCU_GPIO_RST  
  - VCC/GND → 3.3 V / GND  

### LED Indicators
- **Green LED**  
  - MCU_GPIO1 → 1 kΩ → LED → GND  
  - On when tracking is active.  
- **Yellow LED**  
  - MCU_GPIO2 → 1 kΩ → LED → GND  
  - On when battery voltage < threshold (measured via ADC).

---

## Sensor Science and Counting Logic
- **IMU**  
  - MPU6050 (6-axis) or ICM20948 (9-axis) via I²C.  
- **Sampling**  
  - 100 Hz (every 10 ms).  
- **Peak Detection Algorithm**  
  1. Read Z-axis acceleration.  
  2. Apply 5-sample moving average filter.  
  3. When filtered value crosses upward threshold → register one rep.  
- **Calibration**  
  - On startup, record “rest” acceleration (gravity bias) and subtract from readings.

---

## Physical Controls
- **Power / Mode Button**  
  - Momentary push-button to ground MCU_GPIO; long-press = power off, short-press = mode cycle.  
- **SWD Header**  
  - Standard 2×5 (10-pin) or 2×4 footprint.  
  - Exposes: SWCLK, SWDIO, NRST, VDD, GND.  
- **Reset Button (optional)**  
  - Push-button to NRST pin for manual MCU reset.

---

## Test Points
- SMD pad + via for:  
  - **VBAT**  
  - **3.3 V**  
  - **GND**  
  - **SDA**  
  - **SCL**  
  - **SPI_SCK**  
  - **SPI_MOSI**  
  - **ADC_BATTERY** (divider output)

---

## Communication Protocols
### I²C (Sensor Bus)
- **Use for:** IMU (accelerometer/gyro)  
- **Lines:** SDA, SCL, GND + 2×4.7 kΩ pull-ups to 3.3 V  

### SPI (Display Bus)
- **Use for:** OLED display  
- **Lines:** SCK, MOSI, CS, DC, RST + GND  

---

## System Logic & Block Diagram

> **Disclaimer:**  
> - Assumes MCU, OLED display, and IMU all operate at 3.3 V.  
> - Assumes Li-Po battery pack includes a protection circuit.  
> - Assumes PMIC supports USB-C CC negotiation, battery charging, and power-path switching.  
> - USB-C D+ / D– are wired directly to the STM32’s USB_DP / USB_DM pins for DFU flashing and serial logging. 

1. **USB-C Plug-In & Power Negotiation**  
   1. User connects a USB-C cable → VBUS (5 V) appears at the connector.  
   2. PMIC senses CC1/CC2 resistors → advertises as a downstream device → negotiates 5 V input.  
   3. VBUS drives the PMIC’s charge/display circuitry.  

2. **Battery Charging & Power-Path Management**  
   1. PMIC routes incoming 5 V to charge the Li-Po via its internal charge regulator.  
   2. Simultaneously, PMIC’s power-path switch selects the higher of USB-C or battery voltage to feed its “SYS” output.  
   3. If USB-C is present → PMIC feeds “SYS” from VBUS, keeping the battery topped off.  
   4. If USB-C is removed → PMIC switches “SYS” to the battery, ensuring uninterrupted operation.  

3. **3.3 V Regulation**  
   1. PMIC “SYS” output (~3.7–4.2 V) feeds the LDO input.  
   2. LDO drops “SYS” to a stable 3.3 V rail.  
   3. Decoupling capacitors at the LDO input and output smooth transients.  
   4. The 3.3 V rail powers:  
      - **STM32 MCU**  
      - **OLED display** via SPI  
      - **IMU sensor** via I²C  

4. **MCU Initialization & Boot**  
   1. On power-up or reset, the STM32 boots its internal bootloader or DFU application (via USB).  
   2. If SWD is connected, a debugger can override and control execution from reset.  

5. **Peripheral Power-On Sequence**  
   1. MCU de-asserts reset lines to OLED and IMU.  
   2. Pull-up resistors on I²C and SPI lines ensure known idle states.  

6. **Sensor & Display Setup**  
   1. MCU configures I²C peripheral → initializes IMU registers (sample rate, scale).  
   2. MCU configures SPI peripheral → resets and initializes the OLED driver (screen resolution, color mode).  

7. **Main Loop: Rep Counting & UI Update**  
   1. **Read Sensor:** at 100 Hz, MCU polls IMU over I²C → reads X/Y/Z acceleration.  
   2. **Filter & Detect Peak:** apply 5-sample moving average to Z-axis → compare to threshold → when threshold crossed upward, increment rep count.  
   3. **Update Display:** use SPI to send new rep count, mode, and battery status to the OLED.  
   4. **LED Control:**  
      - Green LED remains lit while the device is active and tracking.  
      - If rep counting is paused or standby, green LED toggles off.  

8. **Battery Monitoring & Warning**  
   1. Periodically (e.g. once per second), MCU triggers its ADC to sample the voltage divider tapped from VBAT.  
   2. ADC value → calculate actual battery voltage.  
   3. If voltage < low-battery threshold (e.g. 3.4 V):  
      - Turn **yellow LED** ON.  
      - Display “LOW BATTERY” message on the OLED.  
      - Optionally disable rep counting or enter low-power standby.  

9. **USB-DFU & Serial Logging**  
   1. When connected to a PC, MCU’s USB_DP/DM pins enumerate as a USB-CDC (serial) or DFU device.  
   2. User can upload new firmware via DFU or open a serial terminal to view debug logs.  

10. **SWD Debug Fallback**  
    1. If USB-DFU fails (e.g. bad firmware), connect an SWD debugger to SWCLK/SWDIO/NRST.  
    2. Use the debugger to erase or re-flash the MCU and step through code.  

11. **Power-Off & Sleep**  
    1. On long-press of the power/mode button, firmware initiates graceful shutdown:  
       - Display “GOODBYE” message.  
       - Turn off tracking and LEDs.  
       - Place MCU into deep-sleep or standby.  
    2. Battery remains charged and ready for next session.  
