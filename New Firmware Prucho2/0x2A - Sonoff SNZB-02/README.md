# Prucho2 — Sonoff SNZB-02 (board 0x2A)

Custom Slacky-DIY Zigbee temperature and humidity firmware for the original Sonoff SNZB-02 PCB.

## Verified hardware

| Function | MCU GPIO | Physical QFN32 pin | Details |
|---|---|---|---|
| I²C SDA | PC2 | 22 | Measured with multimeter |
| I²C SCL | PC3 | 23 | Measured with multimeter |
| Status LED | PB4 | 14 | Tested; active HIGH in our configuration |
| Push button | PD3 | 32 | Tested; active LOW |
| Battery measurement | PC5 | Internal ADC | Voltage sensing |
| Sensor | SensyLink CHT8305 | — | Working temperature and humidity readings |
| I²C mode | Hardware | — | `I2C_GPIO_GROUP_C2C3` |

MCU: Telink TLSR8258-family, 512 KB flash profile. The GPIO mapping was traced with a multimeter and verified on working hardware.

## Current firmware

**v7.0.02**, Zigbee OTA board/image type **0x022A**, manufacturer ID **0x6565**, firmware file version **0x70023001**. The Zigbee Basic dateCode is generated at build time.

Once the firmware archive is published, the current **BIN** and **.zigbee OTA** files are stored directly in this directory. The BIN is for Telink Burning Tool (SWS); the OTA image is for a compatible Zigbee OTA update. Back up the original 512 KB flash first, preserving factory IEEE address and calibration data.

- [Source board configuration](../../src/common/board_sonoff_snzb02_cht8305.h)
- [Build workflow](../../.github/workflows/build-sonoff.yml)
- [Build history and artifacts](https://github.com/prucho2/ts0201_tz3000_zed/actions/workflows/build-sonoff.yml)

## Confirmed compatible additional board

The same **board 0x2A v7.0.02** BIN was successfully flashed and tested on a second, distinct device: **eWeLink CK-TLSR8656-SS5-01(7014)**, with a **TLSR8656F512** processor and **CHT8305** sensor. The user measured exactly the same MCU pin mapping: SDA **PC2 / pin 22**, SCL **PC3 / pin 23**, LED **PB4 / pin 14**, pushbutton **PD3 / pin 32**.

**Both Sonoff SNZB-02 (TLSR8258) and eWeLink 7014 (TLSR8656F512) are confirmed working with the same 0x2A firmware.** This is a tested compatibility note, not a separate firmware variant. Always retain the original flash backup for each physical device.

## PCB photos

Photos from the board investigation are stored in `images/` when available. They show the actual board, not a stock example.

A model-specific PCB photograph has not yet been archived here. Do not substitute a photograph of a different board revision.
