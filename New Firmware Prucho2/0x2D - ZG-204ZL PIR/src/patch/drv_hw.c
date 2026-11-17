/********************************************************************************************************
 * @file    drv_hw.c
 *
 * @brief   This is the source file for drv_hw
 *
 * @author  Zigbee Group
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
#include "../tl_common.h"

/*
 * system clock configuration
 */
#if defined(MCU_CORE_826x)
    #if (CLOCK_SYS_CLOCK_HZ == 32000000)
        #define SYS_CLOCK_VALUE    SYS_CLK_32M_PLL
    #elif (CLOCK_SYS_CLOCK_HZ == 16000000)
        #define SYS_CLOCK_VALUE    SYS_CLK_16M_PLL
    #else
        #error please config system clock
    #endif
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
    #if (CLOCK_SYS_CLOCK_HZ == 24000000)
        #define SYS_CLOCK_VALUE    SYS_CLK_24M_Crystal
    #elif (CLOCK_SYS_CLOCK_HZ == 16000000)
        #define SYS_CLOCK_VALUE    SYS_CLK_16M_Crystal
    #elif (CLOCK_SYS_CLOCK_HZ == 48000000)
        #define SYS_CLOCK_VALUE    SYS_CLK_48M_Crystal
    #else
        #error please config system clock
    #endif
#elif defined(MCU_CORE_B91) || defined(MCU_CORE_B92)
    #if (CLOCK_SYS_CLOCK_HZ == 48000000)
        #define CLOCK_INIT         CCLK_48M_HCLK_48M_PCLK_24M
    #else
        #error please config system clock
    #endif
#elif defined(MCU_CORE_TL721X)
    #if (CLOCK_SYS_CLOCK_HZ == 120000000)
        #define CLOCK_INIT         PLL_240M_CCLK_120M_HCLK_60M_PCLK_60M_MSPI_48M
    #else
        #error please config system clock
    #endif
#elif defined(MCU_CORE_TL321X)
    #if (CLOCK_SYS_CLOCK_HZ == 48000000)
        #define CLOCK_INIT         PLL_192M_CCLK_48M_HCLK_48M_PCLK_24M_MSPI_48M
    #elif (CLOCK_SYS_CLOCK_HZ == 96000000)
        #define CLOCK_INIT         PLL_192M_CCLK_96M_HCLK_48M_PCLK_24M_MSPI_48M
    #else
        #error please config system clock
    #endif
#endif

//system ticks per US
u32 sysTimerPerUs;

static void randInit(void)
{
#if defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
    random_generator_init();
#elif defined(MCU_CORE_B91) || defined(MCU_CORE_B92) || defined(MCU_CORE_TL721X) || defined(MCU_CORE_TL321X)
    trng_init();
#endif
}

/*********************************************************************
 * @fn      internalFlashSizeCheck
 *
 * @brief   This function is provided to get and update to the correct flash address
 * 			where are stored the right MAC address and pre-configured parameters.
 * 			NOTE: It should be called before ZB_RADIO_INIT().
 *
 * @param   None
 *
 * @return  None
 */
