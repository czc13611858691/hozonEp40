#include "target.h"
#include "soft_timer.h"
#include "led.h"
#include "ep40_cfg.h"
#include <usart_basic.h>
#include "touch_press_motor.h"
#include "touch.h"
#include <stdio.h>

uint8_t g_hc595_buffer[1] = {0xfe};
led_Dx_t g_led_Dx[LED_NUM] = {
	{0, 0},
	{0, 1},
	{0, 2},
	{0, 3},
	{0, 4},
	{0, 5},
	{0, 6},
	{0, 7},
};
state_obj_t g_btn_a_state;
state_obj_t g_btn_b_state;
state_obj_t g_btn_self_state;
int adc_res[2];
uint8_t g_touch_press_motor_init_flg = 0;

extern uint8_t sin_data_out_flg;

void target_soft_timer_init(void)
{
	soft_timer_create(100, led_update_buf_task);
	soft_timer_create(20, soft_timer_window_lock_task);
	soft_timer_create(20, soft_timer_window_task);
	soft_timer_create(10, soft_timer_lin_signal_update_task);
	soft_timer_create(100, backlight_task);
}

size_t USART0_Read(uint8_t * rDATA)
{
	*rDATA = LIN_USART_X.RXDATAL;
	return 0;
}

size_t USART0_Send(uint8_t * tDATA)
{
	LIN_USART_X.TXDATAL = *tDATA;
	return 0;
}

void motor_shake_cb(void)
{
    sin_data_out_flg = 1;
}

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
    }
}

void __attribute__((optimize("O0"))) lin_go_to_sleep(void)
{
    LIN_EN_set_level(0);

    Disable_global_interrupt();
    LIN_USART_X.CTRLA &= ~(1 << USART_ABEIE_bp | 1 << USART_RXCIE_bp);
    LIN_USART_X.CTRLB &= ~(1 << USART_TXEN_bp | USART_RXMODE_LINAUTO_gc | 1 << USART_RXEN_bp);

    LIN_TX_SET_DIR(PORT_DIR_OUT);
    LIN_TX_SET_LEVEL(false);
}
