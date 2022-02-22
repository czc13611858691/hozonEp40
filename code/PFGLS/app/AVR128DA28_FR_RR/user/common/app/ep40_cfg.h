#ifndef EP40_CFG_H_
#define EP40_CFG_H_

#include <stdint.h>
#include "ep40_function_specification.h"
#include "lin_cfg.h"

#define LONG_PRESS_DELTA 300

#define ONE_TICK_UNIT_MS 20

#define BACKLIGHT_NIGHT_GAIN 50
#define BACKLIGHT_DAYTIME_GAIN 100

#define TIMEOUT_REAR_UNIT_S 60

#define WINDOW_TICK_UNIT_MS 20

extern btn_rear_t g_btn_rear;

void soft_timer_rear_btn_task(void);
void soft_timer_window_lock_task(void);
void soft_timer_lin_signal_update_task(void);
void soft_timer_window_task(void);
void ep40_lin_signal_update_while_task(void);
void lin_signal_send_x_times(l_signal_handle signal_handle, uint8_t cnt, uint8_t val);

#endif /* EP40_CFG_H_ */