static void internalFlashSizeCheck(void)
{
#if defined(MCU_CORE_8258) || defined(MCU_CORE_8278) || defined(MCU_CORE_B91) || defined(MCU_CORE_B92) || defined(MCU_CORE_TL721X) || defined(MCU_CORE_TL321X)
#if defined(MCU_CORE_TL721X)
    u32 mid = flash_read_mid_with_device_num(SLAVE0);
#else
    u32 mid = flash_read_mid();
#endif

    u8 *pMid = (u8 *)&mid;

    if ((pMid[2] < FLASH_SIZE_512K) || \
        ((g_u32MacFlashAddr == FLASH_ADDR_OF_MAC_ADDR_1M) && (pMid[2] < FLASH_SIZE_1M)) || \
        ((g_u32MacFlashAddr == FLASH_ADDR_OF_MAC_ADDR_2M) && (pMid[2] < FLASH_SIZE_2M)) || \
        ((g_u32MacFlashAddr == FLASH_ADDR_OF_MAC_ADDR_4M) && (pMid[2] < FLASH_SIZE_4M))) {
        /* Flash space not matched. */
        while(1);
    }

    switch (pMid[2]) {
    case FLASH_SIZE_1M:
        g_u32MacFlashAddr = FLASH_ADDR_OF_MAC_ADDR_1M;
        g_u32CfgFlashAddr = FLASH_ADDR_OF_F_CFG_INFO_1M;
        break;
    case FLASH_SIZE_2M:
        g_u32MacFlashAddr = FLASH_ADDR_OF_MAC_ADDR_2M;
        g_u32CfgFlashAddr = FLASH_ADDR_OF_F_CFG_INFO_2M;
        break;
    case FLASH_SIZE_4M:
        g_u32MacFlashAddr = FLASH_ADDR_OF_MAC_ADDR_4M;
        g_u32CfgFlashAddr = FLASH_ADDR_OF_F_CFG_INFO_4M;
        break;
    default:
        break;
    }
#endif
}

#if VOLTAGE_DETECT_ENABLE || defined(MCU_CORE_8258)
static void voltage_detect_init(u32 detectPin)
{
    drv_adc_init();

#if defined(MCU_CORE_826x)
    (void)detectPin;
    drv_adc_mode_pin_set(DRV_ADC_VBAT_MODE, NOINPUT);
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
    drv_adc_mode_pin_set(DRV_ADC_VBAT_MODE, (GPIO_PinTypeDef)detectPin);
#elif defined(MCU_CORE_B91) || defined(MCU_CORE_B92)
    drv_adc_mode_pin_set(DRV_ADC_BASE_MODE, (adc_input_pin_def_e)detectPin);
#elif defined(MCU_CORE_TL721X) || defined(MCU_CORE_TL321X)
    drv_adc_mode_pin_set(DRV_ADC_BASE_MODE, (adc_input_pin_def_e)detectPin);
#endif

    drv_adc_enable(1);
}
#endif

#if VOLTAGE_DETECT_ENABLE
#define VOLTAGE_DEBOUNCE_NUM 	5
void voltage_detect(bool powerOn)
{
    u16 voltage = drv_get_adc_data();
    u32 curTick = clock_time();
    s32 debounceNum = VOLTAGE_DEBOUNCE_NUM;

    //printf("VDD: %d\n", voltage);
    if (powerOn || voltage < BATTERY_SAFETY_THRESHOLD) {
        while (debounceNum > 0) {
            if (voltage > BATTERY_SAFETY_THRESHOLD) {
                debounceNum--;
            } else {
                debounceNum = VOLTAGE_DEBOUNCE_NUM;
            }

            if (clock_time_exceed(curTick, 1000 * 1000)) {
                //customers can adjust according to actual applications
#if PM_ENABLE
                //low battery power, entering deep sleep mode forcefully.
                drv_pm_sleep(PM_SLEEP_MODE_DEEPSLEEP, PM_WAKEUP_SRC_PAD, 0);
#else
                SYSTEM_RESET();
#endif
            }

            voltage = drv_get_adc_data();
        }
    }
}
#endif

#if (BOARD == BOARD_PRUCHO2_PIR_ZG204ZL)
/* Imported from PVVX ZigbeeTLc-PIR src/patch_z_sdk/hw_drv.c:
 * same port-wide register writes and analog pull initialization as 0.1.40.
 * Compile only for our PIR board, not other Slacky devices. */
