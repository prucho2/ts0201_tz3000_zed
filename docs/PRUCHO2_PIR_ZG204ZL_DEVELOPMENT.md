# Prucho2 PIR (ZG-204ZL) — Slacky-DIY firmware development

**Status:** **v0.1.41 SUCCESSFULLY BUILT**, firmware metadata validated in GitHub Actions, but NOT tested on real hardware — **DO NOT FLASH ON MAIN DEVICES**.  
**Target:** Telink TLSR8253, hardware designation **ZG-204ZL**, **one identical PCB and one firmware image**. Some units have the lux sensor components soldered; others have the same component footprints unpopulated.  
**Working branch:** `dev/prucho2-pir-zg204zl` of `prucho2/ts0201_tz3000_zed`. Production `main` and all existing TH board builds remain unchanged.

## Key decision: Slacky firmware, NOT a PVVX derivative

- Base firmware, Zigbee stack, commissioning, networking, config persistence, power management scheduling, reporting architecture, battery, OTA and error handling: **Slacky-DIY**.
- Event-driven implementation lessons from **slacky1965/tuya_door_sensor_ts0203_zed**, **slacky1965/ts0207_tz3000_zed**, and reporting patterns from **slacky1965/watermeter_zed**.
- **PVVX/ZigbeeTLc** is a reference for **ZG-204ZL wiring, PIR and illuminance electrical behavior**, including GPIO wake-up. Do **not** copy PVVX `reporting.c` or application event loop.
- No temperature/humidity controller, Tuya logic, or PVVX-specific network/reporting mechanisms.

## Hardware reference (from prucho2/ZigbeeTLc-PIR `src/board_zg204zl.h`)

| Function | Pin | Electrical behavior |
|---|---|---|
| PIR digital output | **PD4** | **Active HIGH**, must wake device on event |
| Illuminance measurement | **PC4** | Analog ADC **C4P** |
| Illuminance sensor power | **PD3** | Active HIGH; switched off after reading |
| Status LED | **PD7** | Active LOW |
| Pairing button | **PA1** | Active LOW |
| Battery measurement | **PC5** | ADC C5P profile; validate on actual board |
| CPU | **TLSR8253** | Confirm memory/flash config and toolchain compatibility before compiling |

**Single hardware design, single firmware build:** the PCB, MCU, pin mapping and Zigbee configuration are identical regardless of whether the illuminance components are fitted. The same firmware must work unchanged on both. As already observed in the current PVVX devices, units without populated illuminance parts report `0 lux`; do not introduce separate builds, model IDs, board profiles or configuration switches to distinguish them. This is established behavior of the existing factory/PVVX firmware: the same firmware runs on both and units with no lux components simply report `0 lux`. No hardware-population checks or separate lux tests are needed.

## Required ZCL behavior

1. **Occupancy Sensing (0x0406):** report `occupancy=1` promptly after validated GPIO motion; report `occupancy=0` once no further PIR activity and occupancy hold expires.
2. **Illuminance Measurement (0x0400):** Slacky reporting, configurable sample interval and reportable change; the same image exposes this on both hardware populations and non-populated sensors report `0 lux`, as in the existing PVVX firmware.
3. **Power Configuration (0x0001)** and **Basic (0x0000)** from Slacky.
4. Configurable occupancy hold; start with **3 seconds** on TEST, evaluate 1-second performance separately.
5. Distinguish raw PIR GPIO state, derived occupancy state, outstanding ZCL report state and transmission success.
6. Bound and reschedule reports following unavailable network, APS queue pressure or failed confirmation. Do not block GPIO processing on Zigbee.
7. Periodically reconcile occupancy and reported state, without blindly issuing `unoccupied` while the hardware is actively detecting motion.
8. Provide diagnostics for raw rises/falls, timer arm/fires/cancels, state changes, send attempts/confirmations/failures and wake cause (RAM counters to start).

## Known PVVX problem to investigate, not yet resolved

Existing PVVX PIRs could remain `occupied` for hours when hold was set to 1 second. Changing to 3 seconds eventually cleared the problem; the immediate change did not fix existing stuck state. Possible causes include timer scheduling, source GPIO held high, event/retention sleep behavior, or delayed/missing report. The observation is **not proof** of any single cause.

PVVX `sensor_pir.c`: `occupancyTimerCb()` keeps occupancy high and returns 0 if GPIO is still asserted; it requests reporting only when occupancy is actually cleared. The effective callback-return semantics and sleep/timer lifetime must be verified in Telink SDK rather than assumed.

## Implementation stages

