#ifndef __APP_TIMER_LIST_H
#define __APP_TIMER_LIST_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "timer.h"

typedef void (*timer_node_handler_t) (void *p_data);

/**
 * 注: handler 不能放耗时过长的函数
 */
typedef struct
{
    timer_object_t          *p_timer_base_obj;  //

    bool                    active;
    bool                    single_mode;        //单次模式
    bool                    immediately;        //true:在中断中执行  false:利用scheduler调度
    uint32_t                remain_ms;          //剩余多少ms
    uint32_t                init_ms;
    void                    *p_data;
    timer_node_handler_t    handler;
} timer_node_t;

typedef timer_node_t *  timer_node_id_t;

int timer_init(timer_object_t *p_timer_obj);
int timer_create( timer_node_id_t const *p_timer_id, bool single_mode, bool immediately, timer_node_handler_t timeout_handler);
int timer_start(timer_node_id_t timer_id, uint32_t ms, void *p_data);
int timer_stop(timer_node_id_t timer_id);
void timer_loop_task(void);

#endif