static void prucho2_pvvx_gpio_init(int anaRes_init_en) {
	//PA group
	reg_gpio_pa_setting1 =
		(PA0_INPUT_ENABLE<<8) 	| (PA1_INPUT_ENABLE<<9)	| (PA2_INPUT_ENABLE<<10)	| (PA3_INPUT_ENABLE<<11) |
		(PA4_INPUT_ENABLE<<12)	| (PA5_INPUT_ENABLE<<13)	| (PA6_INPUT_ENABLE<<14)	| (PA7_INPUT_ENABLE<<15) |
		((PA0_OUTPUT_ENABLE?0:1)<<16)	| ((PA1_OUTPUT_ENABLE?0:1)<<17) | ((PA2_OUTPUT_ENABLE?0:1)<<18)	| ((PA3_OUTPUT_ENABLE?0:1)<<19) |
		((PA4_OUTPUT_ENABLE?0:1)<<20)	| ((PA5_OUTPUT_ENABLE?0:1)<<21) | ((PA6_OUTPUT_ENABLE?0:1)<<22)	| ((PA7_OUTPUT_ENABLE?0:1)<<23) |
		(PA0_DATA_OUT<<24)	| (PA1_DATA_OUT<<25)	| (PA2_DATA_OUT<<26)	| (PA3_DATA_OUT<<27) |
		(PA4_DATA_OUT<<28)	| (PA5_DATA_OUT<<29)	| (PA6_DATA_OUT<<30)	| (PA7_DATA_OUT<<31) ;
	reg_gpio_pa_setting2 =
		(PA0_DATA_STRENGTH<<8)		| (PA1_DATA_STRENGTH<<9)| (PA2_DATA_STRENGTH<<10)	| (PA3_DATA_STRENGTH<<11) |
		(PA4_DATA_STRENGTH<<12)	| (PA5_DATA_STRENGTH<<13)	| (PA6_DATA_STRENGTH<<14)	| (PA7_DATA_STRENGTH<<15) |
		(PA0_FUNC==AS_GPIO ? BIT(16):0)	| (PA1_FUNC==AS_GPIO ? BIT(17):0)| (PA2_FUNC==AS_GPIO ? BIT(18):0)| (PA3_FUNC==AS_GPIO ? BIT(19):0) |
		(PA4_FUNC==AS_GPIO ? BIT(20):0)	| (PA5_FUNC==AS_GPIO ? BIT(21):0)| (PA6_FUNC==AS_GPIO ? BIT(22):0)| (PA7_FUNC==AS_GPIO ? BIT(23):0);

	//PB group
	//ie
	analog_write(areg_gpio_pb_ie, 	(PB0_INPUT_ENABLE<<0) 	| (PB1_INPUT_ENABLE<<1)	| (PB2_INPUT_ENABLE<<2)	| (PB3_INPUT_ENABLE<<3) |
									(PB4_INPUT_ENABLE<<4)	| (PB5_INPUT_ENABLE<<5) | (PB6_INPUT_ENABLE<<6)	| (PB7_INPUT_ENABLE<<7) );
	//dataO
#if USE_SENSOR_RC2417
	u8 pb5_out = rc2417_is_powered() ? 0 : PB5_DATA_OUT;
#else
	u8 pb5_out = PB5_DATA_OUT;
#endif
	reg_gpio_pb_out =
		(PB0_DATA_OUT<<0)	| (PB1_DATA_OUT<<1)	| (PB2_DATA_OUT<<2)	| (PB3_DATA_OUT<<3) |
		(PB4_DATA_OUT<<4)	| (pb5_out<<5)	| (PB6_DATA_OUT<<6)	| (PB7_DATA_OUT<<7) ;
	//oen
	reg_gpio_pb_oen =
		((PB0_OUTPUT_ENABLE?0:1)<<0)	| ((PB1_OUTPUT_ENABLE?0:1)<<1) | ((PB2_OUTPUT_ENABLE?0:1)<<2)	| ((PB3_OUTPUT_ENABLE?0:1)<<3) |
		((PB4_OUTPUT_ENABLE?0:1)<<4)	| ((PB5_OUTPUT_ENABLE?0:1)<<5) | ((PB6_OUTPUT_ENABLE?0:1)<<6)	| ((PB7_OUTPUT_ENABLE?0:1)<<7);
	//ds
	analog_write(areg_gpio_pb_ds, 	(PB0_DATA_STRENGTH<<0) 	| (PB1_DATA_STRENGTH<<1)  | (PB2_DATA_STRENGTH<<2)	| (PB3_DATA_STRENGTH<<3) |
									(PB4_DATA_STRENGTH<<4)	| (PB5_DATA_STRENGTH<<5)  | (PB6_DATA_STRENGTH<<6)	| (PB7_DATA_STRENGTH<<7) );
	//func
	reg_gpio_pb_gpio =
		(PB0_FUNC==AS_GPIO ? BIT(0):0)	| (PB1_FUNC==AS_GPIO ? BIT(1):0)| (PB2_FUNC==AS_GPIO ? BIT(2):0)| (PB3_FUNC==AS_GPIO ? BIT(3):0) |
		(PB4_FUNC==AS_GPIO ? BIT(4):0)	| (PB5_FUNC==AS_GPIO ? BIT(5):0)| (PB6_FUNC==AS_GPIO ? BIT(6):0)| (PB7_FUNC==AS_GPIO ? BIT(7):0);

	//PC group
	//ie
	analog_write(areg_gpio_pc_ie, 	(PC0_INPUT_ENABLE<<0) 	| (PC1_INPUT_ENABLE<<1)	| (PC2_INPUT_ENABLE<<2)	| (PC3_INPUT_ENABLE<<3) |
									(PC4_INPUT_ENABLE<<4)	| (PC5_INPUT_ENABLE<<5) | (PC6_INPUT_ENABLE<<6)	| (PC7_INPUT_ENABLE<<7) );
	//dataO
	reg_gpio_pc_out =
		(PC0_DATA_OUT<<0)	| (PC1_DATA_OUT<<1)	| (PC2_DATA_OUT<<2)	| (PC3_DATA_OUT<<3) |
		(PC4_DATA_OUT<<4)	| (PC5_DATA_OUT<<5)	| (PC6_DATA_OUT<<6)	| (PC7_DATA_OUT<<7) ;
	//oen
	reg_gpio_pc_oen =
		((PC0_OUTPUT_ENABLE?0:1)<<0)	| ((PC1_OUTPUT_ENABLE?0:1)<<1) | ((PC2_OUTPUT_ENABLE?0:1)<<2)	| ((PC3_OUTPUT_ENABLE?0:1)<<3) |
		((PC4_OUTPUT_ENABLE?0:1)<<4)	| ((PC5_OUTPUT_ENABLE?0:1)<<5) | ((PC6_OUTPUT_ENABLE?0:1)<<6)	| ((PC7_OUTPUT_ENABLE?0:1)<<7);
	//ds
	analog_write(areg_gpio_pc_ds, 	(PC0_DATA_STRENGTH<<0) 	| (PC1_DATA_STRENGTH<<1)  | (PC2_DATA_STRENGTH<<2)	| (PC3_DATA_STRENGTH<<3) |
									(PC4_DATA_STRENGTH<<4)	| (PC5_DATA_STRENGTH<<5)  | (PC6_DATA_STRENGTH<<6)	| (PC7_DATA_STRENGTH<<7) );
	reg_gpio_pc_gpio =
		(PC0_FUNC==AS_GPIO ? BIT(0):0)	| (PC1_FUNC==AS_GPIO ? BIT(1):0)| (PC2_FUNC==AS_GPIO ? BIT(2):0)| (PC3_FUNC==AS_GPIO ? BIT(3):0) |
		(PC4_FUNC==AS_GPIO ? BIT(4):0)	| (PC5_FUNC==AS_GPIO ? BIT(5):0)| (PC6_FUNC==AS_GPIO ? BIT(6):0)| (PC7_FUNC==AS_GPIO ? BIT(7):0);

	//PD group
	reg_gpio_pd_setting1 =
		(PD0_INPUT_ENABLE<<8) 	| (PD1_INPUT_ENABLE<<9)	| (PD2_INPUT_ENABLE<<10)	| (PD3_INPUT_ENABLE<<11) |
		(PD4_INPUT_ENABLE<<12)	| (PD5_INPUT_ENABLE<<13)| (PD6_INPUT_ENABLE<<14)	| (PD7_INPUT_ENABLE<<15) |
		((PD0_OUTPUT_ENABLE?0:1)<<16)	| ((PD1_OUTPUT_ENABLE?0:1)<<17) | ((PD2_OUTPUT_ENABLE?0:1)<<18)	| ((PD3_OUTPUT_ENABLE?0:1)<<19) |
		((PD4_OUTPUT_ENABLE?0:1)<<20)	| ((PD5_OUTPUT_ENABLE?0:1)<<21) | ((PD6_OUTPUT_ENABLE?0:1)<<22)	| ((PD7_OUTPUT_ENABLE?0:1)<<23) |
		(PD0_DATA_OUT<<24)	| (PD1_DATA_OUT<<25)	| (PD2_DATA_OUT<<26)	| (PD3_DATA_OUT<<27) |
		(PD4_DATA_OUT<<28)	| (PD5_DATA_OUT<<29)	| (PD6_DATA_OUT<<30)	| (PD7_DATA_OUT<<31) ;
	reg_gpio_pd_setting2 =
		(PD0_DATA_STRENGTH<<8)	| (PD1_DATA_STRENGTH<<9)	| (PD2_DATA_STRENGTH<<10)	| (PD3_DATA_STRENGTH<<11) |
		(PD4_DATA_STRENGTH<<12)	| (PD5_DATA_STRENGTH<<13)	| (PD6_DATA_STRENGTH<<14)	| (PD7_DATA_STRENGTH<<15) |
		(PD0_FUNC==AS_GPIO ? BIT(16):0)	| (PD1_FUNC==AS_GPIO ? BIT(17):0)| (PD2_FUNC==AS_GPIO ? BIT(18):0)| (PD3_FUNC==AS_GPIO ? BIT(19):0) |
		(PD4_FUNC==AS_GPIO ? BIT(20):0)	| (PD5_FUNC==AS_GPIO ? BIT(21):0)| (PD6_FUNC==AS_GPIO ? BIT(22):0)| (PD7_FUNC==AS_GPIO ? BIT(23):0);

	//PE group
	reg_gpio_pe_ie = (PE0_INPUT_ENABLE<<0)	| (PE1_INPUT_ENABLE<<1)| (PE2_INPUT_ENABLE<<2)	| (PE3_INPUT_ENABLE<<3);
	reg_gpio_pe_out = (PE0_DATA_OUT<<0)	| (PE1_DATA_OUT<<1)	| (PE2_DATA_OUT<<2)	| (PE3_DATA_OUT<<3);
	reg_gpio_pe_oen = ((PE0_OUTPUT_ENABLE?0:1)<<0)	| ((PE1_OUTPUT_ENABLE?0:1)<<1) | ((PE2_OUTPUT_ENABLE?0:1)<<2)	| ((PE3_OUTPUT_ENABLE?0:1)<<3);
	reg_gpio_pe_ds = (PE0_DATA_STRENGTH<<0)	| (PE1_DATA_STRENGTH<<1)	| (PE2_DATA_STRENGTH<<2)	| (PE3_DATA_STRENGTH<<3);
	reg_gpio_pe_gpio = (PE0_FUNC==AS_GPIO ? BIT(0):0)	| (PE1_FUNC==AS_GPIO ? BIT(1):0)| (PE2_FUNC==AS_GPIO ? BIT(2):0)| (PE3_FUNC==AS_GPIO ? BIT(3):0);

	if (anaRes_init_en)	{
		analog_write (0x0e,  PULL_WAKEUP_SRC_PA0 |
							(PULL_WAKEUP_SRC_PA1<<2) |
							(PULL_WAKEUP_SRC_PA2<<4) |
							(PULL_WAKEUP_SRC_PA3<<6));

		analog_write (0x0f,  PULL_WAKEUP_SRC_PA4 |
							(PULL_WAKEUP_SRC_PA5<<2) |
							(PULL_WAKEUP_SRC_PA6<<4) |
							(PULL_WAKEUP_SRC_PA7<<6));


		analog_write (0x10,  PULL_WAKEUP_SRC_PB0 |
							(PULL_WAKEUP_SRC_PB1<<2) |
							(PULL_WAKEUP_SRC_PB2<<4) |
							(PULL_WAKEUP_SRC_PB3<<6));

#if USE_SENSOR_RC2417
		u8 pb5_pull = rc2417_is_powered() ? PM_PIN_PULLDOWN_100K : PULL_WAKEUP_SRC_PB5;
#else
		u8 pb5_pull = PULL_WAKEUP_SRC_PB5;
#endif
		analog_write (0x11,  PULL_WAKEUP_SRC_PB4 |
							(pb5_pull<<2) |
							(PULL_WAKEUP_SRC_PB6<<4) |
							(PULL_WAKEUP_SRC_PB7<<6));


		analog_write (0x12,  PULL_WAKEUP_SRC_PC0 |
							(PULL_WAKEUP_SRC_PC1<<2) |
							(PULL_WAKEUP_SRC_PC2<<4) |
							(PULL_WAKEUP_SRC_PC3<<6));

		analog_write (0x13,  PULL_WAKEUP_SRC_PC4 |
							(PULL_WAKEUP_SRC_PC5<<2) |
							(PULL_WAKEUP_SRC_PC6<<4) |
							(PULL_WAKEUP_SRC_PC7<<6));


		analog_write (0x14,  PULL_WAKEUP_SRC_PD0 |
							(PULL_WAKEUP_SRC_PD1<<2) |
							(PULL_WAKEUP_SRC_PD2<<4) |
							(PULL_WAKEUP_SRC_PD3<<6));

		analog_write (0x15,  PULL_WAKEUP_SRC_PD4 |
							(PULL_WAKEUP_SRC_PD5<<2) |
							(PULL_WAKEUP_SRC_PD6<<4) |
							(PULL_WAKEUP_SRC_PD7<<6));
	}
}
#endif

