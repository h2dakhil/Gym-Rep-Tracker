# Gym Rep Tracker Hardware

This directory contains the **hardware design files** for the Gym Rep Tracker, including schematics, PCB layouts, fabrication outputs, and backups.  
The hardware is built around the **STM32F103CBT6 microcontroller**, with supporting circuitry for the **MPU-6050 IMU**, **SSD1306 OLED**, battery charging, and power regulation.

---

## Directory Structure

- **Backups/**  
  Contains backup copies of hardware design files for recovery and version tracking.  

- **Fab/**  
  Fabrication-ready outputs, including Gerber files, drill files, and pick-and-place exports for PCB manufacturing.  

- **PCB/**  
  Source PCB design files (e.g., KiCad project files, layout, footprints).  
  - Includes board layout for the 4-layer stack-up (GND plane, power plane, signal layers).  

- **Schematic/**  
  Electrical schematics showing MCU, IMU, OLED, power management (MCP73831 Li-ion charger, LDO regulator), and supporting passives.  

- **README.md**  
  This file, describing the structure and usage of the hardware directory.  

---

## Usage

1. **View Schematics**  
   Open the files in the `Schematic/` folder using KiCad (recommended version noted in project files).  

2. **Edit PCB Layout**  
   Open the project in KiCad from the `PCB/` folder.  

3. **Generate Manufacturing Files**  
   Use KiCad’s *Plot* and *Fabrication Outputs* to regenerate Gerbers if changes are made.  
   Fabrication-ready files are already available in the `Fab/` folder.  

---

## Notes
- **Backups** are not guaranteed to be the latest version. Always check the active `PCB/` and `Schematic/` folders first.  
- Ensure fabrication files match the correct **4-layer stack-up** before sending to a manufacturer.  
- If modifying, confirm footprints and net connections pass ERC/DRC in KiCad before fabrication.  
