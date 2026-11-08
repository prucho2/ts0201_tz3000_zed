# New Firmware Prucho2

This directory documents **custom hardware profiles added to the Slacky-DIY TS0201 Zigbee temperature/humidity firmware fork**. Only boards not already supported by the upstream project are listed. Hardware pinouts were measured on physical devices with a multimeter and checked in working firmware.

## Board directory

| Board ID | Board / hardware | Sensor / protocol | Current version | Technical documentation |
|---|---|---|---|---|
| **0x2A** | Sonoff SNZB-02 | SensyLink CHT8305 | **v7.0.02** | [Sonoff SNZB-02](0x2A%20-%20Sonoff%20SNZB-02/README.md) |
| **0x2B** | ZTH01, JQ2010025-V2.0 | SHT30-compatible | **v7.0.02** | [ZTH01 JQ V2.0](0x2B%20-%20ZTH01%20JQ%20V2.0/README.md) |
| **0x2C** | GXHTV4 PC2/PC3 | SHT4x-compatible | **v7.0.04** | [GXHTV4](0x2C%20-%20GXHTV4/README.md) |

Each board folder contains its hardware description, MCU GPIO mapping and physical pin numbers, sensor selection, software/hardware I²C mode, and links to the exact source profile. **The latest selected BIN and Zigbee OTA image are archived alongside the README**; subsequent work-in-progress versions continue to build under **GitHub Actions**, without replacing these files automatically.

### Build and flashing

- `*.bin` — raw Telink firmware for SWS flashing with Telink Burning Tool (B85).
- `*.zigbee` — Zigbee OTA image; only use where the current device's manufacturer/image-type compatibility permits it.
- Before using SWS, preserve the complete original flash backup, including original device identity and factory data.
- Each board has its own Zigbee OTA image type (**0x022A**, **0x022B**, **0x022C**); never interchange binary images across different boards without verifying compatibility.

### Related tested boards

A separate eWeLink TLSR8656F512 / CHT8305 sensor with Zigbee model `CK-TLSR8656-SS5-01(7014)` was hardware-tested using the **0x2A** firmware because its tested I²C, LED and button GPIOs match Sonoff SNZB-02. It does not need a duplicate profile here.

ZG-227Z / ZG-227ZS with AHT20 is already supported by upstream Slacky-DIY as board **0x23** and is therefore not duplicated here.

### Source and attribution

Based on [Slacky-DIY original project](https://github.com/slacky1965/ts0201_tz3000_zed) and GPIO investigation by Prucho2. Consult the individual README for measured hardware details. Original source files and upstream binaries remain in their existing repository locations.

### Development workflows

- [Sonoff builds](https://github.com/prucho2/ts0201_tz3000_zed/actions/workflows/build-sonoff.yml)
- [ZTH01 builds](https://github.com/prucho2/ts0201_tz3000_zed/actions/workflows/build-zth01.yml)
- [GXHTV4 builds](https://github.com/prucho2/ts0201_tz3000_zed/actions/workflows/build-gxhtv4.yml)

The versioned BIN files are explicitly staged despite upstream `.gitignore` rules. The file archive is copied from specifically verified successful GitHub Actions artifacts by [archive-prucho2-firmware.yml](../.github/workflows/archive-prucho2-firmware.yml); this is not an automatic publishing step for every later compilation.

> Archive sync: versioned, validated binaries and OTA images from the successful workflow runs.
