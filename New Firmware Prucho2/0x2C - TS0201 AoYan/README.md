# Prucho2 — TS0201 AoYan

**Board ID `0x2C`**. Actual device/project name: **TS0201 AoYan**. The earlier folder name *GXHTV4 PC2/PC3* described the sensor protocol and GPIO mapping, **not** the device model.

## Photos — hosted on telsim.pl (full resolution)

**PCB:**

![TS0201 AoYan physical PCB](https://telsim.pl/dev/20261108_165025.JPG)

**Zigbee2MQTT device view:**

![TS0201 AoYan Zigbee2MQTT](https://telsim.pl/dev/AoYan-2026-11-10-204156.png)

## Verified hardware / GPIO

| Function | MCU GPIO | Physical QFN32 pin | Notes |
|---|---|---|---|
| Software I²C SDA | **PC2** | **22** | Multimeter-traced and tested |
| Software I²C SCL | **PC3** | **23** | Multimeter-traced and tested |
| Status LED | **PB4** | **14** | Tested, active HIGH |
| Push button | **PC0** | **20** | Tested, active LOW |
| Battery sensing | **PC5** | Internal ADC | Voltage available |
| Sensor | **GXHTV4 / SHT4x protocol** | — | Marked `GV4 / 5357`; working with SHT40 driver |
| I²C mode | **Software** | PC2/PC3 | Same bit-banged implementation as Slacky-DIY |

TLSR8258-family board, no LCD. GPIO assignments were measured with a multimeter and verified on the physical sensor.

## Current firmware files

**v7.0.04**, manufacturer `0x6565`, image type `0x022C`, file version `0x70043001`.

- `ts0201_tz3000_0x2c_zed_V7.0.04.bin` — BDT/SWS.
- `6565-022c-70043001-ts0201_tz3000_0x2c_zed.zigbee` — Zigbee OTA.

Both files are kept in this folder.

### Firmware history

| Version | Changes |
|---|---|
| v7.0.02 | First working sensor readings using hardware I²C |
| v7.0.03 | I²C reinitialisation, retry and protection against invalid reports |
| **v7.0.04** | Switched to **software I²C** on PC2/PC3 for improved stability |

## Code and build

- [Working board pin configuration](../../src/common/board_gxhtv4_pc2pc3.h) — legacy source filename preserved for compiler compatibility.
- [GitHub Actions build](../../.github/workflows/build-gxhtv4.yml)
