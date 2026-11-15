# ZG-204ZL experimental firmware build status

Workflow result: **success**

Source SHA: `711bbbd8639c1ef181bc3746cfb8cb476a7828b5`

Run: https://github.com/prucho2/ts0201_tz3000_zed/actions/runs/43763132746

This is development-only and not approved for flashing.

## Last compiler output

\`\`\`text
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
Building file: src/app_pir_lux_prucho2.c
Building file: src/app_binding.c
Building file: src/app_button.c
Building file: src/app_led.c
Building file: src/app_pm.c
Building file: src/app_on_off.c
Building file: src/app_battery.c
Building file: src/app_reporting.c
src/app_reporting.c:46:13: warning: 'forceReportTemperature' defined but not used
src/app_reporting.c:137:13: warning: 'forceReportHumidity' defined but not used
src/app_reporting.c:210:13: warning: 'forceReportSwitchAction' defined but not used
Building file: src/app_endpoint_cfg.c
Building file: src/app_utility.c
Building file: src/app_main.c
Building file: src/zb_appCb.c
Building file: src/zcl_appCb.c
Building target: out/prucho2_zg204zl_pir.elf
Invoking: TC32 C Linker
/opt/tc32/bin/tc32-elf-ld --gc-sections -L ./tl_zigbee_sdk/zigbee/lib/tc32 -L ./tl_zigbee_sdk/platform/lib -L ./tl_zigbee_sdk/platform/tc32 -T ./tl_zigbee_sdk/platform/boot/8258/boot_8258.link -o "./out/prucho2_zg204zl_pir.elf" ./out/zigbee/zcl/zcl.o ./out/zigbee/zcl/zcl_nv.o ./out/zigbee/zcl/zll_commissioning/zcl_toucklink_security.o ./out/zigbee/zcl/zll_commissioning/zcl_zllTouchLinkDiscovery.o ./out/zigbee/zcl/zll_commissioning/zcl_zllTouchLinkJoinOrStart.o ./out/zigbee/zcl/zll_commissioning/zcl_zll_commissioning.o ./out/zigbee/zcl/zcl_wwah/zcl_wwah.o ./out/zigbee/zcl/zcl_wwah/zcl_wwah_attr.o ./out/zigbee/zcl/smart_energy/zcl_metering.o ./out/zigbee/zcl/smart_energy/zcl_metering_attr.o ./out/zigbee/zcl/security_safety/zcl_ias_ace.o ./out/zigbee/zcl/security_safety/zcl_ias_wd.o ./out/zigbee/zcl/security_safety/zcl_ias_wd_attr.o ./out/zigbee/zcl/security_safety/zcl_ias_zone.o ./out/zigbee/zcl/security_safety/zcl_ias_zone_attr.o ./out/zigbee/zcl/ota_upgrading/zcl_ota.o ./out/zigbee/zcl/ota_upgrading/zcl_ota_attr.o ./out/zigbee/zcl/measument_sensing/zcl_electrical_measurement.o ./out/zigbee/zcl/measument_sensing/zcl_electrical_measurement_attr.o ./out/zigbee/zcl/measument_sensing/zcl_illuminance_measurement.o ./out/zigbee/zcl/measument_sensing/zcl_illuminance_measurement_attr.o ./out/zigbee/zcl/measument_sensing/zcl_occupancy_sensing.o ./out/zigbee/zcl/measument_sensing/zcl_occupancy_sensing_attr.o ./out/zigbee/zcl/measument_sensing/zcl_temperature_measurement.o ./out/zigbee/zcl/measument_sensing/zcl_temperature_measurement_attr.o ./out/zigbee/zcl/light_color_control/zcl_light_colorCtrl.o ./out/zigbee/zcl/light_color_control/zcl_light_colorCtrl_attr.o ./out/zigbee/zcl/hvac/zcl_thermostat.o ./out/zigbee/zcl/general/zcl_alarm.o ./out/zigbee/zcl/general/zcl_alarm_attr.o ./out/zigbee/zcl/general/zcl_basic.o ./out/zigbee/zcl/general/zcl_basic_attr.o ./out/zigbee/zcl/general/zcl_binary_input.o ./out/zigbee/zcl/general/zcl_binary_input_attr.o ./out/zigbee/zcl/general/zcl_binary_output.o ./out/zigbee/zcl/general/zcl_binary_output_attr.o ./out/zigbee/zcl/general/zcl_devTemperatureCfg.o ./out/zigbee/zcl/general/zcl_devTemperatureCfg_attr.o ./out/zigbee/zcl/general/zcl_diagnostics.o ./out/zigbee/zcl/general/zcl_diagnostics_attr.o ./out/zigbee/zcl/general/zcl_greenPower.o ./out/zigbee/zcl/general/zcl_greenPower_attr.o ./out/zigbee/zcl/general/zcl_group.o ./out/zigbee/zcl/general/zcl_group_attr.o ./out/zigbee/zcl/general/zcl_identify.o ./out/zigbee/zcl/general/zcl_identify_attr.o ./out/zigbee/zcl/general/zcl_level.o ./out/zigbee/zcl/general/zcl_level_attr.o ./out/zigbee/zcl/general/zcl_multistate_input.o ./out/zigbee/zcl/general/zcl_multistate_input_attr.o ./out/zigbee/zcl/general/zcl_multistate_output.o ./out/zigbee/zcl/general/zcl_multistate_output_attr.o ./out/zigbee/zcl/general/zcl_onoff.o ./out/zigbee/zcl/general/zcl_onoff_attr.o ./out/zigbee/zcl/general/zcl_pollCtrl.o ./out/zigbee/zcl/general/zcl_pollCtrl_attr.o ./out/zigbee/zcl/general/zcl_powerCfg.o ./out/zigbee/zcl/general/zcl_powerCfg_attr.o ./out/zigbee/zcl/general/zcl_scene.o ./out/zigbee/zcl/general/zcl_scene_attr.o ./out/zigbee/zcl/general/zcl_time.o ./out/zigbee/zcl/general/zcl_time_attr.o ./out/zigbee/zcl/commissioning/zcl_commissioning.o ./out/zigbee/zcl/commissioning/zcl_commissioning_attr.o ./out/zigbee/zcl/closures/zcl_door_lock.o ./out/zigbee/zcl/closures/zcl_door_lock_attr.o ./out/zigbee/zcl/closures/zcl_window_covering.o ./out/zigbee/zcl/closures/zcl_window_covering_attr.o  ./out/zigbee/wwah/wwah.o ./out/zigbee/wwah/wwahEpCfg.o  ./out/zigbee/ss/ss_nv.o  ./out/zigbee/ota/ota.o ./out/zigbee/ota/otaEpCfg.o  ./out/zigbee/mac/mac_phy.o ./out/zigbee/mac/mac_pib.o  ./out/zigbee/gp/gp.o ./out/zigbee/gp/gpEpCfg.o ./out/zigbee/gp/gp_proxy.o ./out/zigbee/gp/gp_proxyTab.o ./out/zigbee/gp/gp_sink.o ./out/zigbee/gp/gp_sinkTab.o ./out/zigbee/gp/gp_trans.o ./out/zigbee/gp/gp_transCfg.o  ./out/zigbee/common/zb_config.o ./out/zigbee/common/zb_version.o ./out/zigbee/aps/aps_group.o ./out/zigbee/af/zb_af.o  ./out/zbhci/zbhci.o ./out/zbhci/zbhciCmdProcess.o ./out/zbhci/zbhci_zclHandler.o ./out/zbhci/usbPrint/hci_usbPrint.o ./out/zbhci/usbHID/zbhci_usb_hid.o ./out/zbhci/usbCDC/zbhci_usb_cdc.o ./out/zbhci/uart/hci_uart.o  ./out/proj/common/list.o ./out/proj/common/mempool.o ./out/proj/common/string.o ./out/proj/common/tlPrintf.o ./out/proj/common/utility.o ./out/proj/drivers/drv_adc.o ./out/proj/drivers/drv_calibration.o ./out/proj/drivers/drv_flash.o ./out/proj/drivers/drv_gpio.o ./out/proj/drivers/drv_hw.o ./out/proj/drivers/drv_i2c.o ./out/proj/drivers/drv_keyboard.o ./out/proj/drivers/drv_nv.o ./out/proj/drivers/drv_pm.o ./out/proj/drivers/drv_putchar.o ./out/proj/drivers/drv_pwm.o ./out/proj/drivers/drv_security.o ./out/proj/drivers/drv_spi.o ./out/proj/drivers/drv_timer.o ./out/proj/drivers/drv_uart.o ./out/proj/drivers/usb/usb.o ./out/proj/drivers/usb/usbdesc.o ./out/proj/drivers/usb/app/usbcdc.o ./out/proj/drivers/usb/app/usbkb.o ./out/proj/drivers/usb/app/usbmouse.o ./out/proj/drivers/usb/app/usbvendor.o ./out/proj/os/ev.o ./out/proj/os/ev_buffer.o ./out/proj/os/ev_poll.o ./out/proj/os/ev_queue.o ./out/proj/os/ev_rtc.o ./out/proj/os/ev_timer.o  ./out/platform/boot/link_cfg.o ./out/platform/boot/8258/cstartup_8258.o ./out/platform/boot/826x/cstartup_826x.o ./out/platform/boot/8278/cstartup_8278.o  ./out/platform/tc32/div_mod.o ./out/platform/chip_8258/flash.o ./out/platform/chip_8258/adc.o ./out/platform/chip_8258/flash/flash_common.o ./out/platform/chip_8258/flash/flash_mid011460c8.o ./out/platform/chip_8258/flash/flash_mid1060c8.o ./out/platform/chip_8258/flash/flash_mid13325e.o ./out/platform/chip_8258/flash/flash_mid134051.o ./out/platform/chip_8258/flash/flash_mid136085.o ./out/platform/chip_8258/flash/flash_mid1360c8.o ./out/platform/chip_8258/flash/flash_mid1360eb.o ./out/platform/chip_8258/flash/flash_mid14325e.o ./out/platform/chip_8258/flash/flash_mid1460c8.o ./out/platform/services/b85m/irq_handler.o  ./out/./src/common/main.o ./out/./src/common/flash.o ./out/./src/zigbee/zcl/zcl_relative_humidity_measurement.o ./out/./src/zigbee/zcl/zcl_onoffSwitchCfg.o ./out/./src/zigbee/zcl/zcl_reporting.o ./out/./src/zigbee/zdo/zdp.o ./out/./src/zigbee/bdb/bdb.o ./out/./src/sensors/cht8305.o ./out/./src/sensors/sht30.o ./out/./src/sensors/sht40.o ./out/./src/sensors/aht20.o ./out/./src/app_timer.o ./out/./src/app_config.o ./out/./src/app_i2c.o ./out/./src/app_sensor.o ./out/./src/app_pir_prucho2.o ./out/./src/app_pir_lux_prucho2.o ./out/./src/app_binding.o ./out/./src/app_button.o ./out/./src/app_led.o ./out/./src/app_pm.o ./out/./src/app_on_off.o ./out/./src/app_battery.o ./out/./src/app_reporting.o ./out/./src/app_endpoint_cfg.o ./out/./src/app_utility.o ./out/./src/app_main.o ./out/./src/zb_appCb.o ./out/./src/zcl_appCb.o  -lzb_ed -ldrivers_8258 
Finished building target: out/prucho2_zg204zl_pir.elf
 
Create Flash image (binary format)
copy from `./out/prucho2_zg204zl_pir.elf' [elf32-littletc32] to `./out/prucho2_zg204zl_pir.bin' [binary]
Firmware patched!
Create zigbee OTA file
./bin/6565-022d-01423001-prucho2_zg204zl_pir.zigbee was created with ZCL OTA Header.
Create zigbee Tuya OTA file
Custom board: preserve firmware file version and image type in OTA header
rm -rf ./bin/1141-d3a3-1111114b-ts0201_tz3000_0x23_zed.zigbee   
 
Finished building: out/prucho2_zg204zl_pir.bin
 
Invoking: TC32 Create Extended Listing
/opt/tc32/bin/tc32-elf-objdump -x -D -l -S ./out/prucho2_zg204zl_pir.elf  > ./out/prucho2_zg204zl_pir.lst
Finished building: out/prucho2_zg204zl_pir.lst
 
Invoking: Print Size
   text	   data	    bss	    dec	    hex	filename
 151644	    816	  13288	 165748	  28774	./out/prucho2_zg204zl_pir.elf
 151644	    816	  13288	 165748	  28774	(TOTALS)
Finished building: sizedummy
 
\`\`\`
