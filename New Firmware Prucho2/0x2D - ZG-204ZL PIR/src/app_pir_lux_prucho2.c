#include "app_main.h"

#if (BOARD == BOARD_PRUCHO2_PIR_ZG204ZL)

/* Shared board/firmware for fitted/unfitted lux components.
 * ADC hardware description is taken from the measured ZG-204ZL wiring;
 * ZCL and reporting are Slacky style, not copied from PVVX reporting.c.
 */
#define PRUCHO2_LUX_PERIOD_MS 2000
#define PRUCHO2_LUX_MIN_DELTA_LX 20UL
#define PRUCHO2_LUX_SYNC_MS 30000UL
/* Independent diagnostic heartbeat; does NOT change actual lux reporting. */
#define PRUCHO2_LUX_DIAG_MS 15000UL
/* 0xF200..0xF209: all read-only counters/raw data. */
uint32_t prucho2_lux_diag_timer_ticks = 0;
uint32_t prucho2_lux_diag_samples = 0;
uint16_t prucho2_lux_diag_vbat_adc = 0;
uint16_t prucho2_lux_diag_pc4_adc = 0;
uint16_t prucho2_lux_diag_zcl = 0;
uint32_t prucho2_lux_diag_report_attempts = 0;
uint32_t prucho2_lux_diag_reports_enqueued = 0;
uint32_t prucho2_lux_diag_reports_failed = 0;
uint32_t prucho2_lux_diag_snapshots_enqueued = 0;
uint8_t prucho2_lux_diag_status = 0; /* bits: 1 sampled, 2 ADC zero, 4 joined, 8 pending report */
static bool lux_diag_pending = true;
static uint32_t lux_diag_sent_at = 0;
static bool lux_diag_sent_once = false;


static bool sample_due = false;
static bool need_report = false;
static uint32_t last_reported_lux = 0;
static uint32_t current_lux = 0;
static bool first_report = true;
static uint32_t last_report_tick = 0;
static ev_timer_event_t *luxTimer = NULL;

static void lux_diag_send(void) {
    if (!zb_isDeviceJoinedNwk()) return;
    prucho2_lux_diag_status = (prucho2_lux_diag_status & 0x03)
        | 0x04 | (need_report ? 0x08 : 0);
    if (lux_diag_sent_once &&
            !clock_time_exceed(lux_diag_sent_at, PRUCHO2_LUX_DIAG_MS * 1000UL)) return;
    const u16 ids[10] = {
        0xF200,0xF201,0xF202,0xF203,0xF204,
        0xF205,0xF206,0xF207,0xF208,0xF209
    };
    struct {
        u8 numAttr;
        zclReport_t attr[10];
    } report;
    report.numAttr = 0;
    for (u8 i=0; i<10; ++i) {
        zclAttrInfo_t *a = zcl_findAttribute(APP_ENDPOINT1,
            ZCL_CLUSTER_MS_ILLUMINANCE_MEASUREMENT, ids[i]);
        if (!a || !a->data) return;
        report.attr[i].attrID = a->id;
        report.attr[i].dataType = a->type;
        report.attr[i].attrData = a->data;
        ++report.numAttr;
    }
    epInfo_t dst;
    TL_SETSTRUCTCONTENT(dst, 0);
    dst.profileId = HA_PROFILE_ID;
    dst.dstAddrMode = APS_DSTADDR_EP_NOTPRESETNT;
    status_t st = zcl_sendReportAttrsCmd(APP_ENDPOINT1, &dst, TRUE,
            ZCL_FRAME_SERVER_CLIENT_DIR, ZCL_CLUSTER_MS_ILLUMINANCE_MEASUREMENT,
            (zclReportCmd_t *)&report);
    if (st == ZCL_STA_SUCCESS) {
        ++prucho2_lux_diag_snapshots_enqueued;
        lux_diag_pending = false;
        lux_diag_sent_once = true;
        lux_diag_sent_at = clock_time();
    }
}


/* Convert lux to standard Zigbee Illuminance measuredValue:
 * 10000*log10(lux)+1. Integer fixed-point log2 avoids libm and FPU.
 * 0 lux is represented by ZCL 0.
 */
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
    /* Read battery before changing ADC mode; restore ADC battery config
     * after measuring PC4 so Slacky's battery service remains operational.
     */
    uint32_t vbat = drv_get_adc_data();
    drv_gpio_write(PRUCHO2_PIR_LUX_POWER_GPIO, true);
    drv_gpio_output_en(PRUCHO2_PIR_LUX_POWER_GPIO, true);
    sleep_ms(1);
    drv_adc_mode_pin_set(DRV_ADC_BASE_MODE, PRUCHO2_PIR_LUX_ADC_GPIO);
    uint32_t sense = drv_get_adc_data();
    prucho2_lux_diag_vbat_adc = (uint16_t)vbat;
    prucho2_lux_diag_pc4_adc = (uint16_t)sense;
    prucho2_lux_diag_status = 0x01 | ((!vbat || !sense) ? 0x02 : 0);
    ++prucho2_lux_diag_samples;
    drv_adc_mode_pin_set(DRV_ADC_VBAT_MODE, VOLTAGE_DETECT_PIN);
    drv_gpio_write(PRUCHO2_PIR_LUX_POWER_GPIO, false);
    drv_gpio_output_en(PRUCHO2_PIR_LUX_POWER_GPIO, false);

    if (!vbat || !sense) return 0;
    if (sense > vbat) sense = vbat;
    /* ZG-204ZL photodetector voltage ratio and fourth-power response.
     * Scale to ~1000 lux, pending physical calibration on TEST.
     */
    /* 12-bit Q12 fixed point: every multiplication fits in uint32_t,
     * including the 1000-lux scaling. TC32 linker has no 64-bit helpers. */
    uint32_t ratio = (sense << 12) / vbat;
    uint32_t ratio2 = (ratio * ratio) >> 12;
    uint32_t ratio4 = (ratio2 * ratio2) >> 12;
    return (ratio4 * 1000UL) >> 12;
}

static int32_t lux_periodCb(void *arg) {
    ++prucho2_lux_diag_timer_ticks;
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
    lux_diag_pending = true;
    lux_diag_sent_once = false;
}

void prucho2_lux_task(void) {
    if (sample_due) {
        sample_due = false;
        current_lux = read_lux();
        prucho2_lux = zcl_illuminance(current_lux);
        prucho2_lux_diag_zcl = prucho2_lux;
        lux_diag_pending = true;
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
        ++prucho2_lux_diag_report_attempts;
        status_t status = zcl_sendReportCmd(APP_ENDPOINT1, &dst, TRUE, ZCL_FRAME_SERVER_CLIENT_DIR,
                ZCL_CLUSTER_MS_ILLUMINANCE_MEASUREMENT, ZCL_ATTRID_MEASURED_VALUE,
                ZCL_DATA_TYPE_UINT16, (uint8_t *)&prucho2_lux);
        if (status == ZCL_STA_SUCCESS) {
            ++prucho2_lux_diag_reports_enqueued;
            need_report = false;
            last_reported_lux = current_lux;
            last_report_tick = clock_time();
            first_report = false;
        } else {
            ++prucho2_lux_diag_reports_failed;
        }
    }
    lux_diag_send();
}
#endif
