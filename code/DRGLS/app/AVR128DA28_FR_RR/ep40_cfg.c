/**
 * @file ep40_cfg.c
 * @author 曹志成
 * @brief ep40功能规范中所有和硬件相关的配置放在这里的文件
 * @date 2021-10-20
 */
#include "ep40_cfg.h"
#include "led.h"
#include <atmel_start.h>
#include "touch.h"
#include "ep40_function_specification.h"
#include "lin.h"
#include "lin_cfg.h"

/* REAR按键回调函数注册 */
void rear_led_ctrl(uint8_t status);
btn_rear_t g_btn_rear = {
    .rear_led_ctrl_cb = rear_led_ctrl,
};

/* REAR触摸检测定时器任务 */
void soft_timer_rear_btn_task(void)
{
    static uint8_t btn_status_last = 0;
    static uint8_t btn_status = 0;
    uint8_t all_cap_status = 0;
    btn_status_last = btn_status;
    btn_status = get_sensor_state(0) & 0x80;

    g_btn_rear.press_trig_flg = 1;

    if ((btn_status != 0) && (btn_status_last == 0))
    {
        g_btn_rear.cap_trig_flg = 1;
    }

    for (uint8_t i = 0; i < 6; i++)
    {
        if ((get_sensor_state(i) & 0x80) != 0)
        {
            all_cap_status = 1;
        }
    }
    for (uint8_t i = 0; i < 2; i++)
    {
        if (get_scroller_state(i) != 0)
        {
            all_cap_status = 1;
        }
    }

    g_btn_rear.all_cap_status = all_cap_status;

    btn_rear_tick_task();
}

void rear_led_ctrl(uint8_t status)
{
    if (status == 1)
    {
        led_set_level(led_d5, 1);
        led_set_level(led_d6, 0);
    }
    else
    {
        led_set_level(led_d5, 0);
        led_set_level(led_d6, 1);
    }
}

/* 窗锁止led控制回调函数注册和信号输出控制 */
void window_lock_led_cb(uint8_t status);
void window_lock_signal_cb(void);

window_lock_t g_window_lock = {
    .led_cb = NULL,
    .signal_cb = window_lock_signal_cb,
};

void window_lock_led_cb(uint8_t status)
{
    if (status == 1)
    {
        led_set_level(led_d7, 1);
        led_set_level(led_d8, 0);
    }
    else
    {
        led_set_level(led_d7, 0);
        led_set_level(led_d8, 1);
    }
}

/* 窗锁止定时器按键触摸检测任务 */
void soft_timer_window_lock_task(void)
{
    static uint8_t btn_status_last = 0;
    static uint8_t btn_status = 0;
    btn_status_last = btn_status;
    btn_status = get_sensor_state(2) & 0x80;

    g_window_lock.press_flg = 1;

    if ((btn_status != 0) && (btn_status_last == 0))
    {
        g_window_lock.capsense_flg = 1;
		lin_signal_send_x_times(LI0_PFGLS_ButtonSoundRequest, 1, 1);
    }

    window_lock_task();
}

void window_lock_signal_cb(void)
{
    lin_signal_send_x_times(LI0_PFGLS_CustomSignal, 3, 1);
}

PFGLS_signal_t g_PFGLS_signal;

LIN_RX_signal_t g_lin_rx_signal = {
    .backlight_brightness = 3,
};
/* lin信号更新到变量定时器任务 */
void soft_timer_lin_signal_update_task(void)
{
    if (l_u8_rd_LI0_PDCU_BDCS1_Backlight_brightness_fb() != 0)
    {
        g_lin_rx_signal.backlight_brightness = l_u8_rd_LI0_PDCU_BDCS1_Backlight_brightness_fb();
    }
    g_lin_rx_signal.backlight_status = l_bool_rd_LI0_BDCS1_BacklightStatus();
}

/* 背光亮度控制回调函数注册 */
void backlight_cb(uint32_t duty);
backlight_cb_t g_backlight_cb = backlight_cb;
void backlight_cb(uint32_t duty)
{
    TCA0.SINGLE.CMP2 = duty;
}

