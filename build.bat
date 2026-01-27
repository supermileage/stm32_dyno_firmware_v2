@echo off
REM build.bat
REM Build STM32CubeIDE project headless on Windows

REM Check for required arguments
if "%1"=="" (
    echo ERROR: Missing required arguments
    echo.
    echo Usage: build.bat ^<CUBEIDE_PATH^> ^<WORKSPACE_PATH^> [CONFIG] [clean]
    echo.
    echo Arguments:
    echo   CUBEIDE_PATH   - Path to stm32cubeide.exe
    echo   WORKSPACE_PATH - Path to workspace directory
    echo   CONFIG         - Debug or Release ^(optional, default: Debug^)
    echo   clean          - Perform clean build ^(optional^)
    echo.
    echo Examples:
    echo   ./build.bat "C:\STM32CubeIDE\STM32CubeIDE_1.14.1\STM32CubeIDE\stm32cubeide.exe" "C:\STM32CubeIDE\Projects"
    echo   ./build.bat "C:\STM32CubeIDE\STM32CubeIDE_1.14.1\STM32CubeIDE\stm32cubeide.exe" "C:\STM32CubeIDE\Projects" Release
    echo   ./build.bat "C:\STM32CubeIDE\STM32CubeIDE_1.14.1\STM32CubeIDE\stm32cubeide.exe" "C:\STM32CubeIDE\Projects" Debug clean
    exit /b 1
)

if "%2"=="" (
    echo ERROR: Missing workspace path
    echo Usage: build.bat ^<CUBEIDE_PATH^> ^<WORKSPACE_PATH^> [CONFIG] [clean]
    exit /b 1
)

REM Get required parameters
set "CUBEIDE=%~1"
set "WORKSPACE=%~2"
set "PROJECT_PATH=%CD%"

REM Default build configuration
set "CONFIG=Debug"
set "BUILD_TYPE=build"
set "BUILD_CMD=-build"

REM Parse optional arguments
if "%3"=="Debug" set "CONFIG=Debug"
if "%3"=="Release" set "CONFIG=Release"

if /i "%4"=="clean" (
    set "BUILD_TYPE=cleanBuild"
    set "BUILD_CMD=-cleanBuild"
)

REM Validate CubeIDE path
if not exist "%CUBEIDE%" (
    echo ERROR: STM32CubeIDE not found at %CUBEIDE%
    exit /b 1
)

REM Create workspace directory if it doesn't exist
if not exist "%WORKSPACE%" (
    echo Creating workspace directory...
    mkdir "%WORKSPACE%"
)

REM Extract project name
for %%f in ("%PROJECT_PATH%") do set "PROJECT_NAME=%%~nxf"

echo ===============================
echo CubeIDE path: %CUBEIDE%
echo Building project: %PROJECT_NAME%
echo Configuration: %CONFIG%
echo Workspace: %WORKSPACE%
echo Project path: %PROJECT_PATH%
echo Build type: %BUILD_TYPE%
echo ===============================

REM Delete old log
if exist build_log.txt del build_log.txt

REM Import and build in single command
echo Building...
"%CUBEIDE%" -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild -data "%WORKSPACE%" -import "%PROJECT_PATH%" %BUILD_CMD% "%PROJECT_NAME%/%CONFIG%" > build_log.txt 2>&1

echo.

REM Print log to terminal
type build_log.txt

echo.
if %ERRORLEVEL%==0 (
    echo Build succeeded!
) else (
    echo Build failed with status %ERRORLEVEL%
)