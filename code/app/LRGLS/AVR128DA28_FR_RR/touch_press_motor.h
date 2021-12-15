#ifndef __touch_press_motor_H_
#define __touch_press_motor_H_

#include <stdint.h>

typedef void(*motor_shake_cb_t)(void);

typedef struct
{
    uint8_t state_handle;
    uint32_t adc_res;
    uint32_t adc_min;
    uint32_t adc_max;
    uint8_t delta_D1;
    uint8_t delta_D2;
    uint8_t delta_D3;
    uint8_t capsense_status;
    motor_shake_cb_t motor_shake;
} state_obj_t;

extern state_obj_t g_btn_a_state;
extern state_obj_t g_btn_b_state;
extern state_obj_t g_btn_self_state;

void state_mahine_run(state_obj_t *state_obj_ptr);

#endif