#include "touch_press_motor_cfg.h"
#include "touch_press_motor.h"
#include "touch.h"
#include <stdio.h>

#define PRESS_DELTA_D1 2
#define PRESS_DELTA_D2 2
#define PRESS_DELTA_D3 2

extern uint8_t sin_data_out_flg;

void motor_shake_cb(void)
{
    //sin_data_out_flg = 1;
}

extern int adc_res[2];

uint8_t g_touch_press_motor_init_flg = 0;

void soft_timer_touch_press_motor_task(void)
{
    uint8_t btn_a_status = 0;
    uint8_t btn_b_status = 0;
    uint8_t btn_self_status = 0;

    if (g_touch_press_motor_init_flg == 0)
    {
        g_touch_press_motor_init_flg = 1;

        g_btn_a_state.motor_shake = motor_shake_cb;
        g_btn_a_state.adc_res = adc_res[1];
        g_btn_a_state.adc_min = 0xffffffff;
        g_btn_a_state.adc_max = 0;
        g_btn_a_state.delta_D1 = PRESS_DELTA_D1;
        g_btn_a_state.delta_D2 = PRESS_DELTA_D2;
        g_btn_a_state.delta_D3 = PRESS_DELTA_D3;

        g_btn_b_state.motor_shake = motor_shake_cb;
        g_btn_b_state.adc_res = adc_res[0];
        g_btn_b_state.adc_min = 0xffffffff;
        g_btn_b_state.adc_max = 0;
        g_btn_b_state.delta_D1 = 4;
        g_btn_b_state.delta_D2 = 4;
        g_btn_b_state.delta_D3 = 4;

        g_btn_self_state.motor_shake = motor_shake_cb;
        g_btn_self_state.adc_res = adc_res[0];
        g_btn_self_state.adc_min = 0xffffffff;
        g_btn_self_state.adc_max = 0;
        g_btn_self_state.delta_D1 = PRESS_DELTA_D1;
        g_btn_self_state.delta_D2 = PRESS_DELTA_D2;
        g_btn_self_state.delta_D3 = PRESS_DELTA_D3;
    }
    else
    {
        g_btn_a_state.adc_res = adc_res[1];

        btn_a_status = (get_sensor_state(0) & 0x80) >> 7;

        g_btn_a_state.capsense_status = btn_a_status;

        state_mahine_run(&g_btn_a_state);

        float filt_val = 0.7;
        g_btn_b_state.adc_res = adc_res[0];

        btn_b_status = (get_sensor_state(1) & 0x80) >> 7;

        g_btn_b_state.capsense_status = btn_b_status;

        state_mahine_run(&g_btn_b_state);

        g_btn_self_state.adc_res = g_btn_self_state.adc_res * filt_val / 10 + adc_res[0] * (10 - filt_val) / 10;

        btn_self_status = (get_sensor_state(2) & 0x80) >> 7;

        g_btn_self_state.capsense_status = btn_self_status;

        state_mahine_run(&g_btn_self_state);

        // int res1 = g_btn_b_state.adc_res;
        // int res2 = g_btn_b_state.adc_min;

        //         int res3 = g_btn_a_state.adc_res;
        //         int res4 = g_btn_a_state.adc_min;

        // printf(":%d,%d,%d,%d\r\n", res1, res2,res3,res4);
    }
}