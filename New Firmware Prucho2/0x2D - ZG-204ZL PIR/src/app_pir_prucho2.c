#include "app_main.h"

#if (BOARD == BOARD_PRUCHO2_PIR_ZG204ZL)

/* Slacky GPIO/PM. Preserve normalized PD4 bit-mask read, required on TLSR8253. */
static bool raw_high = false;
static bool occupancy_pending = false;
static bool pir_confirm_active = false;
static uint8_t pir_confirm_count = 0;
static uint32_t pir_confirm_tick = 0;
#define PIR_CONFIRM_SAMPLES 3
#define PIR_CONFIRM_MS 5UL
static ev_timer_event_t *occupancyTimer = NULL;
/* Up to two extra false reports at 1 s intervals, only while PIR remains idle. */
static ev_timer_event_t *occupancyFalseRepeatTimer = NULL;
static uint8_t occupancyFalseRepeatCount = 0;

static bool read_pir(void) {
    /* Telink gpio_read() returns mask (PD4=0x10), not a normalized bool. */
    return (drv_gpio_read(PRUCHO2_PIR_GPIO) != 0) ==
           (PRUCHO2_PIR_ACTIVE_LEVEL != 0);
}

static void request_occupancy(uint8_t value) {
    if (prucho2_occupancy != value) {
        prucho2_occupancy = value;
        occupancy_pending = true;
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
    if (st == ZCL_STA_SUCCESS) occupancy_pending = false;
}

/* Extra false reports are intentionally separate from normal occupancy transitions.
 * Any new motion cancels them; never publish a stale false after a new true. */
static int32_t occupancyFalseRepeatCb(void *arg) {
    if (read_pir() || prucho2_occupancy != 0) {
        occupancyFalseRepeatTimer = NULL;
        return -1;
    }
    occupancy_pending = true;
    report_occupancy();
    if (++occupancyFalseRepeatCount < 2) return 0;
    occupancyFalseRepeatTimer = NULL;
    return -1;
}

/* Retain occupied while physical PIR stays HIGH, release after the hold
 * timer (3 s by default) sees LOW. */
static int32_t occupancyTimerCb(void *arg) {
    if (read_pir()) return 0;
    request_occupancy(0);
    occupancyFalseRepeatCount = 0;
    if (occupancyFalseRepeatTimer) TL_ZB_TIMER_CANCEL(&occupancyFalseRepeatTimer);
    occupancyFalseRepeatTimer = TL_ZB_TIMER_SCHEDULE(occupancyFalseRepeatCb, NULL, 1000UL);
    occupancyTimer = NULL;
    return -1;
}

void prucho2_pir_init(void) {
    drv_gpio_func_set(PRUCHO2_PIR_GPIO);
    drv_gpio_output_en(PRUCHO2_PIR_GPIO, false);
    drv_gpio_input_en(PRUCHO2_PIR_GPIO, true);
    raw_high = read_pir();
    prucho2_occupancy = raw_high ? 1 : 0;
    occupancy_pending = true;
    if (raw_high) {
        uint32_t hold = prucho2_occupancy_hold;
        if (hold < 1) hold = 1;
        if (hold > 600) hold = 600;
        occupancyTimer = TL_ZB_TIMER_SCHEDULE(occupancyTimerCb, NULL, hold * 1000UL);
    }
}

void prucho2_pir_task(void) {
    bool active = read_pir();
    if (active != raw_high) {
        raw_high = active;
        pir_confirm_active = true;
        pir_confirm_count = 0;
        pir_confirm_tick = clock_time();
    }
    if (pir_confirm_active && clock_time_exceed(pir_confirm_tick, PIR_CONFIRM_MS * 1000UL)) {
        pir_confirm_tick = clock_time();
        if (++pir_confirm_count >= PIR_CONFIRM_SAMPLES) pir_confirm_active = false;
    }
    if (active && !prucho2_occupancy) {
        if (occupancyFalseRepeatTimer) TL_ZB_TIMER_CANCEL(&occupancyFalseRepeatTimer);
        request_occupancy(1);
        if (occupancyTimer) TL_ZB_TIMER_CANCEL(&occupancyTimer);
        uint32_t hold = prucho2_occupancy_hold;
        if (hold < 1) hold = 1;
        if (hold > 600) hold = 600;
        occupancyTimer = TL_ZB_TIMER_SCHEDULE(occupancyTimerCb, NULL, hold * 1000UL);
    }
    report_occupancy();
}

void prucho2_pir_prepare_sleep(void) {
    extern drv_pm_pinCfg_t g_switchPmCfg[];
    drv_pm_wakeupPinLevelChange(g_switchPmCfg, 2);
}
bool prucho2_pir_idle(void) { return pir_confirm_active; }
void prucho2_pir_request_sync(void) {
    occupancy_pending = true;
    report_occupancy();
}
#endif
