# STM32 Dyno Firmware v2

## Overview
This repository contains the firmware for the STM32-based dynamometer project. It
targets the **STM32H743IITx** microcontroller and uses FreeRTOS for real-time task
management. The project is built with **CMake + Ninja** and the Arm GNU toolchain
(`arm-none-eabi-gcc`); code is generated from the `.ioc` with **STM32CubeMX**.

## Cloning the Repository
Include all submodules when cloning:

```bash
git clone --recurse-submodules <repository-url>
```

If you forgot the flag, initialize the submodules afterwards:

```bash
git submodule update --init --recursive
```

## Requirements
You only need these to **build**:

| Tool | Purpose | Install (Fedora) | Install (Ubuntu/Debian) |
|------|---------|------------------|--------------------------|
| Arm GNU toolchain | Compiler/linker | `sudo dnf install arm-none-eabi-gcc-cs arm-none-eabi-newlib` | `sudo apt install gcc-arm-none-eabi` |
| CMake (≥ 3.22) | Build system | `sudo dnf install cmake` | `sudo apt install cmake` |
| Ninja | Build backend | `sudo dnf install ninja-build` | `sudo apt install ninja-build` |

Additional, only if you need them:
- **STM32CubeMX** — to regenerate code after editing `stm32_dyno_firmware_v2.ioc`
  ([download](https://www.st.com/en/development-tools/stm32cubemx.html)).
- **STM32CubeProgrammer** or **stlink** — to flash the board.

## Building the Project

### Using the scripts (recommended)
```bash
./Scripts/build.sh            # Debug (default)
./Scripts/build.sh Release    # Release
./Scripts/clean.sh            # remove build output
```
On Windows use the `.bat` equivalents in `Scripts/`.

Build output is written to `build/<CONFIG>/`:
- `stm32_dyno_firmware_v2.elf`
- `stm32_dyno_firmware_v2.hex`
- `stm32_dyno_firmware_v2.bin`
- `stm32_dyno_firmware_v2.map`

### Using CMake directly
```bash
cmake --preset Debug
cmake --build --preset Debug
```
Presets (`Debug`, `Release`) are defined in `CMakePresets.json`; the Arm toolchain
file is `cmake/gcc-arm-none-eabi.cmake`.

### Using Docker (reproducible, matches CI exactly)
Requires only Docker — no host toolchain. The `Dockerfile` pins the Arm GNU
toolchain, CMake and Ninja, and CI builds inside this same image:
```bash
./Scripts/build-docker.sh            # Debug
./Scripts/build-docker.sh Release
```
The repo is bind-mounted, so output still lands in `build/<CONFIG>/` on the host.
On Windows run it from Git Bash/WSL. (On SELinux hosts the script adds the
required `:z` mount option automatically.)

## Regenerating Code from the `.ioc`
The toolchain in the `.ioc` is set to **CMake**. After editing the design in
STM32CubeMX, click **Generate Code** to refresh the HAL/driver sources and
`cmake/stm32cubemx/CMakeLists.txt`. Your edits in the top-level `CMakeLists.txt`
(and inside `USER CODE BEGIN/END` blocks) are preserved.

To regenerate headlessly from the command line, set `CUBEMX` before building:
```bash
CUBEMX="/path/to/STM32CubeMX" ./Scripts/build.sh Debug
```

## Flashing the Firmware
Flash to address `0x08000000` over SWD/ST-Link. With **stlink**:
```bash
st-flash write build/Debug/stm32_dyno_firmware_v2.bin 0x08000000
```
Or with **STM32CubeProgrammer**:
```bash
STM32_Programmer_CLI -c port=SWD -d build/Debug/stm32_dyno_firmware_v2.elf -rst
```

## Continuous Integration
`.github/workflows/build.yml` builds both `Debug` and `Release` with CMake on every
push/PR and uploads the resulting firmware as workflow artifacts.

## Notes
- Ensure all submodules are initialized and updated before building.
- The build is IDE-independent. The project can still be opened in STM32CubeIDE
  1.15+ via **File → Import → Import CMake Project**, but that is optional.
