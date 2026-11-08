#ifndef SRC_COMMON_BOARD_GXHTV4_PC2PC3_H_
#define SRC_COMMON_BOARD_GXHTV4_PC2PC3_H_

#if (BOARD == BOARD_GXHTV4_PC2PC3)
/* Measured TLSR8258 QFN32 board (third physical board variant):
 * GXHTV4 candidate SHT4x protocol, I2C SDA PC2 (pin 22),
 * SCL PC3 (pin 23). LED cathode switched via transistor driven by
 * PB4 (pin 14): GPIO HIGH is expected to turn the transistor on.
 * Pairing pushbutton grounds PC0 (pin 20).
 * LED polarity/button already verified in hardware; GXHTV4 stability testing continues.
 */
#define ZCL_BASIC_MFG_NAME  {10,'S','l','a','c','k','y','-','D','I','Y'}
#define ZCL_BASIC_MODEL_ID  {12,'T','S','0','2','0','1','-','z','-','S','l','D'}
#define I2C_DRV_USED        I2C_DRV_SOFT
#define SENSOR_USED         SENSOR_SHT40
/* Same bit-banged SHT40 bus method as upstream Slacky-DIY, with
 * our independently traced GPIOs (SDA PC2 and SCL PC3). */
#define I2C_SDA             GPIO_PC2
#define PC2_FUNC            AS_GPIO
#define PC2_INPUT_ENABLE    1
#define PC2_OUTPUT_ENABLE   0
#define PC2_DATA_OUT        0
#define PULL_WAKEUP_SRC_PC2 PM_PIN_PULLUP_10K

#define I2C_SCL             GPIO_PC3
#define PC3_FUNC            AS_GPIO
#define PC3_INPUT_ENABLE    1
#define PC3_OUTPUT_ENABLE   0
#define PC3_DATA_OUT        0
#define PULL_WAKEUP_SRC_PC3 PM_PIN_PULLUP_10K

#define I2C_CLOCK           200000

#if UART_PRINTF_MODE
#define DEBUG_INFO_TX_PIN   GPIO_PB1
#define PB1_DATA_OUT        ON
#define DEBUG_BAUDRATE      115200
#endif
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
#define BUTTON1             GPIO_PC0
#define PC0_FUNC            AS_GPIO
#define PC0_OUTPUT_ENABLE   OFF
#define PC0_INPUT_ENABLE    ON
#define PULL_WAKEUP_SRC_PC0 PM_PIN_PULLUP_1M
enum { VK_SW1 = 0x01, };
#define KB_MAP_NORMAL       {{VK_SW1,}}
#define KB_MAP_NUM          KB_MAP_NORMAL
#define KB_MAP_FN           KB_MAP_NORMAL
#define KB_DRIVE_PINS       {NULL}
#define KB_SCAN_PINS        {BUTTON1}

#endif
#endif
