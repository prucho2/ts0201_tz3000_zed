# Prucho2 — ZTH01 / JQ2010025-V2.0 (board 0x2B)

Custom Slacky-DIY Zigbee temperature and humidity firmware for the ZTH01 PCB marked `JQ2010025-V2.0`. Three matching devices have been tested.

## Verified hardware

| Function | MCU GPIO | Physical QFN32 pin | Details |
|---|---|---|---|
| Software I²C SDA | PD2 | 31 | Multimeter traced; tested |
| Software I²C SCL | PC3 | 23 | Multimeter traced; tested |
| Status LED | PC2 | 22 | Tested |
| Push button | PB4 | 14 | Tested |
| Battery measurement | PC5 | Internal ADC | Voltage sensing |
| Sensor | SHT30-compatible | — | Temperature and humidity readings verified |
| I²C mode | Software | — | Slacky-DIY software I²C implementation |

Sensor's original package marking was `ML / DPDT`; the working driver is `SENSOR_SHT30` (SHT3x-compatible protocol). The board model is **ZTH01**, not ZYZTH02; an existing PVVX profile helped cross-check its GPIO map.

The GPIO assignments were traced with a multimeter and confirmed on real hardware.

## Current firmware

**v7.0.02**, Zigbee OTA board/image type **0x022B**, manufacturer ID **0x6565**, firmware file version **0x70023001**.

The current BIN and OTA image will be archived here. Use the BIN for SWS flashing after a complete flash backup. Upgrading from a different firmware family via OTA may be incompatible because image identifiers differ.

- [Source board configuration](../../src/common/board_zth01_jq_v20.h)
- [Build workflow](../../.github/workflows/build-zth01.yml)
- [Build history and artifacts](https://github.com/prucho2/ts0201_tz3000_zed/actions/workflows/build-zth01.yml)

## PCB photos

![Photographed ZTH01 JQ2010025 V2.0 PCB](images/pcb.jpg)

Photo taken from the actual board under investigation. A small compressed preview is included; original high-resolution pictures remain outside this repository.
