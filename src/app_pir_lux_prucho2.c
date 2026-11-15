#include "app_main.h"

#if (BOARD == BOARD_PRUCHO2_PIR_ZG204ZL)

/* Shared board/firmware for fitted/unfitted lux components.
 * ADC hardware description is taken from the measured ZG-204ZL wiring;
 * ZCL and reporting are Slacky style, not copied from PVVX reporting.c.
 */
#define PRUCHO2_LUX_PERIOD_MS 10000
#define PRUCHO2_LUX_MIN_ZCL_DELTA 10
#define PRUCHO2_LUX_SYNC_MS 3600000UL

static bool sample_due = false;
static bool need_report = false;
static uint16_t last_reported_lux = 0;
static uint32_t last_report_tick = 0;
static ev_timer_event_t *luxTimer = NULL;

/* Convert lux to standard Zigbee Illuminance measuredValue:
 * 10000*log10(lux)+1. Integer fixed-point log2 avoids libm and FPU.
 * 0 lux is represented by ZCL 0.
 */
static uint16_t zcl_illuminance(uint32_t lux) {
    if (!lux) return 0;
    uint32_t n = 0, fraction = 0;
    uint32_t normalized = lux;
    while (normalized >= 2) { normalized >>= 1; ++n; }
    uint32_t v = (uint32_t)(((uint64_t)lux << 16) >> n);
    for (uint8_t i = 0; i < 10; ++i) {
        v = (uint32_t)(((uint64_t)v * v) >> 16);
        if (v >= (2UL << 16)) {
            v >>= 1;
            fraction |= 1U << (9-i);
        }
    }
    uint32_t z = ((n*1024UL+fraction)*30103UL)/10240UL+1UL;
    return z > 0xfffe ? 0xfffe : (uint16_t)z;
}

static uint16_t read_lux(void) {
    /* Read battery before changing ADC mode; restore ADC battery config
     * after measuring PC4 so Slacky's battery service remains operational.
     */
    uint32_t vbat = drv_get_adc_data();
    drv_gpio_write(PRUCHO2_PIR_LUX_POWER_GPIO, true);
    drv_gpio_output_en(PRUCHO2_PIR_LUX_POWER_GPIO, true);
    delay_us(500);
    drv_adc_mode_pin_set(DRV_ADC_BASE_MODE, PRUCHO2_PIR_LUX_ADC_GPIO);
    uint32_t sense = drv_get_adc_data();
    drv_adc_mode_pin_set(DRV_ADC_VBAT_MODE, VOLTAGE_DETECT_PIN);
    drv_gpio_write(PRUCHO2_PIR_LUX_POWER_GPIO, false);
    drv_gpio_output_en(PRUCHO2_PIR_LUX_POWER_GPIO, false);

    if (!vbat || !sense) return 0;
    if (sense > vbat) sense = vbat;
    /* ZG-204ZL photodetector voltage ratio and fourth-power response.
     * Scale to ~1000 lux, pending physical calibration on TEST.
     */
    uint32_t ratio = (sense << 16) / vbat;
    uint32_t ratio2 = (uint32_t)(((uint64_t)ratio * ratio) >> 16);
    uint32_t ratio4 = (uint32_t)(((uint64_t)ratio2 * ratio2) >> 16);
    uint32_t lux = (ratio4 * 1000UL) >> 16;
    return zcl_illuminance(lux);
}

static int32_t lux_periodCb(void *arg) {
    sample_due = true;
    return 0;
}

void prucho2_lux_init(void) {
    drv_gpio_write(PRUCHO2_PIR_LUX_POWER_GPIO, false);
    drv_gpio_output_en(PRUCHO2_PIR_LUX_POWER_GPIO, false);
    if (!luxTimer) luxTimer = TL_ZB_TIMER_SCHEDULE(lux_periodCb, NULL, PRUCHO2_LUX_PERIOD_MS);
    sample_due = true;
    last_report_tick = clock_time();
}

void prucho2_lux_task(void) {
    if (sample_due) {
        sample_due = false;
        uint16_t next = read_lux();
        if (next != prucho2_lux) {
            prucho2_lux = next;
            uint16_t change = next > last_reported_lux ? next-last_reported_lux : last_reported_lux-next;
            if (change >= PRUCHO2_LUX_MIN_ZCL_DELTA) need_report = true;
        }
    }
    if (clock_time_exceed(last_report_tick, PRUCHO2_LUX_SYNC_MS * 1000UL)) need_report = true;
    if (need_report && zb_isDeviceJoinedNwk()) {
        epInfo_t dst;
        TL_SETSTRUCTCONTENT(dst, 0);
        dst.profileId = HA_PROFILE_ID;
        dst.dstAddrMode = APS_DSTADDR_EP_NOTPRESETNT;
        status_t status = zcl_sendReportCmd(APP_ENDPOINT1, &dst, TRUE, ZCL_FRAME_SERVER_CLIENT_DIR,
                ZCL_CLUSTER_MS_ILLUMINANCE_MEASUREMENT, ZCL_ATTRID_MEASURED_VALUE,
                ZCL_DATA_TYPE_UINT16, (uint8_t *)&prucho2_lux);
        if (status == ZCL_STA_SUCCESS) {
            need_report = false;
            last_reported_lux = prucho2_lux;
            last_report_tick = clock_time();
        }
    }
}
#endif
