#ifndef __SYSTICK_H
#define __SYSTICK_H

#include <stdint.h>

typedef struct
{
    uint32_t    clk_div;
    uint32_t    ticks_per_us;
    uint32_t    ticks_per_sec;
    void        *p_systick;
} systick_cfg_t;

typedef struct
{
    void (* systick_init)      (systick_cfg_t *p_cfg);
    void (* systick_clk_div_set)   (systick_cfg_t *p_cfg, uint32_t clk_div);
    void (* systick_us_ticks_set)  (systick_cfg_t *p_cfg, uint32_t ticks_per_us);
    void (* delay_us)   (systick_cfg_t *p_cfg, uint32_t us);
    void (* delay_ms)   (systick_cfg_t *p_cfg, uint32_t ms);
    uint32_t (* systick_get)    (void);
} systick_ops_t;


typedef struct
{
    systick_cfg_t systick_cfg;
    systick_ops_t systick_ops;
} systick_object_t;

extern systick_object_t g_systick_obj;

#endif