#ifndef PRUCHO2_APP_PIR_H_
#define PRUCHO2_APP_PIR_H_
/*
 * Slacky-based ZG-204ZL PIR motion module, independent from lux measurement.
 * Development-only. See docs/PRUCHO2_PIR_ZG204ZL_DEVELOPMENT.md.
 */
extern uint8_t prucho2_occupancy;
extern uint16_t prucho2_occupancy_hold;
extern uint16_t prucho2_lux;
/* v0.1.42 read-only ZCL diagnostics on occupancy cluster 0x0406. */
extern uint8_t prucho2_diag_raw_gpio;
extern uint32_t prucho2_diag_rising_edges;
extern uint32_t prucho2_diag_falling_edges;
extern uint32_t prucho2_diag_reports_requested;
extern uint32_t prucho2_diag_reports_enqueued;
extern uint32_t prucho2_diag_reports_retries;
extern uint32_t prucho2_diag_timer_checks;
extern uint32_t prucho2_diag_gpio_high_samples;
extern uint32_t prucho2_diag_gpio_low_samples;
extern uint32_t prucho2_diag_sleep_returns;
extern uint32_t prucho2_diag_poll_samples;
void prucho2_pir_note_wakeup(void);
void prucho2_pir_init(void);
void prucho2_pir_task(void);
void prucho2_pir_prepare_sleep(void);
void prucho2_pir_request_sync(void);
void prucho2_lux_init(void);
void prucho2_lux_task(void);
#endif