- **A — Baseline / preflight:** inventory Slacky TS0201 ZCL registration, reporting callbacks, `app_pm.c`, timer behavior, boot/init retention semantics, available flash and SDK TLSR8253 support.
- **B — Board profile:** add conditional ZG-204ZL GPIO header, power/ADC and battery profile; keep original TH builds untouched.
- **C — New event module:** first implementation committed: Slacky-style PIR state machine, GPIO wake, occupancy ZCL reports, timer checks and periodic synchronization. Not yet validated on hardware.
- **D — Clusters / reporting:** occupancy 0x0406 and illuminance 0x0400 added as initial code. Lux sampled every 10 seconds through SDK ADC, configurable ZCL occupancy delay; further Z2M validation and calibration are pending.
- **E — Build:** added .github/workflows/build-prucho2-pir.yml for experimental board **0x2D**, file version **0x01413001**, image type **0x022D**, v0.1.41. Successful TC32 compilation confirmed on 2026-11-15 by [run 43752694975](https://github.com/prucho2/ts0201_tz3000_zed/actions/runs/43752694975), with firmware image type 0x022D and version 0x01413001. Archived only as an **UNTESTED workflow artifact**, not in the known-good firmware directory.
- **F — Zigbee2MQTT:** initial converter [zigbee2mqtt/prucho2_pir_zg204zl.mjs](../zigbee2mqtt/prucho2_pir_zg204zl.mjs) using occupancy, illuminance and battery modern extends plus 1–600s hold setting. Not deployed or hardware-tested.
- **G — Hardware:** backup full raw flash, program the **disassembled third test PIR only**, join TEST coordinator, test PIR GPIO and 1s/3s/35s occupancy on the disassembled TEST unit, Z2M reports, power draw and OTA. No MAIN restarts; do not modify MAIN PIRs.

## Slacky code references

- [TH baseline](https://github.com/slacky1965/ts0201_tz3000_zed)
- [Door sensor (GPIO wake, debouncing, IAS notifications)](https://github.com/slacky1965/tuya_door_sensor_ts0203_zed)
- [Water leak (wakeup, event-state timers)](https://github.com/slacky1965/ts0207_tz3000_zed)
- [Water meter (reporting min/max timers)](https://github.com/slacky1965/watermeter_zed)
- [PVVX wiring reference](https://github.com/prucho2/ZigbeeTLc-PIR/blob/master/src/board_zg204zl.h)

## Acceptance gate for a usable BIN

No flashing until a **successful clean build**, explicit firmware metadata check (version, Zigbee manufacturer, OTA image type, dateCode), confirmed GPIO polarity/sleep behavior, and test-device-only rollout plan. A profile or stub alone is **not a usable firmware image**.

## First confirmed successful build (2026-11-15)

- [GitHub Actions run #43752694975](https://github.com/prucho2/ts0201_tz3000_zed/actions/runs/43752694975): **success**, independent build only.
- Artifact: `prucho2-pir-zg204zl-v0.1.41-UNTESTED` (ZIP with BIN and OTA, temporary Actions storage).
- OTA file: `6565-022d-01413001-prucho2_zg204zl_pir.zigbee`.
- Version: `v0.1.41`, manufacturer `0x6565`, image type `0x022D`, image file version `0x01413001`.
- Compiler/linker validated, binary produced. `text 151460 / data 816 / bss 13268` bytes shown by linker size tool.
- **Unverified:** live motion events, sleep/wakeup energy and edge handling, lux ADC physical accuracy, end-to-end Zigbee2MQTT reports, converter behavior, OTA update path. The first hardware trial is permitted only on isolated TEST device after backup and review.

## 2026-11-16 — isolated source baseline v0.1.41 with PVVX PIR + full diagnostics

Approved scope: use source exactly from v0.1.41 commit `5c41888e7f85a8888490b4ed1abd6c1ca75b26a2` for lux/PM/ZCL, plus PVVX PIR motion logic and full automatic 11-counter diagnostic snapshots. Separate branch `dev/prucho2-pir-pvvx-v141-baseline` leaves previous v0.1.46 source intact.

- Lux implementation `src/app_pir_lux_prucho2.c` **unchanged from v0.1.41**: sample every 10 seconds; change threshold 10 Zigbee ZCL units; hourly fallback sync. No 50-lx threshold, no 60-second periodic reporting added. ZCL illuminance measuredValue remains RR/reportable; converter uses stock v0.1.41 `illuminance({reporting: {min: '10_SECONDS', max: '1_HOUR', change: 10}})` plus full diagnostic decoding.
- PVVX `src/sensor_pir.c` occupancy lifecycle: PD4 active HIGH; on HIGH entry set occupancy=1 and arm hold timer; timer callback keeps occupied while HIGH (reschedule via return 0), clears when LOW after hold; each application sample arms `cpu_set_gpio_wakeup(PD4, !active, 1)`. Keep Slacky pushbutton wake separately; `prucho2_pir_prepare_sleep()` now touches PA1 only to prevent overriding PVVX PD4.
- All 11 counters and combined automatic diagnostic ZCL report 0xF100..0xF10A retained; event-driven after initial join and observed PD4 transitions; 5s coalescing; numeric GET support remains.
- The new image is v0.1.47 (`0x01473001`) **solely as an OTA-compatible version bump over existing 0.1.46**. It is constructed from 1.41 sources, not from 1.46.
- No live device flashing or HA changes. Build must pass and hardware functionality remains unverified.
