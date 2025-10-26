# AxisDriver Project

## Overview
The AxisDriver project is a comprehensive solution for controlling motorized devices, designed for robotics and automation applications. It integrates firmware, software, and hardware components to provide a robust and flexible platform for motor control and peripheral management.

## Directory Structure
- `firmware/`: Firmware source code, libraries, and PlatformIO configuration.
- `examples/Python/`: Python scripts for hardware interfacing and control.
- `hardware/`: KiCad files for PCB designs, including Ethernet HATs and mainboards.
- `test/`: Unit tests for hardware components.
- `scripts/`: Utility scripts for generating interfaces and managing configurations.

## Getting Started

### Building the Firmware
1. Navigate to the `firmware/` directory.
2. Build the firmware using PlatformIO:
   ```bash
   platformio run
   ```

### Running Python Scripts
- There are a few python scripts that can control motors over ethernet or USB
1. Navigate to the `examples/Python/` directory.
2. Run the desired script using Python:
   ```bash
   python3 <script_name>.py
   ```

### Ethernet Debugging
- If DEBUG_UDP is defined, run the following in a linux terminal to see messages
```
nc -u -lk 35461
```