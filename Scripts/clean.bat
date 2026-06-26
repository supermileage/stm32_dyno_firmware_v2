@echo off
REM Scripts\clean.bat
REM Remove CMake build output. Does NOT touch generated C sources (tracked in git).

setlocal EnableDelayedExpansion

pushd "%~dp0.."
set "PROJECT_PATH=%CD%"
popd

echo Cleaning build output in %PROJECT_PATH% ...

if exist "%PROJECT_PATH%\build" (
    echo   removing build\
    rmdir /s /q "%PROJECT_PATH%\build"
)

REM Legacy STM32CubeIDE artifacts, if any remain from before the CMake migration
for %%d in (Debug Release) do (
    if exist "%PROJECT_PATH%\%%d" (
        echo   removing %%d\ ^(legacy CubeIDE output^)
        rmdir /s /q "%PROJECT_PATH%\%%d"
    )
)
if exist "%PROJECT_PATH%\build_log.txt" (
    echo   removing build_log.txt
    del /q "%PROJECT_PATH%\build_log.txt"
)

echo Clean complete.
exit /b 0
