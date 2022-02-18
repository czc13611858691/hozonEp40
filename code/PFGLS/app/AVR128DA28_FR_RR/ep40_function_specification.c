#include "ep40_function_specification.h"
#include "ep40_cfg.h"
#include "atmel_start_pins.h"
// PFGLS 主开关

/* V01-3.1.5.1 */
/* 反转按键任务，需要放在定时器中，TIMEOUT_REAR_UNIT_S配置定时器任务的时间 */
/* 外部输入:cap_trig_flg\press_trig_flg */
/* 判断电容触发标志，压力触发标志 */
/* 输出:反转LED控制回调函数 */
void btn_rear_tick_task(void)
{
    btn_rear_t *btn_rear_ptr = &g_btn_rear;
    switch (btn_rear_ptr->status)
    {
    case 0:
        if (btn_rear_ptr->cap_trig_flg == 1)
        {
            btn_rear_ptr->cap_trig_flg = 0;
            if (btn_rear_ptr->press_trig_flg == 1)
            {
                btn_rear_ptr->press_trig_flg = 0;

                btn_rear_ptr->status = 1;
                btn_rear_ptr->ticks = 0;
                if (btn_rear_ptr->rear_led_ctrl_cb != NULL)
                {
                    btn_rear_ptr->rear_led_ctrl_cb(1);
                }
            }
        }
        break;
    case 1:
        if (btn_rear_ptr->all_cap_status == 1)
        {
            btn_rear_ptr->ticks = 0;
        }
        else
        {
            btn_rear_ptr->ticks++;
        }
        if (btn_rear_ptr->ticks >= ((uint32_t)TIMEOUT_REAR_UNIT_S * 1000 / ONE_TICK_UNIT_MS))
        {
            btn_rear_ptr->ticks = 0;

            btn_rear_ptr->status = 0;
            if (btn_rear_ptr->rear_led_ctrl_cb != NULL)
            {
                btn_rear_ptr->rear_led_ctrl_cb(0);
            }
        }
        if (btn_rear_ptr->cap_trig_flg == 1)
        {
            btn_rear_ptr->cap_trig_flg = 0;
            if (btn_rear_ptr->press_trig_flg == 1)
            {
                btn_rear_ptr->press_trig_flg = 0;

                btn_rear_ptr->status = 0;
                if (btn_rear_ptr->rear_led_ctrl_cb != NULL)
                {
                    btn_rear_ptr->rear_led_ctrl_cb(0);
                }
            }
        }
        break;

    default:
        break;
    }
}

void window_task(window_t *window_ptr)
{
    switch (window_ptr->status)
    {
    case 0:
    {
        window_ptr->signal = SIGNAL_VAL_NO_REQUEST;
        if (window_ptr->btn_a_cap_trig_flg == 1)
        {
            window_ptr->btn_a_cap_trig_flg = 0;

            if (window_ptr->long_flg == 1)
            {
                window_ptr->signal = SIGNAL_VAL_MANUAL_UP;
                if (window_ptr->cb != NULL)
                {
                    window_ptr->cb(window_ptr->signal);
                }
                window_ptr->status = 1;
            }
            else
            {
                window_ptr->signal = SIGNAL_VAL_AUTO_UP;
                if (window_ptr->cb != NULL)
                {
                    window_ptr->cb(window_ptr->signal);
                }
            }
        }
        else if (window_ptr->btn_b_cap_trig_flg == 1)
        {
            window_ptr->btn_b_cap_trig_flg = 0;

            if (window_ptr->long_flg == 1)
            {
                window_ptr->signal = SIGNAL_VAL_MANUAL_DOWN;
                if (window_ptr->cb != NULL)
                {
                    window_ptr->cb(window_ptr->signal);
                }
                window_ptr->status = 1;
            }
            else
            {
                window_ptr->signal = SIGNAL_VAL_AUTO_DOWN;
                if (window_ptr->cb != NULL)
                {
                    window_ptr->cb(window_ptr->signal);
                }
            }
        }
        else if (window_ptr->slide_1_to_2_flg == 1)
        {
            window_ptr->slide_1_to_2_flg = 0;
            if (window_ptr->long_flg == 1)
            {
                window_ptr->signal = SIGNAL_VAL_MANUAL_UP;
                if (window_ptr->cb != NULL)
                {
                    window_ptr->cb(window_ptr->signal);
                }
                window_ptr->status = 2;
            }
            else
            {
                window_ptr->signal = SIGNAL_VAL_AUTO_UP;
                if (window_ptr->cb != NULL)
                {
                    window_ptr->cb(window_ptr->signal);
                }
            }
        }
        else if (window_ptr->slide_2_to_1_flg == 1)
        {
            window_ptr->slide_2_to_1_flg = 0;
            if (window_ptr->long_flg == 1)
            {
                window_ptr->signal = SIGNAL_VAL_MANUAL_DOWN;
                if (window_ptr->cb != NULL)
                {
                    window_ptr->cb(window_ptr->signal);
                }
                window_ptr->status = 2;
            }
            else
            {
                window_ptr->signal = SIGNAL_VAL_AUTO_DOWN;
                if (window_ptr->cb != NULL)
                {
                    window_ptr->cb(window_ptr->signal);
                }
            }
        }
    }
    break;
    case 1:
    {
        if (window_ptr->signal == SIGNAL_VAL_MANUAL_DOWN)
        {
            if (window_ptr->btn_b_status != 0)
            {
                if (window_ptr->cb != NULL)
                {
                    window_ptr->cb(window_ptr->signal);
                }
            }
            else
            {
                if (window_ptr->cb != NULL)
                {
                    window_ptr->cb(window_ptr->signal);
                }
                window_ptr->status = 0;
            }
        }
        else if (window_ptr->signal == SIGNAL_VAL_MANUAL_UP)
        {
            if (window_ptr->btn_a_status != 0)
            {
                if (window_ptr->cb != NULL)
                {
                    window_ptr->cb(window_ptr->signal);
                }
            }
            else
            {
                if (window_ptr->cb != NULL)
                {
                    window_ptr->cb(window_ptr->signal);
                }
                window_ptr->status = 0;
            }
        }
    }
    break;
    case 2:
    {
        if (window_ptr->slide_status != 0)
        {
            if (window_ptr->cb != NULL)
            {
                window_ptr->cb(window_ptr->signal);
            }
        }
        else
        {
            if (window_ptr->cb != NULL)
            {
                window_ptr->cb(window_ptr->signal);
            }
            window_ptr->status = 0;
        }
    }
    break;
    default:
        break;
    }
}

