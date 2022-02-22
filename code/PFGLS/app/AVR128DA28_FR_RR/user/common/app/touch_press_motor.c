#include "touch_press_motor.h"
#include <stddef.h>

/**
 * 1.电容未触发的时候一直记录压力最低值
 * 2.电容触发后，当压力低于最低值-一定阈值，触发振动
 * 3.不断记录电机触发后的最低值，当压力高于最低值+一定阈值，触发振动
 * 4.并不断记录电机二次触发后的最高值，当压力低于最高值-一定阈值
 * 5.电容没有触发需要进入空闲状态
 * 
 * 事件1:电容触发  
 * 事件2:压力高于最低值+阈值D2  行为：  电机振动
 * 事件3:压力低于最高值-阈值D3  行为：  电机振动
 * 事件4:电容未触发 行为:如果触发当前事件时处于状态2，电机振动
 * 事件5:压力低于最低值-阈值D1    行为：  电机振动
 * 
 * 状态1：记录压力最低值
 * 状态2：记录压力最低值
 * 状态3：记录压力最高值
 * 状态4:空
 * 
 * 状态1-->事件1-->状态4
 * 状态4-->事件5-->状态2
 * 状态2-->事件2-->状态3
 * 状态3-->事件3-->状态2
 * 
 * 状态2-->事件4-->状态1
 * 状态3-->事件4-->状态1
 * 状态4-->事件4-->状态1
 * 
 */

#define STATE_NUM 4
#define EVENT_NUM 5
#define STATE_EVENT_TABLE_NUM 7

uint8_t event_det_touch_ok(state_obj_t *state_obj_ptr);
uint8_t event_det_press_higher_d2(state_obj_t *state_obj_ptr);
uint8_t event_det_press_lower_d3(state_obj_t *state_obj_ptr);
uint8_t event_det_touch_no(state_obj_t *state_obj_ptr);
uint8_t event_det_press_lower_d1(state_obj_t *state_obj_ptr);

void event_action_motor_shake(state_obj_t *state_obj_ptr);
void event_action_motor_shake_cap_no(state_obj_t *state_obj_ptr);

typedef uint8_t (*event_det_cb_t)(state_obj_t *state_obj_ptr);
typedef void (*event_action_cb_t)(state_obj_t *state_obj_ptr);

typedef struct
{
    event_det_cb_t det_cb;
    event_action_cb_t action_cb;
} event_t;

event_t g_event[EVENT_NUM] = {
    {
        event_det_touch_ok,
        NULL,
    },
    {
        event_det_press_higher_d2,
        event_action_motor_shake,
    },
    {
        event_det_press_lower_d3,
        event_action_motor_shake,
    },
    {
        event_det_touch_no,
        event_action_motor_shake_cap_no,
    },
    {
        event_det_press_lower_d1,
        event_action_motor_shake,
    },
};

void state_action_record_press_min(state_obj_t *state_obj_ptr);
void state_action_record_press_max(state_obj_t *state_obj_ptr);

typedef void (*state_action_cb_t)(state_obj_t *state_obj_ptr);

state_action_cb_t g_state[STATE_NUM] = {
    state_action_record_press_min,
    state_action_record_press_min,
    state_action_record_press_max,
    NULL,
};

typedef struct
{
    uint8_t cur_state_handle;
    uint8_t event_handle;
    uint8_t next_state_handle;
} state_event_table_t;

state_event_table_t g_state_event_table[STATE_EVENT_TABLE_NUM] = {
    {0, 0, 3},
    {3, 4, 1},
    {1, 1, 2},
    {2, 2, 1},
    {1, 3, 0},
    {2, 3, 0},
    {3, 3, 0},
};

