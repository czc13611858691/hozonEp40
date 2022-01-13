/*
 * ep40_cfg.h
 *
 * Created: 2021/10/19 17:51:06
 *  Author: Sunny
 */ 


#ifndef EP40_CFG_H_
#define EP40_CFG_H_

#include <stdint.h>
#include "ep40_function_specification.h"
#include "lin_cfg.h"

/* 长按短按区分时间 */
#define LONG_PRESS_DELTA 300

/* REAR时基配置 */
#define ONE_TICK_UNIT_MS 20

/* 硬件pwm占空比最大值配置 */
#define DUTY_MAX 0x4a

/* 背光亮度白天和夜晚的亮度区别 */
#define BACKLIGHT_NIGHT_GAIN 50
#define BACKLIGHT_DAYTIME_GAIN 100

/* 功能规范中REAR电容不触发时间，60*1000=60000ms */
#define TIMEOUT_REAR_UNIT_S 60

/* WINDOW时基配置 */
#define WINDOW_TICK_UNIT_MS 20

typedef struct
{
    uint8_t backlight_brightness : 4;
    uint8_t ctrl_window_lock : 1;
    uint8_t backlight_status : 1;
} LIN_RX_signal_t;

extern btn_rear_t g_btn_rear;
extern LIN_RX_signal_t g_lin_rx_signal;

void soft_timer_rear_btn_task(void);
void soft_timer_window_lock_task(void);
void soft_timer_lin_signal_update_task(void);
void soft_timer_window_task(void);
void ep40_lin_signal_update_while_task(void);
void lin_signal_send_x_times(l_signal_handle signal_handle, uint8_t cnt, uint8_t val);

#endif /* EP40_CFG_H_ */