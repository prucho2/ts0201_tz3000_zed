# Sonoff SNZB-02 (CHT8305) – custom Slacky-DIY firmware

Status: **source profile ready; NOT yet compiled or hardware-tested**.

This fork adds a separate board ID rather than modifying upstream Slacky-DIY boards.

## Hardware – traced and measured

| Signal | GPIO | TLSR8258 QFN32 physical pin | Evidence |
| --- | --- | --- | --- |
| CHT8305 SDA | PC2 | 22 | PCB trace / user's continuity check |
| CHT8305 SCL | PC3 | 23 | PCB trace / user's continuity check |
| LED | PB4 | 14 | PCB trace / user's continuity check |
| Button SW1 | PD3 | 32 | SW1 pressed: ~1 kOhm to GND through R2 |
| Battery | upstream floating-pin ADC VBAT mode | not yet validated | needs comparison against actual battery voltage |

Sensor U3 marking: 8305 (CHT8305). No new I2C sensor driver is required.

## Build

Run `build-sonoff-snzb02.cmd` in the repo root on Windows with GNU make,
Python 3 and Telink TC32 toolchain installed at `C:\TelinkSDK\opt\tc32\bin`.

Equivalent make command:

```sh
make PROJECT_NAME=ts0201_tz3000_0x2a_zed PROJECT_DEF=-DBOARD=BOARD_SONOFF_SNZB02_CHT8305 DEBUG=-DUART_PRINTF_MODE=OFF
```

Generated **raw firmware for SWS**:

`bin/ts0201_tz3000_0x2a_zed_last_version.bin`

The Zigbee OTA container (`.zigbee`) is NOT an SWS flashing image.
Do not flash the upstream precompiled binaries – they target different boards.

## Identity and update separation

- Standard Zigbee model: `TS0201-z-SlD`
- Zigbee manufacturer: `Slacky-DIY`
- New firmware board ID: `0x2A`
- Firmware label: `v7.0.01`
- OTA Manufacturer Code: `0x6565`
- OTA Image Type (8258, 512k configuration): `0x022A`
- OTA File Version: `0x70013001`

The existing upstream Zigbee clusters, attributes and converter
compatibility are intentionally preserved. Future updates are manual.

## SWS first flash – important

1. **Back up the entire original flash using Telink Burning Tool before writing.**
   Preserve factory IEEE/EUI-64, calibration and any pairing/identity data.
2. Confirm exact MCU and flash size in the programmer; the fork defaults
   to the upstream 512 KB TLSR8258 build configuration.
3. Flash only the appropriate app region using a verified raw `.bin`.
   Do not mass erase without a verified full backup.
4. Test temperature, humidity, Zigbee join, button wakeup, LED polarity,
   battery voltage, sleep current and OTA identity.
5. Do not flash the remaining sensors until the first sample passes.

The battery ADC method is inherited from Slacky-DIY and **is not yet validated**
on this specific PCB. The absence of an externally bonded PC5 pin on QFN32
does not by itself prove whether the SDK's floating-input VBAT method works.