static startup_state_e platform_wakeup_init(void)
{
    startup_state_e state = SYSTEM_BOOT;

#if defined(MCU_CORE_826x) || defined(MCU_CORE_8258)
    cpu_wakeup_init();
#elif defined(MCU_CORE_8278)
    cpu_wakeup_init(LDO_MODE, INTERNAL_CAP_XTAL24M);
#elif defined(MCU_CORE_B91)
    sys_init(LDO_1P4_LDO_1P8, VBAT_MAX_VALUE_GREATER_THAN_3V6, INTERNAL_CAP_XTAL24M);
#elif defined(MCU_CORE_B92)
    sys_init(LDO_1P4_LDO_2P0, VBAT_MAX_VALUE_GREATER_THAN_3V6, GPIO_VOLTAGE_3V3, INTERNAL_CAP_XTAL24M);
#elif defined(MCU_CORE_TL721X)
    sys_init(LDO_0P94_LDO_1P8, VBAT_MAX_VALUE_GREATER_THAN_3V6, INTERNAL_CAP_XTAL24M);
#elif defined(MCU_CORE_TL321X)
    sys_init(LDO_1P25_LDO_1P8, VBAT_MAX_VALUE_GREATER_THAN_3V6, INTERNAL_CAP_XTAL24M);
#endif

#if defined(MCU_CORE_B92) || defined(MCU_CORE_TL721X) || defined(MCU_CORE_TL321X)
    wd_32k_stop();
    wd_stop();
#endif

#if defined(MCU_CORE_B92) || defined(MCU_CORE_TL721X) || defined(MCU_CORE_TL321X)
    pm_update_status_info(1);
#endif

#if defined(MCU_CORE_826x)
    //826x not support ram retention.
    state = (pm_mcu_status == MCU_STATUS_DEEP_BACK) ? SYSTEM_DEEP : SYSTEM_BOOT;
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
    state = (startup_state_e)pm_get_mcu_status();
#elif defined(MCU_CORE_B91) || defined(MCU_CORE_B92) || defined(MCU_CORE_TL721X) || defined(MCU_CORE_TL321X)
    if (g_pm_status_info.mcu_status == MCU_STATUS_DEEPRET_BACK) {
        state = SYSTEM_DEEP_RETENTION;
    } else if (g_pm_status_info.mcu_status == MCU_STATUS_DEEP_BACK) {
        state = SYSTEM_DEEP;
    }
#endif

    return state;
}

