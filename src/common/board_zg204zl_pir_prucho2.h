/*
 * Board reference: ZG-204ZL / TLSR8253 PIR (Prucho2).
 *
 * Hardware mapping traced by PVVX and recorded in
 * prucho2/ZigbeeTLc-PIR/src/board_zg204zl.h.
 *
 * THIS HEADER IS AN UNINTEGRATED HARDWARE PROFILE, NOT A WORKING
 * PIR IMAGE. The Slacky-DIY ZCL Occupancy + Illuminance integration,
 * GPIO wake source and independent light ADC driver must be added
 * and compiled before it is safe to flash.
 *
 * SAME PCB, SAME firmware and GPIOs for all devices. Some production units
 * simply have no light-sensor components soldered. The absent sensor must
 * report 0 lux, as already established by the identical factory/PVVX firmware.
 * No special variant handling or extra sensor-population tests are required.
 */
#ifndef PRUCHO2_BOARD_ZG204ZL_PIR_H_
#define PRUCHO2_BOARD_ZG204ZL_PIR_H_

/* Do not mix this with temperature/humidity board pin definitions. */
#define PRUCHO2_PIR_GPIO             GPIO_PD4
#define PRUCHO2_PIR_ACTIVE_LEVEL     1

#define PRUCHO2_PIR_BUTTON_GPIO      GPIO_PA1
#define PRUCHO2_PIR_BUTTON_ACTIVE    0

#define PRUCHO2_PIR_LED_GPIO         GPIO_PD7
#define PRUCHO2_PIR_LED_ACTIVE       0

#define PRUCHO2_PIR_LUX_ADC_GPIO     GPIO_PC4
#define PRUCHO2_PIR_LUX_ADC_CHANNEL  C4P
#define PRUCHO2_PIR_LUX_POWER_GPIO   GPIO_PD3
#define PRUCHO2_PIR_LUX_POWER_ACTIVE 1

#define PRUCHO2_PIR_VBAT_ADC_GPIO    GPIO_PC5
#define PRUCHO2_PIR_VBAT_ADC_CHANNEL C5P

#define PRUCHO2_PIR_DEFAULT_HOLD_SEC 3

/* No separate light-sensor board variant, build flag or firmware image. */

#endif /* PRUCHO2_BOARD_ZG204ZL_PIR_H_ */
