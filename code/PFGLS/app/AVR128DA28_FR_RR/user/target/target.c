#include "target.h"
#include "soft_timer.h"
#include "led.h"
#include "ep40_cfg.h"

void target_soft_timer_init(void)
{
	soft_timer_create(100, led_update_buf_task);
	soft_timer_create(20, soft_timer_window_lock_task);
	soft_timer_create(20, soft_timer_window_task);
	soft_timer_create(10, soft_timer_lin_signal_update_task);
	soft_timer_create(100, backlight_task);
}