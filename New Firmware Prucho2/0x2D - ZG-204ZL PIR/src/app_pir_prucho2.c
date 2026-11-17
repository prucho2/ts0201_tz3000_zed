#include "app_main.h"

#if (BOARD == BOARD_PRUCHO2_PIR_ZG204ZL)

/*
 * v0.1.47 test: original v0.1.41 base and PVVX sensor_pir.c motion lifecycle.
 * No changes to original v0.1.41 lux / Zigbee light reporting.
 * PVVX: detect HIGH -> mark occupied -> start hold timer;
 * timer checks PIR again, stays occupied while HIGH, clears after LOW;
 * arm GPIO wake against the sampled electrical PD4 level in every task pass.
 */
#define PRUCHO2_PIR_DIAG_PERIOD_MS 10000UL

static bool raw_high = false;
static bool occupancy_pending = false;
static bool diag_pending = true;
static bool diag_sent_once = false;
static bool pir_confirm_active = false;
static uint8_t pir_confirm_count = 0;
static uint32_t pir_confirm_tick = 0;
#define PIR_CONFIRM_SAMPLES 3
#define PIR_CONFIRM_MS 5UL
static uint32_t diag_last_sent = 0;
static ev_timer_event_t *occupancyTimer = NULL;

/* Diagnostic ZCL attributes, read-only; reset after a cold boot. */
uint8_t prucho2_diag_raw_gpio = 0;
uint32_t prucho2_diag_rising_edges = 0;
uint32_t prucho2_diag_falling_edges = 0;
uint32_t prucho2_diag_reports_requested = 0;
uint32_t prucho2_diag_reports_enqueued = 0;
uint32_t prucho2_diag_reports_retries = 0;
uint32_t prucho2_diag_timer_checks = 0;
uint32_t prucho2_diag_gpio_high_samples = 0;
uint32_t prucho2_diag_gpio_low_samples = 0;
uint32_t prucho2_diag_sleep_returns = 0;
uint32_t prucho2_diag_poll_samples = 0;
/* Actual GPIO port D hardware registers; PD4 is BIT(4). */
uint8_t prucho2_diag_pd_in = 0;
uint8_t prucho2_diag_pd_ie = 0;
uint8_t prucho2_diag_pd_oen = 0;
uint8_t prucho2_diag_pd_gpio = 0;
uint8_t prucho2_diag_pd_out = 0;
uint8_t prucho2_diag_pd_irq_en = 0;
static uint32_t pd_register_last_sent = 0;
static bool pd_register_sent_once = false;

static void snapshot_pd_registers(void) {
    prucho2_diag_pd_in = reg_gpio_pd_in;
    prucho2_diag_pd_ie = reg_gpio_pd_ie;
    prucho2_diag_pd_oen = reg_gpio_pd_oen;
    prucho2_diag_pd_gpio = reg_gpio_pd_gpio;
    prucho2_diag_pd_out = reg_gpio_pd_out;
    prucho2_diag_pd_irq_en = reg_gpio_pd_irq_en;
}

static void report_pd_registers(void) {
    if (!zb_isDeviceJoinedNwk()) return;
    if (pd_register_sent_once && !clock_time_exceed(pd_register_last_sent, 10000UL * 1000UL)) return;
    const u16 ids[6] = {0xF10B, 0xF10C, 0xF10D, 0xF10E, 0xF10F, 0xF110};
    struct { u8 numAttr; zclReport_t attr[6]; } report;
    report.numAttr = 0;
    for (u8 i = 0; i < 6; ++i) {
        zclAttrInfo_t *a = zcl_findAttribute(APP_ENDPOINT1, ZCL_CLUSTER_MS_OCCUPANCY_SENSING, ids[i]);
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
        ZCL_FRAME_SERVER_CLIENT_DIR, ZCL_CLUSTER_MS_OCCUPANCY_SENSING,
        (zclReportCmd_t *)&report);
    if (st == ZCL_STA_SUCCESS) {
        pd_register_sent_once = true;
        pd_register_last_sent = clock_time();
    }
}


void prucho2_pir_note_wakeup(void) {
    ++prucho2_diag_sleep_returns;
}

static bool read_pir(void) {
    /* On TLSR8253, Slacky's wrapper is Telink gpio_read(). */
    return drv_gpio_read(PRUCHO2_PIR_GPIO) == PRUCHO2_PIR_ACTIVE_LEVEL;
}

static void request_occupancy(uint8_t value) {
    if (prucho2_occupancy != value) {
        prucho2_occupancy = value;
        occupancy_pending = true;
        ++prucho2_diag_reports_requested;
    }
}

static void report_occupancy(void) {
    if (!occupancy_pending || !zb_isDeviceJoinedNwk()) return;
    epInfo_t dst;
    TL_SETSTRUCTCONTENT(dst, 0);
    dst.profileId = HA_PROFILE_ID;
    dst.dstAddrMode = APS_DSTADDR_EP_NOTPRESETNT;
    status_t st = zcl_sendReportCmd(APP_ENDPOINT1, &dst, TRUE,
            ZCL_FRAME_SERVER_CLIENT_DIR, ZCL_CLUSTER_MS_OCCUPANCY_SENSING,
            ZCL_ATTRID_OCCUPANCY, ZCL_DATA_TYPE_BITMAP8, &prucho2_occupancy);
    if (st == ZCL_STA_SUCCESS) {
        occupancy_pending = false;
        ++prucho2_diag_reports_enqueued;
    } else {
        ++prucho2_diag_reports_retries;
    }
}