/* 窗提升滑条和按键定时器任务 */
void left_window_signal_out_cb(uint8_t signal);
void right_window_signal_out_cb(uint8_t signal);
typedef struct
{
    uint8_t status;
    uint16_t ticks;
    uint8_t btn_a_status_last;
    uint8_t btn_a_status;
    uint8_t btn_b_status_last;
    uint8_t btn_b_status;
    uint8_t slide_status_last;
    uint8_t slide_status;
    uint8_t slide_pos_first;
    uint8_t slide_pos;
    window_t window;
} window_obj_t;

window_obj_t g_left_window_obj = {
    .window = {
        .cb = left_window_signal_out_cb,
    },
};

window_obj_t g_right_window_obj = {
    .window = {
        .cb = right_window_signal_out_cb,
    },
};

typedef struct
{
    uint8_t cnt;
    uint8_t val;
    uint8_t satus;
} lin_signal_cnt_t;

lin_signal_cnt_t g_lin_signal_cnt[LIN_NUM_OF_SIGS];

void ep40_lin_signal_update_while_task(void)
{
    for (uint8_t i = 0; i < LIN_NUM_OF_SIGS; i++)
    {
        switch (i)
        {
        case LI0_PFGLS_RLWindowControl:
            if (g_lin_signal_cnt[i].cnt > 0)
            {
                g_lin_signal_cnt[i].cnt--;
                l_u8_wr_LI0_PFGLS_RLWindowControl(g_lin_signal_cnt[i].val);
            }
            else
            {
                l_u8_wr_LI0_PFGLS_RLWindowControl(0);
            }

            break;
        case LI0_PFGLS_CustomSignal:
            if (g_lin_signal_cnt[i].cnt > 0)
            {
                g_lin_signal_cnt[i].cnt--;
                l_bool_wr_LI0_PFGLS_CustomSignal(g_lin_signal_cnt[i].val);
            }
            else
            {
                l_bool_wr_LI0_PFGLS_CustomSignal(0);
            }

            break;
        case LI0_PFGLS_ButtonSoundRequest:
            if (g_lin_signal_cnt[i].cnt > 0)
            {
                l_bool_wr_LI0_PFGLS_ButtonSoundRequest(g_lin_signal_cnt[i].val);

                g_lin_signal_cnt[i].cnt--;
            }
            else
            {
                l_bool_wr_LI0_PFGLS_ButtonSoundRequest(0);
            }

            break;
        case LI0_PFGLS_ErrorSts:
            if (g_lin_signal_cnt[i].cnt > 0)
            {
                g_lin_signal_cnt[i].cnt--;
                l_bool_wr_LI0_PFGLS_ErrorSts(g_lin_signal_cnt[i].val);
            }
            else
            {
                l_bool_wr_LI0_PFGLS_ErrorSts(0);
            }
            break;
        case LI0_PFGLS_Voltage_errorCode:
            if (g_lin_signal_cnt[i].cnt > 0)
            {
                g_lin_signal_cnt[i].cnt--;
                l_bool_wr_LI0_PFGLS_Voltage_errorCode(g_lin_signal_cnt[i].val);
            }
            else
            {
                l_bool_wr_LI0_PFGLS_Voltage_errorCode(0);
            }
            break;
        case LI0_PFGLS__Capsense_errorCode:
            if (g_lin_signal_cnt[i].cnt > 0)
            {
                g_lin_signal_cnt[i].cnt--;
                l_bool_wr_LI0_PFGLS__Capsense_errorCode(g_lin_signal_cnt[i].val);
            }
            else
            {
                l_bool_wr_LI0_PFGLS__Capsense_errorCode(0);
            }
            break;
        case LI0_PFGLS__Motor_errorCode:
            if (g_lin_signal_cnt[i].cnt > 0)
            {
                g_lin_signal_cnt[i].cnt--;
                l_bool_wr_LI0_PFGLS__Motor_errorCode(g_lin_signal_cnt[i].val);
            }
            else
            {
                l_bool_wr_LI0_PFGLS__Motor_errorCode(0);
            }
            break;

        default:
            break;
        }
    }
}

