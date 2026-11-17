#ifndef PRUCHO2_APP_PIR_H_
#define PRUCHO2_APP_PIR_H_
/* ZG-204ZL PIR + independent lux; Slacky platform. */
extern uint8_t prucho2_occupancy;
extern uint16_t prucho2_occupancy_hold;
extern uint16_t prucho2_lux;
void prucho2_pir_init(void);
void prucho2_pir_task(void);
void prucho2_pir_prepare_sleep(void);
bool prucho2_pir_idle(void);
void prucho2_pir_request_sync(void);
void prucho2_lux_init(void);
void prucho2_lux_task(void);
#endif
