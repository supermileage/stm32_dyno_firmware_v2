@echo off
REM Scripts\build.bat
REM Configure and build the CMake project (arm-none-eabi-gcc + Ninja).
REM Optionally regenerate sources from the .ioc first if CUBEMX is set.
REM
REM Output (.elf/.hex/.bin/.map) lands in build\<CONFIG>\.

setlocal EnableDelayedExpansion

set "CONFIG=Debug"
if /i "%~1"=="Release" set "CONFIG=Release"
if /i "%~1"=="Debug"   set "CONFIG=Debug"
if /i "%~1"=="-h"      goto :usage
if /i "%~1"=="--help"  goto :usage

REM Project root is the parent of the Scripts\ directory holding this script
pushd "%~dp0.."
set "PROJECT_PATH=%CD%"

REM Required host tools
where cmake >nul 2>nul || (echo ERROR: cmake not found on PATH & popd & exit /b 1)
where ninja >nul 2>nul || (echo ERROR: ninja not found on PATH & popd & exit /b 1)
where arm-none-eabi-gcc >nul 2>nul || (echo ERROR: arm-none-eabi-gcc not found on PATH & popd & exit /b 1)

REM Optional: regenerate code from the .ioc via STM32CubeMX
if defined CUBEMX (
    if not exist "%CUBEMX%" (echo ERROR: STM32CubeMX not found at %CUBEMX% & popd & exit /b 1)
    set "IOC_FILE="
    for %%f in ("%PROJECT_PATH%\*.ioc") do set "IOC_FILE=%%f"
    if "!IOC_FILE!"=="" (echo ERROR: No .ioc file found & popd & exit /b 1)
    echo Regenerating from "!IOC_FILE!"...
    set "MX_SCRIPT=%TEMP%\cubemx_gen_%RANDOM%.txt"
    (
        echo config load !IOC_FILE!
        echo project generate
        echo exit
    ) > "!MX_SCRIPT!"
    "%CUBEMX%" -q "!MX_SCRIPT!"
    del "!MX_SCRIPT!"
)

echo Configuring (%CONFIG%)...
cmake --preset %CONFIG%
if errorlevel 1 (popd & exit /b 1)

echo Building (%CONFIG%)...
cmake --build --preset %CONFIG%
set "STATUS=%ERRORLEVEL%"

if "%STATUS%"=="0" (
    echo.
    echo Build succeeded! Artifacts in build\%CONFIG%\
)
popd
exit /b %STATUS%

:usage
echo Usage: build.bat [CONFIG]
echo   CONFIG   - Debug ^(default^) or Release
echo.
echo Environment:
echo   CUBEMX   - Path to STM32CubeMX.exe; when set, regenerates from the .ioc first
exit /b 0
