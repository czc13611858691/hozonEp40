#ifndef __ep40_function_specification_H_
#define __ep40_function_specification_H_

#include <stdint.h>
#include "stddef.h"

#define SIGNAL_VAL_LEVEL_1 0x01
#define SIGNAL_VAL_LEVEL_2 0x02
#define SIGNAL_VAL_LEVEL_3 0x03
#define SIGNAL_VAL_LEVEL_4 0x04
#define SIGNAL_VAL_LEVEL_5 0x05
#define SIGNAL_VAL_LEVEL_6 0x06
#define SIGNAL_VAL_LEVEL_7 0x07
#define SIGNAL_VAL_LEVEL_8 0x08

#define SIGNAL_VAL_INACTIVE 0x00
#define SIGNAL_VAL_ACTIVE 0x01

#define SIGNAL_VAL_NORMAL 0x00
#define SIGNAL_VAL_FAULT 0x01

#define SIGNAL_VAL_NO_REQUEST 0x00
#define SIGNAL_VAL_MANUAL_UP 0x01
#define SIGNAL_VAL_MANUAL_DOWN 0x02
#define SIGNAL_VAL_AUTO_UP 0x03
#define SIGNAL_VAL_AUTO_DOWN 0x04

#define SIGNAL_VAL_NO_ERR 0x00
#define SIGNAL_VAL_ERR 0x01

#define SIGNAL_VAL_NO_PRESS 0x00
#define SIGNAL_VAL_PRESS 0x01

#define SIGNAL_VAL_SLEEP 0x00
#define SIGNAL_VAL_WAKE_UP 0x01



typedef struct
{
    char *name;
    uint8_t id;
    uint8_t pid;
} DFGLS_frame_t;

typedef struct
{
    uint8_t backlight_brightness : 4;
    uint8_t ctrl_window_lock : 1;
    uint8_t backlight_status : 1;
} DFGLS_signal_t;

typedef void (*rear_led_ctrl_t)(uint8_t status);

typedef struct
{
    uint8_t status;
    uint8_t cap_trig_flg;
    uint8_t all_cap_status;
    uint8_t press_trig_flg;
    uint32_t ticks;
    rear_led_ctrl_t rear_led_ctrl_cb;
} btn_rear_t;

typedef void (*window_signal_out_cb)(uint8_t signal);

typedef struct
{
    uint8_t status;
    uint8_t long_flg;
    uint8_t btn_b_cap_trig_flg;
    uint8_t btn_a_cap_trig_flg;
    uint8_t slide_1_to_2_flg;
    uint8_t slide_2_to_1_flg;
    uint8_t btn_a_status;
    uint8_t btn_b_status;
    uint8_t slide_status;
    uint8_t signal;
    window_signal_out_cb cb;
} window_t;

typedef void (*window_lock_cb_t)(void);
typedef void (*window_lock_led_cb_t)(uint8_t status);

typedef struct
{
    uint8_t capsense_flg;
    uint8_t press_flg;
    window_lock_led_cb_t led_cb;
    window_lock_cb_t signal_cb;
} window_lock_t;

typedef void (*backlight_cb_t)(uint32_t duty);


extern DFGLS_signal_t g_DFGLS_signal;
extern btn_rear_t g_btn_rear;
extern window_lock_t g_window_lock;
extern backlight_cb_t g_backlight_cb;

void btn_rear_tick_task(void);
void window_task(window_t *window_ptr);
void window_lock_task(void);
void backlight_task(void);
void lin_go_to_sleep(void);

#endif