/****************************************************************************************************
* @brief 		platform initialization function
*
* @param[in] 	none
*
* @return	  	startup_state_e.
*/
startup_state_e drv_platform_init(void)
{
    drv_disable_irq();
    drv_irqMask_clear();

    startup_state_e state = platform_wakeup_init();

#if defined(MCU_CORE_826x) || defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
    clock_init(SYS_CLOCK_VALUE);
#elif defined(MCU_CORE_B91) || defined(MCU_CORE_B92) || defined(MCU_CORE_TL721X) || defined(MCU_CORE_TL321X)
    CLOCK_INIT;
#endif

    /* Get system ticks per US, must be after the clock is initialized. */
#if defined(MCU_CORE_826x)
    sysTimerPerUs = tickPerUs;
#elif defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
    sysTimerPerUs = sys_tick_per_us;
#elif defined(MCU_CORE_B91) || defined(MCU_CORE_B92) || defined(MCU_CORE_TL721X) || defined(MCU_CORE_TL321X)
    sysTimerPerUs = SYSTEM_TIMER_TICK_1US;
#endif

#if (BOARD == BOARD_PRUCHO2_PIR_ZG204ZL)
    /* Exact PVVX GPIO register + analog pull initialization, 0x2D only. */
    prucho2_pvvx_gpio_init(TRUE);
#else
    gpio_init(TRUE);
#endif

#if UART_PRINTF_MODE
#if CONSOLE_UART_ENABLE
    drv_console_pin_set(CONSOLE_TX_PIN, CONSOLE_RX_PIN);
    drv_console_init(CONSOLE_UART_BAUDRATE);
#else
    DEBUG_TX_PIN_INIT();
#endif
#endif

    if (state != SYSTEM_DEEP_RETENTION) {
        randInit();
        internalFlashSizeCheck();

#if FLASH_PROTECT_ENABLE
        flash_loadOpt();
        flash_lock();
#endif

#if PM_ENABLE
        PM_CLOCK_INIT();
#endif
    } else {
#if PM_ENABLE
        drv_pm_wakeupTimeUpdate();
#endif
    }

    /* Get calibration info to improve performance */
    drv_calibration();

    /* ADC */
#if VOLTAGE_DETECT_ENABLE
    voltage_detect_init(VOLTAGE_DETECT_ADC_PIN);

    voltage_detect((state == SYSTEM_BOOT) ? 1 : 0);
#endif

#if defined(MCU_CORE_TL721X) || defined(MCU_CORE_TL321X)
    /* AES enable after clock_init */
    ske_dig_en();
#endif

    /* RF */
    ZB_RADIO_INIT();
    ZB_TIMER_INIT();

#if defined(MCU_CORE_8258)
    if (flash_is_zb()) {
#if (!VOLTAGE_DETECT_ENABLE) || !defined(VOLTAGE_DETECT_ENABLE)
        voltage_detect_init(VOLTAGE_DETECT_ADC_PIN);
        flash_safe_voltage_set(BATTERY_SAFETY_THRESHOLD);
#endif
    }
#endif

    return state;
}

