/*
 * Prucho2 ZG-204ZL. Single firmware for identical PCB with or without
 * populated illuminance components (missing components report 0 lux).
 *
 * GPIO mapping from PVVX board definition; application and ZCL from Slacky.
 * DEVELOPMENT ONLY. DO NOT FLASH until clean build and TEST hardware check.
 */
#ifndef PRUCHO2_BOARD_ZG204ZL_PIR_H_
#define PRUCHO2_BOARD_ZG204ZL_PIR_H_

#if (BOARD == BOARD_PRUCHO2_PIR_ZG204ZL)
#define PRUCHO2_PIR 1

#define ZCL_BASIC_MFG_NAME {10,'S','l','a','c','k','y','-','D','I','Y'}
#define ZCL_BASIC_MODEL_ID {13,'P','r','u','c','h','o','2','-','P','I','R','-','z'}

#define I2C_DRV_USED I2C_DRV_NONE
/* No T/H sensor is initialized for PIR. A compile-only dummy selection
 * keeps the existing sensor translation unit from hitting its #error. */
#define SENSOR_USED SENSOR_SHT40

#define VOLTAGE_DETECT_PIN GPIO_PC5
#define MAX_BUTTON_NUM 1
#define BUTTON1 GPIO_PA1
#define PA1_FUNC AS_GPIO
#define PA1_OUTPUT_ENABLE OFF
#define PA1_INPUT_ENABLE ON
#define PULL_WAKEUP_SRC_PA1 PM_PIN_PULLUP_10K

enum { VK_SW1 = 0x01 };
#define KB_MAP_NORMAL {{VK_SW1,}}
#define KB_MAP_NUM KB_MAP_NORMAL
#define KB_MAP_FN KB_MAP_NORMAL
#define KB_DRIVE_PINS {NULL}
#define KB_SCAN_PINS {BUTTON1}

#define LED_ON 0
#define LED_OFF 1
#define LED_STATUS GPIO_PD7
#define PD7_FUNC AS_GPIO
#define PD7_OUTPUT_ENABLE ON
#define PD7_INPUT_ENABLE ON
#define PD7_DATA_OUT LED_OFF
#define LED_PERMIT LED_STATUS
#define LED1 LED_STATUS

#define PRUCHO2_PIR_GPIO GPIO_PD4
#define PRUCHO2_PIR_ACTIVE_LEVEL 1
#define PD4_FUNC AS_GPIO
#define PD4_OUTPUT_ENABLE OFF
#define PD4_INPUT_ENABLE ON

#define PRUCHO2_PIR_LUX_ADC_GPIO GPIO_PC4
#define PRUCHO2_PIR_LUX_ADC_CHANNEL C4P
#define PRUCHO2_PIR_LUX_POWER_GPIO GPIO_PD3
#define PRUCHO2_PIR_LUX_POWER_ACTIVE 1

#define PRUCHO2_PIR_VBAT_ADC_GPIO GPIO_PC5
#define PRUCHO2_PIR_VBAT_ADC_CHANNEL C5P
#define PRUCHO2_PIR_DEFAULT_HOLD_SEC 3

#endif /* board */
#endif /* guard */
