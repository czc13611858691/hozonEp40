#ifndef __ep40_function_specification_H_
#define __ep40_function_specification_H_

#include <stdint.h>
#include "stddef.h"

//off 0x7e
#define L_HEAT_LV1 0x7c
#define L_HEAT_LV2 0x78
#define L_HEAT_LV3 0x70

#define L_WIND_LV1 0x3e
#define L_WIND_LV2 0x1e
#define L_WIND_LV3 0x0e


#define R_HEAT_LV1 0x7c
#define R_HEAT_LV2 0x78
#define R_HEAT_LV3 0x70

#define R_WIND_LV1 0x3e
#define R_WIND_LV2 0x1e
#define R_WIND_LV3 0x0e

// #define SIGNAL_VAL_LEVEL_1 0x01
// #define SIGNAL_VAL_LEVEL_2 0x02
// #define SIGNAL_VAL_LEVEL_3 0x03
// #define SIGNAL_VAL_LEVEL_4 0x04
// #define SIGNAL_VAL_LEVEL_5 0x05
// #define SIGNAL_VAL_LEVEL_6 0x06
// #define SIGNAL_VAL_LEVEL_7 0x07
// #define SIGNAL_VAL_LEVEL_8 0x08

// #define SIGNAL_VAL_INACTIVE 0x00
// #define SIGNAL_VAL_ACTIVE 0x01

// #define SIGNAL_VAL_NORMAL 0x00
// #define SIGNAL_VAL_FAULT 0x01

// #define SIGNAL_VAL_NO_REQUEST 0x00
// #define SIGNAL_VAL_MANUAL_UP 0x01
// #define SIGNAL_VAL_MANUAL_DOWN 0x02
// #define SIGNAL_VAL_AUTO_UP 0x03
// #define SIGNAL_VAL_AUTO_DOWN 0x04

// #define SIGNAL_VAL_NO_ERR 0x00
// #define SIGNAL_VAL_ERR 0x01

// #define SIGNAL_VAL_NO_PRESS 0x00
// #define SIGNAL_VAL_PRESS 0x01

// #define SIGNAL_VAL_SLEEP 0x00
// #define SIGNAL_VAL_WAKE_UP 0x01

// typedef struct
// {
//     uint8_t backlight_brightness : 4;
//     uint8_t ctrl_window_lock : 1;
//     uint8_t backlight_status : 1;
// } LIN_RX_signal_t;

// typedef struct
// {
//     char *name;
//     uint8_t id;
//     uint8_t pid;
// } PFGLS_frame_t;

// typedef struct
// {
//     uint8_t backlight_brightness : 4;
//     uint8_t ctrl_window_lock : 1;
//     uint8_t backlight_status : 1;
// } PFGLS_signal_t;

typedef struct
{
    uint8_t status;
    uint8_t cap_trig_flg;
    uint8_t all_cap_status;
    uint8_t press_trig_flg;
    uint32_t ticks;
} btn_normal_t;

void btn_process_task();
void ep40_lin_signal_update_while_task(void);
void ep40_integrate_all_signal(void);
void backlight_task(void);
void lin_go_to_sleep(void);

#endif