void drv_enable_irq(void)
{
#if defined(MCU_CORE_826x) || defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
    irq_enable();
#elif defined(MCU_CORE_B91) || defined(MCU_CORE_B92) || defined(MCU_CORE_TL721X) || defined(MCU_CORE_TL321X)
    core_interrupt_enable();
#endif
}

u32 drv_disable_irq(void)
{
#if defined(MCU_CORE_826x) || defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
    return (u32)irq_disable();
#elif defined(MCU_CORE_B91) || defined(MCU_CORE_B92) || defined(MCU_CORE_TL721X) || defined(MCU_CORE_TL321X)
    return core_interrupt_disable();
#endif
}

void drv_irqMask_clear(void)
{
#if defined(MCU_CORE_826x) || defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
    irq_disable_type(FLD_IRQ_ALL);
#elif defined(MCU_CORE_B91) || defined(MCU_CORE_B92) || defined(MCU_CORE_TL721X) || defined(MCU_CORE_TL321X)
    reg_irq_src0 = 0;
    reg_irq_src1 = 0;
    core_mie_disable(FLD_MIE_MSIE);
    core_mie_disable(FLD_MIE_MTIE);
    core_mie_disable(FLD_MIE_MEIE);
#endif
}

u32 drv_restore_irq(u32 en)
{
#if defined(MCU_CORE_826x) || defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
    irq_restore((u8)en);
    return 0;
#elif defined(MCU_CORE_B91) || defined(MCU_CORE_B92) || defined(MCU_CORE_TL721X) || defined(MCU_CORE_TL321X)
    return core_restore_interrupt(en);
#endif
}

