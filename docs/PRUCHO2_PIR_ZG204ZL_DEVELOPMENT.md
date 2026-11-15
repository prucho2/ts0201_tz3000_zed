# Prucho2 PIR (ZG-204ZL) — Slacky-DIY firmware development

**Status:** DEVELOPMENT, not compiled, not validated on hardware, DO NOT FLASH.  
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

**Single hardware design, single firmware build:** the PCB, MCU, pin mapping and Zigbee configuration are identical regardless of whether the illuminance components are fitted. The same firmware must work unchanged on both. As already observed in the current PVVX devices, units without populated illuminance parts report `0 lux`; do not introduce separate builds, model IDs, board profiles or configuration switches to distinguish them. Verify that the readout of an unpopulated lux circuit is safe, bounded and does not delay PIR operation.

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
- **C — New event module:** Slacky-style PIR state machine and wake-up integration, retain original Slacky Zigbee networking.
- **D — Clusters / reporting:** occupancy 0x0406 and illuminance 0x0400, config/diagnostic attributes, Z2M converter when necessary.
- **E — Build:** separate GitHub Actions artifact with unique **development** version and OTA image type; do not archive it among known-good TH firmware.
- **F — Hardware:** backup full raw flash, program the **disassembled third test PIR only**, join TEST coordinator, test the **same BIN** on both lux-populated and unpopulated ZG-204ZL boards, GPIO and 1s/3s/35s occupancy, Z2M reports, power draw and OTA. No MAIN restarts; do not modify MAIN PIRs.

## Slacky code references

- [TH baseline](https://github.com/slacky1965/ts0201_tz3000_zed)
- [Door sensor (GPIO wake, debouncing, IAS notifications)](https://github.com/slacky1965/tuya_door_sensor_ts0203_zed)
- [Water leak (wakeup, event-state timers)](https://github.com/slacky1965/ts0207_tz3000_zed)
- [Water meter (reporting min/max timers)](https://github.com/slacky1965/watermeter_zed)
- [PVVX wiring reference](https://github.com/prucho2/ZigbeeTLc-PIR/blob/master/src/board_zg204zl.h)

## Acceptance gate for a usable BIN

No flashing until a **successful clean build**, explicit firmware metadata check (version, Zigbee manufacturer, OTA image type, dateCode), confirmed GPIO polarity/sleep behavior, and test-device-only rollout plan. A profile or stub alone is **not a usable firmware image**.