/* Full 11-field diagnostic snapshot every 10 seconds while joined.
 * Transitions still trigger an early snapshot, at most once per interval.
 */
static void report_diagnostics(void) {
    if (!zb_isDeviceJoinedNwk()) return;
    if (diag_sent_once && !clock_time_exceed(diag_last_sent, PRUCHO2_PIR_DIAG_PERIOD_MS * 1000UL)) return;
    struct {
        u8 numAttr;
        zclReport_t attr[11];
    } report;
    const u16 ids[11] = {
        0xF100, 0xF101, 0xF102, 0xF103, 0xF104, 0xF105,
        0xF106, 0xF107, 0xF108, 0xF109, 0xF10A
    };
    report.numAttr = 0;
    for (u8 i = 0; i < 11; ++i) {
        zclAttrInfo_t *a = zcl_findAttribute(APP_ENDPOINT1,
                ZCL_CLUSTER_MS_OCCUPANCY_SENSING, ids[i]);
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
            ZCL_FRAME_SERVER_CLIENT_DIR, ZCL_CLUSTER_MS_OCCUPANCY_SENSING,
            (zclReportCmd_t *)&report);
    if (st == ZCL_STA_SUCCESS) {
        diag_pending = false;
        diag_sent_once = true;
        diag_last_sent = clock_time();
    }
}

/* PVVX occupancyTimerCb: only clear occupied after the hold timer fires
 * AND the physical PIR pin is LOW. If still HIGH, repeat the same timer.
 */
static int32_t occupancyTimerCb(void *arg) {
    ++prucho2_diag_timer_checks;
    bool active = read_pir();
    prucho2_diag_raw_gpio = active ? 1 : 0;
    ++prucho2_diag_poll_samples;
    if (active) {
        ++prucho2_diag_gpio_high_samples;
        return 0; /* PVVX repeat timer period while PIR output remains HIGH */
    }
    ++prucho2_diag_gpio_low_samples;
    request_occupancy(0);
    occupancyTimer = NULL;
    diag_pending = true;
    return -1;
}


void prucho2_pir_init(void) {
    /* Same digital GPIO assignment as PVVX's ZG-204ZL board definition. */
    drv_gpio_func_set(PRUCHO2_PIR_GPIO);
    drv_gpio_output_en(PRUCHO2_PIR_GPIO, false);
    drv_gpio_input_en(PRUCHO2_PIR_GPIO, true);
    snapshot_pd_registers();
    raw_high = read_pir();
    prucho2_diag_raw_gpio = raw_high ? 1 : 0;
    prucho2_occupancy = raw_high ? 1 : 0;
    occupancy_pending = true;
    diag_pending = true;
    diag_sent_once = false;
    if (raw_high) {
        uint32_t hold = prucho2_occupancy_hold;
        if (hold < 1) hold = 1;
        if (hold > 600) hold = 600;
        occupancyTimer = TL_ZB_TIMER_SCHEDULE(occupancyTimerCb, NULL, hold * 1000UL);
    }
}

void prucho2_pir_task(void) {
    snapshot_pd_registers();
    bool active = read_pir();
    prucho2_diag_raw_gpio = active ? 1 : 0;
    ++prucho2_diag_poll_samples;
    if (active) ++prucho2_diag_gpio_high_samples;
    else ++prucho2_diag_gpio_low_samples;

    if (active != raw_high) {
        raw_high = active;
        diag_pending = true;
        if (active) ++prucho2_diag_rising_edges;
        else ++prucho2_diag_falling_edges;
        pir_confirm_active = true;
        pir_confirm_count = 0;
        pir_confirm_tick = clock_time();
    }
    if (pir_confirm_active && clock_time_exceed(pir_confirm_tick, PIR_CONFIRM_MS * 1000UL)) {
        pir_confirm_tick = clock_time();
        if (++pir_confirm_count >= PIR_CONFIRM_SAMPLES) pir_confirm_active = false;
    }
    if (active && !prucho2_occupancy) {
        request_occupancy(1);
        /* PVVX schedules occupancy-off check on the rising edge. */
        if (occupancyTimer) TL_ZB_TIMER_CANCEL(&occupancyTimer);
        uint32_t hold = prucho2_occupancy_hold;
        if (hold < 1) hold = 1;
        if (hold > 600) hold = 600;
        occupancyTimer = TL_ZB_TIMER_SCHEDULE(occupancyTimerCb, NULL, hold * 1000UL);
    }

    /* PD4 wake level is set immediately before sleeping, TS0207 style. */

    report_occupancy();
    report_diagnostics();
    report_pd_registers();
}

void prucho2_pir_prepare_sleep(void) {
    /* Configure PA1 and PD4 against their current electrical levels. */
    extern drv_pm_pinCfg_t g_switchPmCfg[];
    drv_pm_wakeupPinLevelChange(g_switchPmCfg, 2);
}

bool prucho2_pir_idle(void) {
    return pir_confirm_active;
}

void prucho2_pir_request_sync(void) {
    occupancy_pending = true;
    report_occupancy();
    diag_pending = true;
}

#endif