void lin_signal_send_x_times(l_signal_handle signal_handle, uint8_t cnt, uint8_t val)
{
    g_lin_signal_cnt[signal_handle].cnt = cnt;
    g_lin_signal_cnt[signal_handle].val = val;
}

void window_obj_var_clear(window_obj_t *handle)
{
    handle->status = 0;
    handle->ticks = 0;
    handle->btn_a_status_last = 0;
    handle->btn_a_status = 0;
    handle->btn_b_status_last = 0;
    handle->btn_b_status = 0;
    handle->slide_status_last = 0;
    handle->slide_status = 0;
    handle->slide_pos_first = 0;
    handle->slide_pos = 0;
    handle->window.status = 0;
    handle->window.long_flg = 0;
    handle->window.btn_b_cap_trig_flg = 0;
    handle->window.btn_a_cap_trig_flg = 0;
    handle->window.slide_1_to_2_flg = 0;
    handle->window.slide_2_to_1_flg = 0;
    handle->window.btn_a_status = 0;
    handle->window.btn_b_status = 0;
    handle->window.slide_status = 0;
    handle->window.signal = 0;
}

/* 左边窗提升信号输出回调函数 */
void left_window_signal_out_cb(uint8_t signal)
{
    // uint8_t rear_status = g_btn_rear.status;

    // if (rear_status == 0)
    // {
    //     lin_signal_x_shot_wr(PFGLS_0X21_FL_WINDOW_CONTROL, signal, 3);
    // }
    // else
    // {
    //     lin_signal_x_shot_wr(PFGLS_0X21_RL_WINDOW_CONTROL, signal, 3);
    // }
}

/* 右边窗提升信号输出回调函数 */
void right_window_signal_out_cb(uint8_t signal)
{
    lin_signal_send_x_times(LI0_PFGLS_RLWindowControl, 3, signal);
}