void state_mahine_run(state_obj_t *state_obj_ptr)
{
    uint8_t cur_state_handle;
    event_det_cb_t event_det_cb;
    uint8_t det_res = 0;
    event_action_cb_t action_cb;
    uint8_t event_handle = 0;
    uint8_t state_handle = 0;

    state_handle = state_obj_ptr->state_handle;

    /* 运行当前状态的行为 */
    if (g_state[state_handle] != NULL)
    {
        g_state[state_handle](state_obj_ptr);
    }

    /* 监测当前状态所监测的事件 */
    for (uint8_t i = 0; i < STATE_EVENT_TABLE_NUM; i++)
    {
        cur_state_handle = g_state_event_table[i].cur_state_handle;
        if ((state_handle) == cur_state_handle)
        {
            event_handle = g_state_event_table[i].event_handle;
            event_det_cb = g_event[event_handle].det_cb;
            if (event_det_cb != NULL)
            {
                /* 当前状态所需要运行的事件监测 */
                det_res = event_det_cb(state_obj_ptr);
            }

            /* 当监测的事件确实发生后,运行事件发生后的回调函数，同时切换状态 */
            if (det_res != 0)
            {
                action_cb = g_event[event_handle].action_cb;
                /* 切换状态 */
                state_obj_ptr->state_handle = g_state_event_table[i].next_state_handle;
                /* 事件发生后的回调函数 */
                if (action_cb != 0)
                {
                    action_cb(state_obj_ptr);
                }
            }
        }
    }
}

// 记录压力最低值
void state_action_record_press_min(state_obj_t *state_obj_ptr)
{
    float filt_val=0.7;
    // if (state_obj_ptr->adc_res < state_obj_ptr->adc_min)
    {
        state_obj_ptr->adc_min = state_obj_ptr->adc_min*filt_val+(state_obj_ptr->adc_res)*(1-filt_val);
    }
}

// 记录压力最低值
void state_action_record_press_max(state_obj_t *state_obj_ptr)
{
    if (state_obj_ptr->adc_res > state_obj_ptr->adc_max)
    {
        state_obj_ptr->adc_max = state_obj_ptr->adc_res;
    }
}

// 电容触发
uint8_t event_det_touch_ok(state_obj_t *state_obj_ptr)
{
    if (state_obj_ptr->capsense_status == 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

// 压力低于最低值-阈值D1
uint8_t event_det_press_lower_d1(state_obj_t *state_obj_ptr)
{
    uint8_t delta_D1 = state_obj_ptr->delta_D1;
    uint32_t adc_res = state_obj_ptr->adc_res;
    uint32_t adc_min = state_obj_ptr->adc_min;

    if ((adc_res + delta_D1) < adc_min)
    {
		state_obj_ptr->adc_min=adc_res;
        return 1;
    }
    return 0;
}

// 压力高于最低值+阈值D2
uint8_t event_det_press_higher_d2(state_obj_t *state_obj_ptr)
{
    uint8_t delta_D2 = state_obj_ptr->delta_D2;
    uint32_t adc_res = state_obj_ptr->adc_res;
    uint32_t adc_min = state_obj_ptr->adc_min;

    if (adc_res > (adc_min + delta_D2))
    {
		state_obj_ptr->adc_max=adc_res;
        return 1;
    }
    return 0;
}

// 压力低于最高值-阈值D3
uint8_t event_det_press_lower_d3(state_obj_t *state_obj_ptr)
{
    uint8_t delta_D3 = state_obj_ptr->delta_D3;
    uint32_t adc_res = state_obj_ptr->adc_res;
    uint32_t adc_max = state_obj_ptr->adc_max;

    if ((adc_res + delta_D3) < adc_max)
    {
		state_obj_ptr->adc_min=adc_res;
        return 1;
    }
    return 0;
}

// 电容未触发
uint8_t event_det_touch_no(state_obj_t *state_obj_ptr)
{
    if (state_obj_ptr->capsense_status != 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

// 电机振动回调函数
void event_action_motor_shake(state_obj_t *state_obj_ptr)
{
    motor_shake_cb_t cb = state_obj_ptr->motor_shake;
    if (cb != NULL)
    {
        cb();
    }
}

void event_action_motor_shake_cap_no(state_obj_t *state_obj_ptr)
{
    motor_shake_cb_t cb = state_obj_ptr->motor_shake;
    state_obj_ptr->adc_min=state_obj_ptr->adc_res;
    if (state_obj_ptr->state_handle == 1)
    {
        if (cb != NULL)
        {
            cb();
        }
    }
}
