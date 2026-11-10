# Prucho2 — SONOFF-SNZB-02-_TZ3000_fllyghyj & eWeLink CK-TLSR8656-SS5-01(7014)

**Board ID `0x2A` — same working firmware for TWO different devices.** Both device types have been flashed and tested with the identical **v7.0.02** image. No separate image is needed for eWeLink.

## Photos — hosted on telsim.pl (full resolution)

### SONOFF-SNZB-02-_TZ3000_fllyghyj

**PCB:**

![SONOFF SNZB-02 TZ3000_fllyghyj PCB](https://telsim.pl/dev/sonoff-snzb-02-_tz3000_fllyghyj.png)

**Zigbee2MQTT device view:**

![SONOFF SNZB-02 Zigbee2MQTT](https://telsim.pl/dev/snzb02.jpg)

### eWeLink CK-TLSR8656-SS5-01(7014)

**PCB:**

![eWeLink CK-TLSR8656-SS5-01(7014) PCB](https://telsim.pl/dev/ewelink-ck-tlsr8656-ss5-01%287014%29.png)

**Zigbee2MQTT device view:**

![eWeLink Zigbee2MQTT](https://telsim.pl/dev/evelink.png)

## Verified GPIO — identical on both boards

| Function | GPIO | Physical QFN32 pin | Verification |
|---|---|---|---|
| Sensor I²C SDA | **PC2** | **22** | Measured with multimeter; working |
| Sensor I²C SCL | **PC3** | **23** | Measured with multimeter; working |
| Status LED | **PB4** | **14** | Measured; LED operates in firmware |
| Push button | **PD3** | **32** | Measured; button operates in firmware |
| Battery sensing | **PC5** | Internal ADC | Battery voltage available |
| Temperature/humidity sensor | **CHT8305** | — | Working measurements |
| I²C mode | **Hardware I²C** | PC2/PC3 | Telink `I2C_GPIO_GROUP_C2C3` |

- **SONOFF** processor: **TLSR8258** family.
- **eWeLink** processor: **TLSR8656F512** (512 KB flash).
- Tested identical GPIO mapping and operation on both physical boards.

## Current firmware files

**v7.0.02**, manufacturer `0x6565`, image type `0x022A`, file version `0x70023001`.

- `ts0201_tz3000_0x2a_zed_V7.0.02.bin` — BDT/SWS.
- `6565-022a-70023001-ts0201_tz3000_0x2a_zed.zigbee` — Zigbee OTA image (check compatibility with existing firmware).

Both current files are stored **in this folder**. Preserve original Flash/IEEE data before SWS flashing.

## Code and build

- [Working board pin configuration](../../src/common/board_sonoff_snzb02_cht8305.h)
- [GitHub Actions build](../../.github/workflows/build-sonoff.yml)
