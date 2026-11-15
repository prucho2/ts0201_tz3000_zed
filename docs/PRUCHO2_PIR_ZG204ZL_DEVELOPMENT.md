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


## TEST hardware finding, 15 November 2026: v0.1.41 occupancy stuck at zero

- Test PIR joined Zigbee2MQTT Edge as `Prucho2-PIR-z`, FW `v0.1.41`; illuminance updated normally.
- Direct ZCL read of endpoint 1, cluster `msOccupancySensing`, attribute `occupancy` returned **0**, even though user moved in front of PIR.
- The converter, standard Zigbee endpoint and ZCL attribute are present. This alone does not distinguish absent PD4 changes, missed wakeup, short pulses or state-machine failure.
- PVVX functioned on the same hardware, so distinguish PVVX GPIO/PM behavior from our Slacky-based event handling; do not attribute this to a different PCB or missing lux hardware.
- **v0.1.42 diagnostic:** read-only attributes on endpoint 1 / cluster 0x0406. Read their **numeric IDs** in Z2M Dev console. These counters are RAM counters; battery cold boot resets them.
  - `0xF100` UINT8 current sampled PD4 level (0/1)
  - `0xF101` UINT32 rising edges (observed)
  - `0xF102` UINT32 falling edges (observed)
  - `0xF103` UINT32 occupancy change reports requested
  - `0xF104` UINT32 Zigbee report commands successfully enqueued (NOT ACKed)
  - `0xF105` UINT32 failed enqueue attempts/retries
  - `0xF106` UINT32 event timer callbacks
  - `0xF107` UINT32 app-task calls sampling PD4 HIGH
  - `0xF108` UINT32 app-task calls sampling PD4 LOW
  - `0xF109` UINT32 sleep-return calls (all sources, not exclusively GPIO)
  - `0xF10A` UINT32 total app-task PD4 samples.
- Read 0xF100, 0xF101, 0xF107, 0xF109 before/after PIR movement. If no rise/high samples, investigate pin signal and sleep; if rises, investigate occupancy ZCL/reporting. If manual read of these attributes is rejected by converter/Dev console, fix the *diagnostic transport* before interpreting values.
- The test build is *diagnostic, not a confirmed fix*. Do not flash any MAIN device or alter the unified PCB profile.

### TEST Zigbee2MQTT custom attribute reader
The Z2M Windfront Dev console attribute dropdown only shows built-in attributes; custom 0xF100..0xF10A cannot be selected. This was missed during initial v0.1.42 planning. The external converter was extended with explicit `entity.read('msOccupancySensing', [numeric IDs])` requests and `fromZigbee` response decoding. It adds read-only diagnostic exposes (`pir_gpio`, `pir_rising_edges`, etc.) with refresh/get functionality. On HA TEST, replace the existing `external_converters/prucho2_pir_zg204zl.mjs` with this version and restart only Zigbee2MQTT Edge. This converter extension has been committed but has NOT yet been tested with Z2M and physical diagnostic firmware; any read error must be observed before relying on the values.

## Diagnostic read attempt on HA TEST — 2026-11-15 22:14 UTC

