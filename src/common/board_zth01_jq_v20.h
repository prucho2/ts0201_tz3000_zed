#ifndef SRC_COMMON_BOARD_ZTH01_JQ_V20_H_
#define SRC_COMMON_BOARD_ZTH01_JQ_V20_H_

#if (BOARD == BOARD_ZTH01_JQ_V20)

/* ZTH01 casing, PCB JQ2010025-V2.0, TLSR8258.
 * GPIO validated with the working pvvx ZigbeeTLc BOARD_ZYZTH02 profile.
 * SHT30/CHT832x-compatible sensor; first hardware test pending.
 * Physical PCB labels SDA/SCL can be swapped, follow verified pvvx GPIO.
 */
#define ZCL_BASIC_MFG_NAME  {10,'S','l','a','c','k','y','-','D','I','Y'}
#define ZCL_BASIC_MODEL_ID  {12,'T','S','0','2','0','1','-','z','-','S','l','D'}

#define I2C_DRV_USED        I2C_DRV_SOFT
#define SENSOR_USED         SENSOR_SHT30
#define I2C_CLOCK           200000

#define I2C_SCL             GPIO_PC3
#define PC3_FUNC            AS_GPIO
#define PC3_INPUT_ENABLE    1
#define PC3_OUTPUT_ENABLE   0
#define PC3_DATA_OUT        0
#define PULL_WAKEUP_SRC_PC3 PM_PIN_PULLUP_10K

#define I2C_SDA             GPIO_PD2
#define PD2_FUNC            AS_GPIO
#define PD2_INPUT_ENABLE    1
#define PD2_OUTPUT_ENABLE   0
#define PD2_DATA_OUT        0
#define PULL_WAKEUP_SRC_PD2 PM_PIN_PULLUP_10K

#if UART_PRINTF_MODE
#define DEBUG_INFO_TX_PIN   GPIO_PB1
#define DEBUG_BAUDRATE      115200
#endif

/* TLSR8258 internal VBAT ADC floating pad, inherited from pvvx. */
#define VOLTAGE_DETECT_PIN  GPIO_PC5

#define LED_ON              1
#define LED_OFF             0
#define LED_STATUS          GPIO_PC2
#define PC2_FUNC            AS_GPIO
#define PC2_OUTPUT_ENABLE   ON
#define PC2_INPUT_ENABLE    OFF
#define PC2_DATA_OUT        LED_OFF
#define LED_PERMIT          LED_STATUS
#define LED1                LED_STATUS

#define MAX_BUTTON_NUM      1
#define BUTTON1             GPIO_PB4
#define PB4_FUNC            AS_GPIO
#define PB4_OUTPUT_ENABLE   OFF
#define PB4_INPUT_ENABLE    ON
#define PULL_WAKEUP_SRC_PB4 PM_PIN_PULLUP_1M

enum { VK_SW1 = 0x01, };
#define KB_MAP_NORMAL       {{VK_SW1,}}
#define KB_MAP_NUM          KB_MAP_NORMAL
#define KB_MAP_FN           KB_MAP_NORMAL
#define KB_DRIVE_PINS       {NULL}
#define KB_SCAN_PINS        {BUTTON1}

#endif /* BOARD_ZTH01_JQ_V20 */
#endif