/* V01-3.1.5.6 */
void window_lock_task(void)
{
    window_lock_t *window_lock_ptr = &g_window_lock;
    LIN_RX_signal_t *signal_ptr = &g_lin_rx_signal;

    if (window_lock_ptr->capsense_flg == 1)
    {
        window_lock_ptr->capsense_flg = 0;

        if (window_lock_ptr->press_flg == 1)
        {
            window_lock_ptr->press_flg = 0;

            if (window_lock_ptr->signal_cb != NULL)
            {
                window_lock_ptr->signal_cb();
            }
        }
    }

    if (window_lock_ptr->led_cb != NULL)
    {
        window_lock_ptr->led_cb(signal_ptr->ctrl_window_lock);
    }
}

const uint32_t g_level[10] = {
    20,
    28,
    37,
    46,
    55,
    64,
    73,
    82,
    91,
    100,
};

/**
 * @brief 背光亮度控制任务
 * 
 * 输入:LIN信号更新到变量
 * 输出:背光亮度控制回调函数
 * 
 */
void backlight_task(void)
{
    LIN_RX_signal_t *signal_ptr = &g_lin_rx_signal;

    uint32_t tmp = 0;
    uint32_t night_gain = 0;
    uint32_t pwm_index = 0;

    if (signal_ptr->backlight_status == SIGNAL_VAL_ACTIVE)
    {
        night_gain = BACKLIGHT_NIGHT_GAIN;
        pwm_index = signal_ptr->backlight_brightness;
    }
    else
    {
        night_gain = BACKLIGHT_DAYTIME_GAIN;
        pwm_index = 9;
    }

    tmp = g_level[pwm_index] * night_gain * DUTY_MAX / 10000;

    if (g_backlight_cb != NULL)
    {
        g_backlight_cb(tmp);
    }
}

void __attribute__((optimize("O0"))) lin_go_to_sleep(void)
{
    /* 使能脚设置低电平 */
    LIN_EN_set_level(0);

    /* 关闭usart模块 */
    Disable_global_interrupt();
    USART0.CTRLA &= ~(1 << USART_ABEIE_bp | 1 << USART_RXCIE_bp);
    USART0.CTRLB &= ~(1 << USART_TXEN_bp | USART_RXMODE_LINAUTO_gc | 1 << USART_RXEN_bp);

    /* LIN TX设置低电平 */
    PA0_set_dir(PORT_DIR_OUT);
    PA0_set_level(
        // <y> Initial level
        // <id> pad_initial_level
        // <false"> Low
        // <true"> High
        false);
}