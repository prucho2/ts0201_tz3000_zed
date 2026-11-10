# Prucho2 — ZTH01 / 30201 0025-V2.0

**Board ID `0x2B`**. The **correct PCB identification is `30201 0025-V2.0`**, not the earlier transcription `JQ2010025-V2.0`. The board is known as **ZTH01**. Three matching devices were tested.

## PCB photo — telsim.pl (full resolution)

![ZTH01 PCB 30201 0025-V2.0](https://telsim.pl/dev/20260317_205051.JPG)

## Verified hardware / GPIO

| Function | MCU GPIO | Physical QFN32 pin | Notes |
|---|---|---|---|
| Software I²C SDA | **PD2** | **31** | Multimeter-traced and tested |
| Software I²C SCL | **PC3** | **23** | Multimeter-traced and tested |
| Status LED | **PC2** | **22** | Tested |
| Push button | **PB4** | **14** | Tested |
| Battery sensing | **PC5** | Internal ADC | Voltage available |
| Sensor | **SHT30-compatible** | — | Working `SENSOR_SHT30` driver; sensor marking `ML / DPDT` |
| I²C | **Software** | PD2/PC3 | Slacky-DIY bit-banged implementation |

All control GPIO values were traced with a multimeter and verified in working firmware. The device model is ZTH01 (not ZYZTH02).

## Current firmware files

**v7.0.02**, manufacturer `0x6565`, image type `0x022B`, file version `0x70023001`.

- `ts0201_tz3000_0x2b_zed_V7.0.02.bin` — BDT/SWS.
- `6565-022b-70023001-ts0201_tz3000_0x2b_zed.zigbee` — Zigbee OTA.

Both files are included in this folder. Preserve original flash before SWS programming.

## Code and build

- [Working board pin configuration](../../src/common/board_zth01_jq_v20.h) — legacy source filename retained for build compatibility; its name does **not** reflect the corrected silkscreen.
- [GitHub Actions build](../../.github/workflows/build-zth01.yml)
