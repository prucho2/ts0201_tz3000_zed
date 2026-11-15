#include "app_main.h"

#if (BOARD == BOARD_PRUCHO2_PIR_ZG204ZL)

/*
 * Event handling is Slacky style: physical GPIO -> occupancy ZCL attribute ->
 * ZCL reporting command. The PVVX task_pir/reporting.c loops are NOT used.
 * Time spent in 'occupied' is never treated as a reason to force 'vacant'
 * while the GPIO still reports active motion.
 */
#define PRUCHO2_PIR_RECHECK_MS 100
#define PRUCHO2_PIR_SYNC_MS 300000

static bool raw_high = false;
static bool pending = false;
static uint32_t last_low = 0;
static ev_timer_event_t *checkEvt = NULL;
static ev_timer_event_t *syncEvt = NULL;

typedef struct {
    uint32_t rising_edges;
    uint32_t falling_edges;
    uint32_t reports_requested;
    uint32_t report_enqueued;
    uint32_t report_retries;
    uint32_t timer_checks;
} prucho2_pir_diag_t;

static prucho2_pir_diag_t diag;

static bool read_pir(void) {
    return drv_gpio_read(PRUCHO2_PIR_GPIO) == PRUCHO2_PIR_ACTIVE_LEVEL;
}

static void request_occupancy(uint8_t value) {
    if (prucho2_occupancy != value) {
        prucho2_occupancy = value;
        pending = true;
        ++diag.reports_requested;
    }
}

/* Exact same foundation ZCL reporting route as Slacky's own force reports. */
static void send_occupancy(void) {
    epInfo_t dst;
    status_t st;
    if (!pending || !zb_isDeviceJoinedNwk()) return;
    TL_SETSTRUCTCONTENT(dst, 0);
    dst.profileId = HA_PROFILE_ID;
    dst.dstAddrMode = APS_DSTADDR_EP_NOTPRESETNT;
    st = zcl_sendReportCmd(APP_ENDPOINT1, &dst, TRUE, ZCL_FRAME_SERVER_CLIENT_DIR,
            ZCL_CLUSTER_MS_OCCUPANCY_SENSING, ZCL_ATTRID_OCCUPANCY,
            ZCL_DATA_TYPE_BITMAP8, &prucho2_occupancy);
    if (st == ZCL_STA_SUCCESS) {
        pending = false;
        ++diag.report_enqueued;
    } else {
        ++diag.report_retries;
    }
}

static int32_t checkCb(void *arg) {
    checkEvt = NULL;
    ++diag.timer_checks;
    prucho2_pir_task();
    return -1;
}

static void schedule_check(uint32_t delay_ms) {
    if (!checkEvt) checkEvt = TL_ZB_TIMER_SCHEDULE(checkCb, NULL, delay_ms);
}

static int32_t syncCb(void *arg) {
    /* Periodic reconciliation even when there was no new motion edge. */
    pending = true;
    send_occupancy();
    return 0;
}

void prucho2_pir_request_sync(void) {
    pending = true;
    send_occupancy();
}

void prucho2_pir_init(void) {
    raw_high = read_pir();
    prucho2_occupancy = raw_high ? 1 : 0;
    pending = true;
    last_low = clock_time();
    if (raw_high) schedule_check(PRUCHO2_PIR_RECHECK_MS);
    if (!syncEvt) syncEvt = TL_ZB_TIMER_SCHEDULE(syncCb, NULL, PRUCHO2_PIR_SYNC_MS);
}

void prucho2_pir_task(void) {
    bool current = read_pir();
    if (current != raw_high) {
        raw_high = current;
        if (current) {
            ++diag.rising_edges;
            request_occupancy(1);
        } else {
            ++diag.falling_edges;
            last_low = clock_time();
        }
    }

    if (current) {
        request_occupancy(1);
        schedule_check(PRUCHO2_PIR_RECHECK_MS);
    } else if (prucho2_occupancy) {
        uint32_t hold = prucho2_occupancy_hold;
        if (hold < 1) hold = 1;
        if (hold > 600) hold = 600;
        if (clock_time_exceed(last_low, hold * 1000000UL)) {
            request_occupancy(0);
        } else {
            schedule_check(PRUCHO2_PIR_RECHECK_MS);
        }
    }
    send_occupancy();
    if (pending) schedule_check(250);
}

void prucho2_pir_prepare_sleep(void) {
    /* Select wake level opposite to the present PIR voltage; follows
     * Slacky TS0203 GPIO-level-change strategy, not the PVVX event loop. */
    extern drv_pm_pinCfg_t g_switchPmCfg[];
    drv_pm_wakeupPinLevelChange(g_switchPmCfg, 2);
}

#endif
