#ifndef PRUCHO2_APP_PIR_H_
#define PRUCHO2_APP_PIR_H_
/*
 * Slacky-based ZG-204ZL PIR motion module, independent from lux measurement.
 * Development-only. See docs/PRUCHO2_PIR_ZG204ZL_DEVELOPMENT.md.
 */
extern uint8_t prucho2_occupancy;
extern uint16_t prucho2_occupancy_hold;
extern uint16_t prucho2_lux;
void prucho2_pir_init(void);
void prucho2_pir_task(void);
void prucho2_pir_prepare_sleep(void);
void prucho2_pir_request_sync(void);
void prucho2_lux_init(void);
void prucho2_lux_task(void);
#endif