- Confirmed through Bridge v0.5 GitHub IPC and HA TEST Zigbee2MQTT frontend that `pir-prucho` is connected; MQTT state still `occupancy:false` while illuminance reports every ~10 seconds.
- Z2M Dev console limits attributes to named ZCL entries; custom 0xF100..0xF10A not selectable.
- TEST-only converter `prucho2_pir_zg204zl.mjs` was updated via MQTT converter/save (first attempt failed because concurrently added diagnostic declarations both named `pirDiagnostics`; duplicate removed, second attempt **status ok** and Z2M log confirms loaded).
- Converter now has `diagRead` with per-property GET e.g. `pir_gpio`, reading numeric IDs and `fromZigbee` translating numeric readResponses.
- First read sent `zigbee2mqtt/pir-prucho/get` with `{"pir_gpio":""}`. Z2M did invoke `msOccupancySensing.read([61696,...61706])` but **timed out after 10 s**. This is *not* a measured diagnostic value and does not prove GPIO=0. It suggests sleeping end device might not have received ZCL request. Need repeat while device awake (short press button invokes `app_setPollRate(TIMEOUT_20SEC)` in Slacky's `src/app_button.c`). Do not hold >=5 seconds, which factory-resets.
- Generic MQTT `{"read":{...}}` was rejected as `No converter available for 'get' 'read'`, so use supported per-key GET.
- No HA MAIN operations or restart.

## 2026-11-15 — v0.1.43: user-requested battery-saving illuminance changes

Observed live on HA TEST: Z2M logs showed `pir-prucho` repeatedly publishing illuminance at about 10-second intervals, sometimes twice with the same value. Full Zigbee interview completed successfully while diagnostics `msOccupancySensing.read([0xF100..0xF10A])` timed out. **Successful interview makes an incorrect or oversized diagnostic read plausible; don't claim that a lack of returned diagnostic values proves PIR input stayed low.**

V0.1.43 changes:
- ADC sampling once per 60 s. Sampling alone does not send Zigbee radio traffic.
- One initial illuminance report after joining; thereafter report **only when physically estimated illuminance changed by >=50 lx compared with the previous report**, in either direction. No hourly unconditional sync.
- Disable generic ZCL illuminance reporting registration in app_main.c and remove `REPORTABLE` attribute flag in app_endpoint_cfg.c; converter uses `illuminance({reporting:false})`. This avoids automatic duplicate packets on top of our manual reports. This only affects the illuminance cluster, not the standard occupancy/reporting behavior.
- Fix 32-bit arithmetic overflow in the approximate photodetector fourth-power conversion and the Zigbee logarithmic lux encoding. Telink TC32 did not link 64-bit helper calls (`__muldi3`, `__udivdi3`) in the first implementation, therefore final implementation uses **32-bit Q12/Q15 fixed-point** arithmetic.
- Diagnostic converter `diagRead.convertGet` requests **one selected diagnostic ZCL attribute** rather than all 11 simultaneously; responses will still require a reachable sleepy endpoint and an actual ZCL Read response. This is an improvement to transport, not proof that diagnostics or PIR detection now work.
- v0.1.43 firmware image metadata: `0x01433001`, manufacturer `0x6565`, image type `0x022d`.
- GPIO PD4 active high comes from PVVX `src/board_zg204zl.h`; PVVX's `task_pir` updates occupancy on input and explicitly configures opposite-level wake with `cpu_set_gpio_wakeup`. Our Slacky-based power handling remains a separately designed implementation, still requiring hardware validation. No claim of fixed occupancy.
- For OTA and test, use only the validated `last_version.bin` and `0x01433001` .zigbee file. Legacy Makefile may generate a stale name containing `V0.1.41` despite current metadata; workflow artifact list is now restricted to avoid that unrelated filename.

**Status:** GitHub Actions build for v0.1.43 validated and uploaded; pending TEST physical check for battery-friendly reporting and PIR. Do not flash MAIN PIR or change production HA.

## 2026-11-15 ~22:55 UTC — TEST v0.1.43 Z2M configuration failure

- User-provided Z2M log: interview succeeded, then `msIlluminanceMeasurement.configReport(measuredValue, min=10, max=3600, change=10)` failed with `UNSUPPORTED_ATTRIBUTE` on endpoint 1 (attempts 2 and 3). PIR and illuminance were shown as N/A in Z2M; these values alone do not establish GPIO inactivity or prove ADC failure.
- Root of configure failure: firmware v0.1.43 intentionally defines lux measuredValue as **READ only** (not REPORTABLE) and sends manual `zcl_sendReportCmd` only on initial/50 lx change. Z2M attempted standard configuration inconsistent with this contract. The previous repository converter contained `illuminance({reporting:false})`, so the actual loaded converter/runtime configuration must be inspected on TEST; it may be stale or a different external converter.
- TEST converter source fix [commit 08b8dca](https://github.com/prucho2/ts0201_tz3000_zed/commit/08b8dca8deaf780d76b3870113e2de58724ecbfe): remove illuminance modernExtend entirely to prevent its implicit reporting setup, add custom readResponse/attributeReport decoder for standard measuredValue to physical lx, preserve occupancy and battery extensions and all diagnostic getters. This is **converter only**; v0.1.43 firmware itself was not changed.
- **Not yet deployed or tested on HA TEST**. Deploy the exact converter source via supported TEST-only workflow, verify Z2M actually loads it (new description `v0.1.43+ manual illuminance reporting`), then restart Z2M Edge on TEST only and request configure. The configure log must no longer contain `msIlluminanceMeasurement.configReport`. If it still does, investigate active converter version/path and other configure steps rather than modifying firmware.
- The 60-second ADC cycle and 50 lx threshold imply that `illuminance=N/A` during the first minute may mean no sample/report yet, especially immediately following pairing; missing values over multiple samples require examining raw ZCL traffic and join/report events. This configuration failure alone does not explain persistent PIR occupancy false; continue separate PD4 diagnostic work after transport is fixed.
- Do not restart HA MAIN, edit MAIN devices, or claim motion fixed from this converter change.

## 2026-11-15 — v0.1.44 TEST, approved PIR direct GPIO / event diagnostics

User authorized a **TEST-only** diagnostic build and direct PD4 experiment. No changes to HA MAIN or its PIR devices. This release does **not** change the existing illuminance sampling/reporting algorithm (its 60 s ADC schedule remains pending separate agreement).

- GPIO: the PIR event path now reads `gpio_read(GPIO_PD4)` directly, like working PVVX `task_pir()` (instead of Slacky wrapper `drv_gpio_read()`; both should be equivalent on TLSR8253).
- Sleep: when entering low-power mode, keep Slacky's pairing-button wake configuration, but configure PIR PD4 explicitly using `cpu_set_gpio_wakeup(PRUCHO2_PIR_GPIO, !gpio_read(PRUCHO2_PIR_GPIO), 1)` as PVVX does.
- **Event-driven diagnostic snapshot:** all 11 existing attributes `0xF100..0xF10A` are packed into **one ZCL Report Attributes command** on endpoint 1, Occupancy Sensing cluster 0x0406. This uses `zcl_sendReportAttrsCmd`; no 11 independent ZCL reads, no periodic automatic diagnostic traffic in a static environment.
- A first snapshot is queued after join. Following snapshots are triggered by observed PD4 rises/falls and rate-limited to one successful enqueued command per **5 seconds**, with changes coalesced. Counters continue updating locally in RAM; last successful enqueue time controls the throttle. Failed sends remain pending for retry.
- Existing converter's `pirDiagnostics` fromZigbee handles numeric attributes on `attributeReport` as well as `readResponse`, so no converter update is required for this snapshot format. UI values should populate after successful snapshot delivery, without invoking manual GET buttons.
- **Important limitations:** an enqueued packet is not an APS acknowledgement. `N/A` after a test does not alone prove no GPIO movement; verify Z2M logs and any actual cluster 0x0406 report. Short pulses may still be lost if the device is not running when PD4 changes. The diagnostics help separate physical wakeup and ZCL reporting.
- Firmware file version `0x01443001`, manufacturer `0x6565`, image type `0x022D`. Build via test-only GitHub Action; version increment does not imply hardware success. Preserve full-flash backup and test on disassembled PIR attached to HA TEST only.
- CI run: https://github.com/prucho2/ts0201_tz3000_zed/actions/runs/43773593986 (check final status before recommending download or OTA).
- After successful build and manual OTA: verify join/configure, diagnostic fields initially populated, then exercise PD4 with motion (compare `pir_gpio`, `pir_rising_edges`, `pir_high_samples`, `pir_sleep_returns`, `pir_reports_requested`, `pir_reports_enqueued`); keep MAIN untouched.

## 2026-11-15 — Hardware TEST v0.1.44 counters, revert to Slacky path (v0.1.45 source)

User-provided Z2M values after v0.1.44: `occupancy=false`, `pir_gpio=0`, `pir_rising_edges=0`, `pir_falling_edges=0`, `pir_high_samples=0`, `pir_low_samples=2231`, `pir_poll_samples=2231`, `pir_reports_requested=0`, `pir_reports_enqueued=1`, `pir_reports_retries=0`, `pir_sleep_returns=0`, `pir_timer_checks=1`. Other communication is functional (`illuminance=866`, LQI 188, battery 100%). Thus **the application sampled PD4 as LOW in 2231 samples**; the observed failure is upstream of occupancy reporting. The returned counter `pir_sleep_returns=0` only counts function returns after `drv_pm_sleep` and **does not independently prove there were no sleep entries** (long/deep sleep can reboot and reset RAM; early returns do not increment).

Source comparison:
- Slacky's `drv_gpio_read()` on MCU_CORE_8258 is a direct wrapper around Telink `gpio_read()` (from `tl_zigbee_sdk/proj/drivers/drv_gpio.c`). Thus v0.1.44 read API swap cannot explain a different physical PD4 level.
- Both PVVX and our board definition specify PD4 as active HIGH with GPIO input enabled, output disabled; PVVX successfully detected motion earlier on this hardware. The current application does not explicitly re-apply `drv_gpio_func_set(PD4)` / `drv_gpio_input_en(PD4,true)` after board startup; it depends on GPIO configuration macros being correctly applied by platform initialization. **This is a verification target, not yet proven wrong.**
- No samples were HIGH; cannot distinguish a physical PIR output stuck LOW, different actual runtime GPIO configuration, missing event/wakeup, or overlooked transient without a runtime GPIO IE/function/OE register observation and controlled physical motion test. The output cannot be diagnosed from Z2M interiew success alone.
- User requested to restore Slacky API and wakeup setup. Reverted `read_pir()` to `drv_gpio_read()` and `prucho2_pir_prepare_sleep()` to `drv_pm_wakeupPinLevelChange(g_switchPmCfg,2)` in source commit `3f3365c`. Retained event diagnostic snapshots. Incremented TEST-only firmware to **v0.1.45** (`0x01453001`), CI config adjusted. This is a **revert/diagnostic baseline, not a verified fix**. Do not flash production PIRs.
- Next investigation should check the **actual runtime PD4 input enable, output enable, GPIO function**, and whether PD4 becomes HIGH on the disassembled test board during movement. If input is correctly enabled and PD4 physically remains LOW, check sensor power/output electrically; if HIGH physically but read LOW, inspect pin mux and SDK hardware configuration. Do not assume the cause and do not create further builds before agreeing on a specific test.
