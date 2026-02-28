# STM32 Dyno Firmware v2

## Overview
This repository contains the firmware for the STM32-based dynamometer project. It is designed to work with the STM32H743IITX microcontroller and leverages FreeRTOS for real-time task management.

## Cloning the Repository
To clone this repository, ensure you include all submodules:

```bash
git clone --recurse-submodules <repository-url>
```

If you forget to use the `--recurse-submodules` flag, you can initialize and update the submodules later with:

```bash
git submodule update --init --recursive
```

## Requirements
1. **STM32CubeIDE**: Download and install STM32CubeIDE from [ST's official website](https://www.st.com/en/development-tools/stm32cubeide.html).
2. **Windows OS**: The `build.bat` script is designed to work only on Windows.

## Building the Project
### Using STM32CubeIDE
1. Open STM32CubeIDE.
2. Import the project into your workspace.
3. Build the project using the desired configuration (Debug/Release).

### Using `build.bat` (Windows Only)
If you do not need to modify the `.ioc` file for code generation, you can build the project without opening STM32CubeIDE:

1. Close STM32CubeIDE if it is running.
2. Run the `build.bat` script:

   ```cmd
   build.bat <CUBEIDE_PATH> <WORKSPACE_PATH> [CONFIG] [clean]
   ```

   - Replace `<CUBEIDE_PATH>` with the path to `stm32cubeide.exe`.
   - Replace `<WORKSPACE_PATH>` with the path to your workspace directory.
   - `[CONFIG]` is optional and can be `Debug` (default) or `Release`.
   - `[clean]` is optional and performs a clean build.

   Example:

   ```cmd
   build.bat "C:\STM32CubeIDE\STM32CubeIDE_1.14.1\STM32CubeIDE\stm32cubeide.exe" "C:\STM32CubeIDE\Projects" Debug clean
   ```

3. The build log will be saved to `build_log.txt`.

## Flashing the Firmware
Flashing via the command line interface (CLI) will be added in a future update.

## Notes
- Ensure all submodules are initialized and updated before building the project.
- The `build.bat` script is only compatible with Windows systems.