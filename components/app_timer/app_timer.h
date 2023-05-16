/* Copyright (c) 2019 SENSORO Co.,Ltd. All Rights Reserved.
 *
 */

#ifndef APP_TIMER_H__
#define APP_TIMER_H__

#include "app_timer_list.h"
#include "trace.h"

#define TIMER_DEF(timer_id) \
            static timer_node_t timer_id##_data = { .active = false };   \
            static const timer_node_id_t timer_id = &timer_id##_data;   \
            bool timer_id##_running = false;    \


#define TIMER_INIT(p_timer_obj) timer_init(p_timer_obj)

#define TIMER_SCHEDULER_LOOP()  timer_loop_task()

#define TIMER_CREATE(p_id, singlemode, immediately, handler)                        \
{                                                                                   \
    int err_code;                                                                   \
    err_code = timer_create(p_id, singlemode, immediately, handler);            \
    if(err_code)                                                                    \
    {                                                                               \
        trace_error("TIMER_CREATE error %d\n\r",err_code);                          \
    }                                                                               \
}                                                                                   \

#define TIMER_START(id, ms)                                                         \
{                                                                                   \
    int err_code;                                                                   \
    err_code = timer_start(id, ms, NULL);                                       \
    if(err_code)                                                                    \
    {                                                                               \
        trace_error("TIMER_START error %d\n\r",err_code);                           \
    }                                                                               \
    id##_running = true;                                                            \
}                                                                                   \

#define TIMER_START_WITH_PARAM(id, ms, param)                                       \
{                                                                                   \
    int err_code;                                                                   \
    err_code = timer_start(id, ms, param);                                      \
    if(err_code)                                                                    \
    {                                                                               \
        trace_error("TIMER_START_WITH_PARAM error %d\n\r",err_code);                \
    }                                                                               \
    id##_running = true;                                                            \
}                                                                                   \

#define TIMER_STOP(id)                                                              \
{                                                                                   \
    int err_code;                                                                   \
    err_code = timer_stop(id);                                                  \
    if(err_code)                                                                    \
    {                                                                               \
        trace_error("TIMER_STOP error %d\n\r",err_code);                            \
    }                                                                               \
    id##_running = false;                                                           \
}                                                                                   \


#endif
