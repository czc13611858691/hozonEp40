#include "soft_timer.h"
#include <stdlib.h>
#include <string.h>

typedef struct soft_timer_obj
{
    soft_timer_base_t cnt;
    soft_timer_base_t period;
    soft_timer_cb cb;
    struct soft_timer_obj* next;
} soft_timer_obj_t;

soft_timer_obj_t* g_soft_timer_head = NULL;

void soft_timer_create(soft_timer_base_t period, soft_timer_cb cb)
{
    soft_timer_obj_t* obj = NULL;
    obj = (soft_timer_obj_t*)malloc(sizeof(soft_timer_obj_t));
    if (obj == NULL)
    {
        return;
    }
    memset(obj, 0, sizeof(soft_timer_obj_t));

    obj->period = period;
    obj->cb = cb;
    obj->next = g_soft_timer_head;
    g_soft_timer_head = obj;
}

#define OVER_FLOW_VAL_DET 60000U // soft_timer_base_t--uint16_t

void soft_timer_run(void)
{
    soft_timer_obj_t* obj = g_soft_timer_head;
    for (; obj != NULL; obj = obj->next)
    {
        if (((soft_timer_base_t)(g_soft_timer_ticks - obj->cnt) >= obj->period) && ((soft_timer_base_t)(g_soft_timer_ticks - obj->cnt) <= OVER_FLOW_VAL_DET))
        {
            obj->cnt = g_soft_timer_ticks;
            obj->cb();
        }
    }
}