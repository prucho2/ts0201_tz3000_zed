# New Firmware Prucho2

Custom boards added to the [Slacky-DIY TS0201 Zigbee temperature and humidity firmware](https://github.com/slacky1965/ts0201_tz3000_zed), with GPIO traced using a multimeter and verified on real hardware.

## Hardware projects

| Board | Supported device(s) | Sensor/driver | Current firmware | Details |
|---|---|---|---|---|
| **0x2A** | **SONOFF-SNZB-02-_TZ3000_fllyghyj** and **eWeLink CK-TLSR8656-SS5-01(7014)** — **one shared firmware for both** | CHT8305 | **v7.0.02** | [Hardware, pinout and photos](0x2A%20-%20SONOFF-SNZB-02-_TZ3000_fllyghyj%20%2B%20eWeLink%207014/README.md) |
| **0x2B** | **ZTH01, PCB 30201 0025-V2.0** | SHT30-compatible | **v7.0.02** | [Hardware, pinout and photo](0x2B%20-%20ZTH01%2030201%200025-V2.0/README.md) |
| **0x2C** | **TS0201 AoYan** | GXHTV4 / SHT4x-compatible | **v7.0.04** | [Hardware, pinout and photos](0x2C%20-%20TS0201%20AoYan/README.md) |

These are **device/project names**, independent of the source-code identifiers used for builds. Existing board IDs, firmware versions, BIN and OTA contents are unchanged.

Each project folder contains its current **.bin** (Telink BDT/SWS) and **.zigbee** (OTA image), plus detailed GPIO notes. Future experimental builds remain in [GitHub Actions](https://github.com/prucho2/ts0201_tz3000_zed/actions), without automatic replacement of these known-good archives.

All project photos are linked **directly to full-resolution images hosted by telsim.pl** (not stored as small copies in GitHub).

## Compatibility and programming

- **0x2A** has been flashed and verified on **two physically different boards**: SONOFF-SNZB-02-_TZ3000_fllyghyj (TLSR8258) and eWeLink CK-TLSR8656-SS5-01(7014) (TLSR8656F512).
- Back up the original flash and factory IEEE/calibration data before flashing via SWS.
- OTA images have distinct image types **0x022A / 0x022B / 0x022C**; validate manufacturer and image type before OTA.
- The **ZG-227Z / ZG-227ZS (AHT20)** already has a matching original Slacky-DIY profile **0x23** and does not need a duplicate here.

## Source code and builds

| Board | GPIO profile in source | Build workflow |
|---|---|---|
| 0x2A | [board_sonoff_snzb02_cht8305.h](../src/common/board_sonoff_snzb02_cht8305.h) | [Sonoff build](../.github/workflows/build-sonoff.yml) |
| 0x2B | [board_zth01_jq_v20.h](../src/common/board_zth01_jq_v20.h) | [ZTH01 build](../.github/workflows/build-zth01.yml) |
| 0x2C | [board_gxhtv4_pc2pc3.h](../src/common/board_gxhtv4_pc2pc3.h) | [AoYan build (GXHTV4 board profile)](../.github/workflows/build-gxhtv4.yml) |

BIN/OTA archiving is manual through [archive-prucho2-firmware.yml](../.github/workflows/archive-prucho2-firmware.yml) and does not run for each new build.
