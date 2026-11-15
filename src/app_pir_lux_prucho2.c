#include "app_main.h"

#if (BOARD == BOARD_PRUCHO2_PIR_ZG204ZL)

/*
 * Battery-friendly light reporting:
 * - ADC sampling every 60 s (local, no radio transmission).
 * - One initial report after Zigbee join.
 * - Subsequent reports ONLY if physical light changed by >= 50 lx
 *   compared with the last successfully queued report.
 * - No periodic hourly synchronization and no 10 s forced reports.
 * - One shared image for PCB variants with/without the lux components.
 */
#define PRUCHO2_LUX_SAMPLE_MS   60000UL
#define PRUCHO2_LUX_DELTA_LX    50UL

static bool sample_due = false;
static bool first_report = true;
static bool need_report = false;
static uint32_t last_reported_lux = 0;
static uint32_t measured_lux = 0;
static ev_timer_event_t *luxTimer = NULL;

/* Zigbee illuminance cluster uses measuredValue = 10000*log10(lx)+1.
 * 64-bit intermediates are important: fixed-point squaring overflows u32.
 */
static uint16_t zcl_illuminance(uint32_t lux) {
    if (lux == 0) return 0;
    uint32_t n = 0;
    uint32_t fraction = 0;
    uint32_t normalized = lux;
    while (normalized >= 2) {
        normalized >>= 1;
        ++n;
    }
    uint64_t v = ((uint64_t)lux << 15) >> n;
    for (uint8_t i = 0; i < 10; ++i) {
        v = (v * v) >> 15;
        if (v >= ((uint64_t)2 << 15)) {
            v >>= 1;
            fraction |= (uint32_t)1U << (9-i);
        }
    }
    uint64_t z = (((uint64_t)n * 1024UL + fraction) * 30103UL) / 10240UL + 1UL;
    return z > 0xFFFEUL ? 0xFFFE : (uint16_t)z;
}

/* Returns approximate linear lux. Physical calibration still pending.
 * Existing fourth-power photodetector model retained, but overflow fixed.
 */
static uint32_t read_lux_linear(void) {
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
    uint32_t ratio = (uint32_t)(((uint64_t)sense * 65535UL) / vbat);
    uint32_t ratio2 = (uint32_t)(((uint64_t)ratio * ratio) >> 16);
    uint32_t ratio4 = (uint32_t)(((uint64_t)ratio2 * ratio2) >> 16);
    return (uint32_t)(((uint64_t)ratio4 * 1000UL) >> 16);
}

static int32_t lux_periodCb(void *arg) {
    sample_due = true;
    return 0;
}

void prucho2_lux_init(void) {
    drv_gpio_write(PRUCHO2_PIR_LUX_POWER_GPIO, false);
    drv_gpio_output_en(PRUCHO2_PIR_LUX_POWER_GPIO, false);
    if (!luxTimer)
        luxTimer = TL_ZB_TIMER_SCHEDULE(lux_periodCb, NULL, PRUCHO2_LUX_SAMPLE_MS);
    sample_due = true;
    first_report = true;
}

void prucho2_lux_task(void) {
    if (sample_due) {
        sample_due = false;
        measured_lux = read_lux_linear();
        prucho2_lux = zcl_illuminance(measured_lux);
        uint32_t diff = measured_lux > last_reported_lux
            ? measured_lux - last_reported_lux
            : last_reported_lux - measured_lux;
        need_report = first_report || (diff >= PRUCHO2_LUX_DELTA_LX);
    }
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
            first_report = false;
            last_reported_lux = measured_lux;
        }
    }
}
#endif
