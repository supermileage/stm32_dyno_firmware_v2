# Configuration Files

## Overview
This directory contains configuration files that define constants and enable or disable peripherals and tasks for the firmware.

---

## Files

### `config.h`
This file contains constants and general configuration settings for the firmware. It includes definitions for:
- Voltage references
- PID controller parameters
- Task delays
- Circular buffer sizes
- Peripheral configurations

### `debug.h`
This file allows enabling or disabling peripherals and tasks. It includes macros to control:
- GPIO, timers, ADC, SPI, and UART peripherals
- Task-specific settings such as enabling the Session Controller, PID Controller, and USB Controller tasks

---

## Usage
- Modify `config.h` to adjust constants and general settings for the firmware.
- Use `debug.h` to enable or disable specific peripherals and tasks as needed for debugging or deployment.

---

## Conclusion
These configuration files provide a centralized way to manage firmware settings and debugging options, ensuring flexibility and ease of maintenance.