@echo off
setlocal
cd /d "%~dp0"
echo Building Sonoff SNZB-02 CHT8305 (TLSR8258 QFN32) v7.0.01
where make >nul 2>&1
if errorlevel 1 (
 echo ERROR: GNU make not found in PATH.
 goto :failed
)
if not exist "C:\TelinkSDK\opt\tc32\bin\tc32-elf-gcc.exe" (
 echo ERROR: Telink TC32 compiler expected at C:\TelinkSDK\opt\tc32\bin
 goto :failed
)
make clean
if errorlevel 1 goto :failed
make "PROJECT_NAME=ts0201_tz3000_0x2a_zed" "PROJECT_DEF=-DBOARD=BOARD_SONOFF_SNZB02_CHT8305" "DEBUG=-DUART_PRINTF_MODE=OFF"
if errorlevel 1 goto :failed
echo.
echo Build finished. Files:
dir /b "bin\ts0201_tz3000_0x2a_zed*.bin"
echo.
echo Use .bin for SWS flashing; do NOT use .zigbee with Telink Burning Tool.
pause
exit /b 0
:failed
echo.
echo BUILD FAILED. No firmware is ready to flash.
pause
exit /b 1
