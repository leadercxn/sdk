#ifndef __EXIT_H
#define __EXIT_H

#include "stdint.h"

typedef struct
{
    void *p_pin_cfg;
    void *p_exit_cfg;

    uint32_t flag;

    void *p_context;     //传递给中断处理函数的数据

    void (* delay_ms)   (uint32_t ms);
} exit_cfg_t;

typedef struct
{
    int (* exit_init) (exit_cfg_t *p_exit_cfg);
    int (* exit_enable) (exit_cfg_t *p_exit_cfg);
    int (* exit_disable) (exit_cfg_t *p_exit_cfg);

    void (* exit_irq_cb) (exit_cfg_t *p_exit_cfg);  //中断回调函数
} exit_ops_t;

typedef struct
{
    exit_cfg_t exit_cfg;
    exit_ops_t exit_ops;
} exit_object_t;

#endif