/* 窗提升滑条呵呵按键软件定时器任务 */
void soft_timer_window_task(void)
{
    window_obj_t *window_obj_ptr = &g_left_window_obj;
    window_t *window_ptr = &g_left_window_obj.window;
    //static uint8_t rear_status_last = 0;
    //static uint8_t rear_status = 0;

    //rear_status_last = rear_status;
    //rear_status = g_btn_rear.status;

    /*************************************LEFT************************************************/

    /*************************************RIGHT************************************************/
    window_obj_ptr = &g_right_window_obj;
    window_ptr = &g_right_window_obj.window;

    window_obj_ptr->btn_a_status_last = window_obj_ptr->btn_a_status;
    window_obj_ptr->btn_a_status = get_sensor_state(0) & 0x80;

    window_obj_ptr->btn_b_status_last = window_obj_ptr->btn_b_status;
    window_obj_ptr->btn_b_status = get_sensor_state(1) & 0x80;

    window_ptr->btn_a_status = window_obj_ptr->btn_a_status;
    window_ptr->btn_b_status = window_obj_ptr->btn_b_status;

    if ((window_obj_ptr->btn_a_status != 0) && (window_obj_ptr->btn_a_status_last == 0))
    {
        window_obj_ptr->ticks = 0;
        window_obj_ptr->status = 2;
		lin_signal_send_x_times(LI0_PFGLS_ButtonSoundRequest, 1, 1);
    }

    if (window_obj_ptr->status == 2)
    {
        window_obj_ptr->ticks++;
        window_ptr->long_flg = 1;
        window_ptr->btn_a_cap_trig_flg = 1;
        if (window_obj_ptr->ticks >= (LONG_PRESS_DELTA / WINDOW_TICK_UNIT_MS))
        {
            window_obj_ptr->status = 0;
            window_ptr->long_flg = 1;
            window_ptr->btn_a_cap_trig_flg = 1;
        }
        if (window_obj_ptr->btn_a_status == 0)
        {
            window_obj_ptr->status = 0;
            window_ptr->long_flg = 0;
            window_ptr->btn_a_cap_trig_flg = 1;
        }
    }

    if ((window_obj_ptr->btn_b_status != 0) && (window_obj_ptr->btn_b_status_last == 0))
    {
        window_obj_ptr->ticks = 0;
        window_obj_ptr->status = 3;
		lin_signal_send_x_times(LI0_PFGLS_ButtonSoundRequest, 1, 1);
    }

    if (window_obj_ptr->status == 3)
    {
        window_obj_ptr->ticks++;
        window_ptr->long_flg = 1;
        window_ptr->btn_b_cap_trig_flg = 1;
        if (window_obj_ptr->ticks >= (LONG_PRESS_DELTA / WINDOW_TICK_UNIT_MS))
        {
            window_obj_ptr->status = 0;
            window_ptr->long_flg = 1;
            window_ptr->btn_b_cap_trig_flg = 1;
        }
        if (window_obj_ptr->btn_b_status == 0)
        {
            window_obj_ptr->status = 0;
            window_ptr->long_flg = 0;
            window_ptr->btn_b_cap_trig_flg = 1;
        }
    }

    window_obj_ptr->slide_status_last = window_obj_ptr->slide_status;
    window_obj_ptr->slide_status = get_scroller_state(0);
    window_ptr->slide_status = window_obj_ptr->slide_status;

    window_obj_ptr->slide_pos = get_scroller_position(0);

    if (window_obj_ptr->status == 0)
    {
        if ((window_obj_ptr->slide_status != 0) && (window_obj_ptr->slide_status_last == 0))
        {
            window_obj_ptr->ticks = 0;
            window_obj_ptr->status = 4;
            window_obj_ptr->slide_pos_first = window_obj_ptr->slide_pos;
        }
    }
    else if (window_obj_ptr->status == 4)
    {
        if (window_obj_ptr->slide_status == 0)
        {
            window_obj_ptr->status = 0;
            if (window_obj_ptr->slide_pos_first < 100)
            {
                if (window_obj_ptr->slide_pos > 150)
                {
                    window_ptr->long_flg = 0;
                    window_ptr->slide_1_to_2_flg = 1;
                }
            }
            else if (window_obj_ptr->slide_pos_first > 150)
            {
                if (window_obj_ptr->slide_pos < 100)
                {
                    window_ptr->long_flg = 0;
                    window_ptr->slide_2_to_1_flg = 1;
                }
            }
        }
        else
        {
            window_obj_ptr->ticks++;
            if (window_obj_ptr->slide_status != 0)
            {
                if (window_obj_ptr->slide_pos_first < 100)
                {
                    if (window_obj_ptr->slide_pos > 150)
                    {
                        if (window_obj_ptr->ticks >= (LONG_PRESS_DELTA / WINDOW_TICK_UNIT_MS))
                        {
                            window_ptr->slide_1_to_2_flg = 1;
                            window_obj_ptr->status = 5;
                            window_ptr->long_flg = 1;
                        }
                        else
                        {
                            window_ptr->slide_1_to_2_flg = 1;
                            window_ptr->long_flg = 1;
                        }
                    }
                }
                else if (window_obj_ptr->slide_pos_first > 150)
                {
                    if (window_obj_ptr->slide_pos < 100)
                    {
                        if (window_obj_ptr->ticks >= (LONG_PRESS_DELTA / WINDOW_TICK_UNIT_MS))
                        {
                            window_obj_ptr->status = 5;
                            window_ptr->long_flg = 1;
                            window_ptr->slide_2_to_1_flg = 1;
                        }
                        else
                        {
                            window_ptr->long_flg = 1;
                            window_ptr->slide_2_to_1_flg = 1;
                        }
                    }
                }
            }
        }
    }
    else if (window_obj_ptr->status == 5)
    {
        if (window_obj_ptr->slide_status == 0)
        {
            window_obj_ptr->status = 0;
        }
    }
    window_task(window_ptr);
}
