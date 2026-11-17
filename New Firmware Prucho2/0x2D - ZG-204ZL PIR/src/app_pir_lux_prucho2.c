#include "app_main.h"

#if (BOARD == BOARD_PRUCHO2_PIR_ZG204ZL)

/* Battery-friendly independent lux sampler. No motion-triggered read. */
#define PRUCHO2_LUX_PERIOD_MS 20000UL
#define PRUCHO2_LUX_MIN_DELTA_LX 20UL
#define PRUCHO2_LUX_SYNC_MS 300000UL

static bool sample_due = false;
static bool need_report = false;
static uint32_t last_reported_lux = 0;
static uint32_t current_lux = 0;
static bool first_report = true;
static uint32_t last_report_tick = 0;
static ev_timer_event_t *luxTimer = NULL;

/* Convert lux to Zigbee logarithmic value (fixed point). */
static uint16_t zcl_illuminance(uint32_t lux) {
    if (!lux) return 0;
    uint32_t n = 0, fraction = 0;
    uint32_t normalized = lux;
    while (normalized >= 2) { normalized >>= 1; ++n; }
    uint32_t v = (lux << 15) >> n;
    for (uint8_t i = 0; i < 10; ++i) {
        v = (v * v) >> 15;
        if (v >= (2UL << 15)) {
            v >>= 1;
            fraction |= 1U << (9-i);
        }
    }
    uint32_t z = ((n*1024UL+fraction)*30103UL)/10240UL+1UL;
    return z > 0xfffe ? 0xfffe : (uint16_t)z;
}

static uint32_t read_lux(void) {
    /* Preserve the proven PC4/PD3 ADC sequence, including battery ADC restore. */
    uint32_t vbat = drv_get_adc_data();
    drv_gpio_write(PRUCHO2_PIR_LUX_POWER_GPIO, true);
    drv_gpio_output_en(PRUCHO2_PIR_LUX_POWER_GPIO, true);
    sleep_ms(1);
    drv_adc_mode_pin_set(DRV_ADC_BASE_MODE, PRUCHO2_PIR_LUX_ADC_GPIO);
    uint32_t sense = drv_get_adc_data();
    drv_adc_mode_pin_set(DRV_ADC_VBAT_MODE, VOLTAGE_DETECT_PIN);
    drv_gpio_write(PRUCHO2_PIR_LUX_POWER_GPIO, false);
    drv_gpio_output_en(PRUCHO2_PIR_LUX_POWER_GPIO, false);

    if (!vbat || !sense) return 0;
    if (sense > vbat) sense = vbat;
    uint32_t ratio = (sense << 12) / vbat;
    uint32_t ratio2 = (ratio * ratio) >> 12;
    uint32_t ratio4 = (ratio2 * ratio2) >> 12;
    return (ratio4 * 1000UL) >> 12;
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
    first_report = true;
    need_report = true;
}

void prucho2_lux_task(void) {
    if (sample_due) {
        sample_due = false;
        current_lux = read_lux();
        prucho2_lux = zcl_illuminance(current_lux);
        uint32_t change = current_lux > last_reported_lux
            ? current_lux - last_reported_lux : last_reported_lux - current_lux;
        if (first_report || change >= PRUCHO2_LUX_MIN_DELTA_LX) need_report = true;
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
            last_reported_lux = current_lux;
            last_report_tick = clock_time();
            first_report = false;
        }
    }
}
#endif