void drv_wd_setInterval(u32 ms)
{
    wd_set_interval_ms(ms);
}

void drv_wd_start(void)
{
    wd_start();
}

void drv_wd_clear(void)
{
    wd_clear();
}

u32 drv_u32Rand(void)
{
#if defined(MCU_CORE_826x) || defined(MCU_CORE_8258) || defined(MCU_CORE_8278)
    return rand();
#elif defined(MCU_CORE_B91) || defined(MCU_CORE_B92) || defined(MCU_CORE_TL721X) || defined(MCU_CORE_TL321X)
    return trng_rand();
#else
    return 0;
#endif
}

void drv_generateRandomData(u8 *pData, u8 len)
{
    u32 randNums = 0;
    /* if len is odd */
    for (u8 i = 0; i < len; i++) {
        if ((i & 3) == 0) {
            randNums = drv_u32Rand();
        }

        pData[i] = randNums & 0xff;
        randNums >>= 8;
    }
}

/*For vbus supply -- close vbus watchdog, watchdog time is 8s.*/
#if defined(MCU_CORE_B92)
volatile u32 g_vbus_timer_turn_off_start_tick = 0;
volatile u8 g_vbus_timer_turn_off_flag = 0;
void drv_vbusWatchdogClose(void)
{
    if (usb_get_vbus_detect_status()) {
        if (g_vbus_timer_turn_off_flag == 0) {
            if (g_vbus_timer_turn_off_start_tick == 0) {
                g_vbus_timer_turn_off_start_tick = stimer_get_tick();
            }

            if (clock_time_exceed(g_vbus_timer_turn_off_start_tick, 100 * 1000)) {
                wd_turn_off_vbus_timer();//clear reset

                g_vbus_timer_turn_off_start_tick = 0;
                g_vbus_timer_turn_off_flag = 1;
            }
        }
    } else if (g_vbus_timer_turn_off_flag) {
        g_vbus_timer_turn_off_start_tick = 0;
        g_vbus_timer_turn_off_flag = 0;
    }
}
#endif
