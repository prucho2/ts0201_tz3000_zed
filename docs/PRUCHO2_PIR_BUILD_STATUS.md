# ZG-204ZL experimental firmware build status

Workflow result: **failure**

Source SHA: `d09335152b8ff17627c7fa5db0fa5fd3c88b324e`

Run: https://github.com/prucho2/ts0201_tz3000_zed/actions/runs/43779453295

This is development-only and not approved for flashing.

## Last compiler output

\`\`\`text
Building file: tl_zigbee_sdk/proj/common/list.c
Building file: tl_zigbee_sdk/proj/common/mempool.c
Building file: tl_zigbee_sdk/proj/common/string.c
Building file: tl_zigbee_sdk/proj/common/tlPrintf.c
Building file: tl_zigbee_sdk/proj/common/utility.c
Building file: tl_zigbee_sdk/proj/drivers/drv_adc.c
Building file: tl_zigbee_sdk/proj/drivers/drv_calibration.c
Building file: tl_zigbee_sdk/proj/drivers/drv_flash.c
Building file: tl_zigbee_sdk/proj/drivers/drv_gpio.c
Building file: tl_zigbee_sdk/proj/drivers/drv_hw.c
Building file: tl_zigbee_sdk/proj/drivers/drv_i2c.c
Building file: tl_zigbee_sdk/proj/drivers/drv_keyboard.c
Building file: tl_zigbee_sdk/proj/drivers/drv_nv.c
Building file: tl_zigbee_sdk/proj/drivers/drv_pm.c
Building file: tl_zigbee_sdk/proj/drivers/drv_putchar.c
Building file: tl_zigbee_sdk/proj/drivers/drv_pwm.c
Building file: tl_zigbee_sdk/proj/drivers/drv_security.c
Building file: tl_zigbee_sdk/proj/drivers/drv_spi.c
Building file: tl_zigbee_sdk/proj/drivers/drv_timer.c
Building file: tl_zigbee_sdk/proj/drivers/drv_uart.c
Building file: tl_zigbee_sdk/proj/drivers/usb/usb.c
Building file: tl_zigbee_sdk/proj/drivers/usb/usbdesc.c
Building file: tl_zigbee_sdk/proj/drivers/usb/app/usbcdc.c
Building file: tl_zigbee_sdk/proj/drivers/usb/app/usbkb.c
Building file: tl_zigbee_sdk/proj/drivers/usb/app/usbmouse.c
Building file: tl_zigbee_sdk/proj/drivers/usb/app/usbvendor.c
Building file: tl_zigbee_sdk/proj/os/ev.c
Building file: tl_zigbee_sdk/proj/os/ev_buffer.c
Building file: tl_zigbee_sdk/proj/os/ev_poll.c
Building file: tl_zigbee_sdk/proj/os/ev_queue.c
Building file: tl_zigbee_sdk/proj/os/ev_rtc.c
Building file: tl_zigbee_sdk/proj/os/ev_timer.c
Building file: tl_zigbee_sdk/platform/boot/link_cfg.S
Building file: tl_zigbee_sdk/platform/boot/8258/cstartup_8258.S
Building file: tl_zigbee_sdk/platform/boot/826x/cstartup_826x.S
Building file: tl_zigbee_sdk/platform/boot/8278/cstartup_8278.S
Building file: tl_zigbee_sdk/platform/tc32/div_mod.S
Building file: tl_zigbee_sdk/platform/chip_8258/flash.c
Building file: tl_zigbee_sdk/platform/chip_8258/adc.c
Building file: tl_zigbee_sdk/platform/chip_8258/flash/flash_common.c
Building file: tl_zigbee_sdk/platform/chip_8258/flash/flash_mid011460c8.c
Building file: tl_zigbee_sdk/platform/chip_8258/flash/flash_mid1060c8.c
Building file: tl_zigbee_sdk/platform/chip_8258/flash/flash_mid13325e.c
Building file: tl_zigbee_sdk/platform/chip_8258/flash/flash_mid134051.c
Building file: tl_zigbee_sdk/platform/chip_8258/flash/flash_mid136085.c
Building file: tl_zigbee_sdk/platform/chip_8258/flash/flash_mid1360c8.c
Building file: tl_zigbee_sdk/platform/chip_8258/flash/flash_mid1360eb.c
Building file: tl_zigbee_sdk/platform/chip_8258/flash/flash_mid14325e.c
Building file: tl_zigbee_sdk/platform/chip_8258/flash/flash_mid1460c8.c
Building file: tl_zigbee_sdk/platform/services/b85m/irq_handler.c
Building file: src/common/main.c
Building file: src/common/flash.c
Building file: src/zigbee/zcl/zcl_relative_humidity_measurement.c
Building file: src/zigbee/zcl/zcl_onoffSwitchCfg.c
Building file: src/zigbee/zcl/zcl_reporting.c
Building file: src/zigbee/zdo/zdp.c
Building file: src/zigbee/bdb/bdb.c
Building file: src/sensors/cht8305.c
Building file: src/sensors/sht30.c
Building file: src/sensors/sht40.c
Building file: src/sensors/aht20.c
Building file: src/app_timer.c
Building file: src/app_config.c
Building file: src/app_i2c.c
Building file: src/app_sensor.c
src/app_sensor.c: In function 'sht40_i2c_read':
src/app_sensor.c:319:1: warning: no return statement in function returning non-void
src/app_sensor.c: In function 'sht40_i2c_write':
src/app_sensor.c:331:1: warning: no return statement in function returning non-void
src/app_sensor.c: In function 'app_sht40_dev_set':
src/app_sensor.c:343:9: warning: implicit declaration of function 'scan_i2c_addr'
Building file: src/app_pir_prucho2.c
src/app_pir_prucho2.c:126:6: error: redefinition of 'prucho2_pir_note_wakeup'
src/app_pir_prucho2.c:34:6: note: previous definition of 'prucho2_pir_note_wakeup' was here
make: *** [project.mk:44: out/./src/app_pir_prucho2.o] Error 1
\`\`\`
