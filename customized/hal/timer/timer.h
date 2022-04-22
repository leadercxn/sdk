#ifndef __TIMER_H
#define __TIMER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct
{
    void        *timer;         //可指向结构体，或定时器地址

    uint32_t    prescaler_freq; //分频后得到的时钟频率
    uint32_t    period;         //溢出计数
    uint32_t    flag;

    uint64_t    ticks;          //该定时器中断计数
    void        *p_context;     //传递给中断处理函数的数据
} timer_cfg_t;

typedef struct
{
    int (* timer_init)      (timer_cfg_t *p_timer_cfg); //初始化
    int (* timer_start)     (timer_cfg_t *p_timer_cfg); //开始定时器
    int (* timer_stop)      (timer_cfg_t *p_timer_cfg); //停止定时器
    int (* timer_param_modify)  (timer_cfg_t *p_timer_cfg); //修改，设置定时器参数
    int (* timer_irq_enable)    (timer_cfg_t *p_timer_cfg); //使能定时中断
    int (* timer_irq_disable)   (timer_cfg_t *p_timer_cfg); //禁止定时中断
    void (* timer_irq_cb)       (timer_cfg_t *p_timer_cfg);//放在定时器中断函数里的回调函数
    uint64_t (* timer_tick_get) (timer_cfg_t *p_timer_cfg); //获取该定时器的心跳数
} timer_ops_t;


typedef struct
{
    timer_cfg_t timer_cfg;
    timer_ops_t timer_ops;
} timer_object_t;

#endif
