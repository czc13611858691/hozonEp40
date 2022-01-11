#ifndef __soft_timer_H_
#define __soft_timer_H_

#include <stdint.h>

typedef uint16_t soft_timer_base_t;
typedef void (*soft_timer_cb)(void);

soft_timer_base_t g_soft_timer_ticks;

void soft_timer_create(soft_timer_base_t period, soft_timer_cb cb);
void soft_timer_run(void);

#endif