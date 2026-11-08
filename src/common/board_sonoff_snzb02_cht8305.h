#ifndef SRC_COMMON_BOARD_SONOFF_SNZB02_CHT8305_H_
#define SRC_COMMON_BOARD_SONOFF_SNZB02_CHT8305_H_

#if (BOARD == BOARD_SONOFF_SNZB02_CHT8305)

/* Sonoff SNZB-02 TLSR8258 QFN32 - traced PCB revision.
 * U3 CHT8305: SDA PC2 (U2 pin 22), SCL PC3 (pin 23).
 * LED PB4 (pin 14); SW1 PD3 (pin 32), active-low through R2 1k.
 * Battery voltage uses the same floating-pin ADC method as upstream;
 * readings must be checked on hardware.
 */
#define ZCL_BASIC_MFG_NAME  {10,'S','l','a','c','k','y','-','D','I','Y'}
#define ZCL_BASIC_MODEL_ID  {12,'T','S','0','2','0','1','-','z','-','S','l','D'}

#define I2C_DRV_USED        I2C_DRV_HARD
#define SENSOR_USED         SENSOR_CHT8305
#define I2C_PIN_GROUP       I2C_GPIO_GROUP_C2C3
#define I2C_CLOCK           200000

#if UART_PRINTF_MODE
#define DEBUG_INFO_TX_PIN   GPIO_PB1
#define PB1_DATA_OUT        ON
#define DEBUG_BAUDRATE      115200
#endif

/* Upstream TLSR8258 ADC VBAT mode expects a floating GPIO.
 * PC5 is not bonded on QFN32; validate battery readings before release.
 */
#define VOLTAGE_DETECT_PIN  GPIO_PC5

#define LED_ON              1
#define LED_OFF             0
#define LED_STATUS          GPIO_PB4
#define PB4_FUNC            AS_GPIO
#define PB4_OUTPUT_ENABLE   ON
#define PB4_INPUT_ENABLE    OFF
#define PB4_DATA_OUT        LED_OFF
#define LED_PERMIT          LED_STATUS
#define LED1                LED_STATUS

#define MAX_BUTTON_NUM      1
#define BUTTON1             GPIO_PD3
#define PD3_FUNC            AS_GPIO
#define PD3_OUTPUT_ENABLE   OFF
#define PD3_INPUT_ENABLE    ON
#define PULL_WAKEUP_SRC_PD3 PM_PIN_PULLUP_1M

enum { VK_SW1 = 0x01, };
#define KB_MAP_NORMAL       {{VK_SW1,}}
#define KB_MAP_NUM          KB_MAP_NORMAL
#define KB_MAP_FN           KB_MAP_NORMAL
#define KB_DRIVE_PINS       {NULL}
#define KB_SCAN_PINS        {BUTTON1}

#endif
#endif
