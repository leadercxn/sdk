# LVGL使用方法
## 1. 移植
1. 对于LVGL的移植，有以下操作:
   1. `lvgl/porting/lv_port.disp.c`
      1. `lv_port_disp_init`:选择buffer config-->选择了example 1，其他的屏蔽即可;同时把disp_drv.ver_res/hor_res与tftlcd对象关联起来即可。
      2. `disp_flush`:在最内层的函数中移植描点函数
      3. `disp_init`:通过`g_tftlcd_lvgl_obj`对象把该lcd硬件初始化
   2. `lvgl/porting/lv_port.disp.c`
      1. 声明函数`lv_port_disp_init`
      2. 外部声明`g_tftlcd_lvgl_obj`，该变量是专门用于lvgl的底层操作，有可能会在`hk_peripheral.c`中定义多个tftlcd对象(即不同屏幕)，使用该lvgv对象指向对应的lcd屏幕即可以完成移植
   3. `hk_lib/fx/timer/hk_timer.c`
      1. 在定时器中断中添加`lv_tick_inc(1);`定时周期需要为1ms
   4. 在链接脚本中，把STACK/HEAP的最小值调到0x2000(8kb)

## 2. 使用
1. 与lvgl关联的对象为`g_tftlcd_lvgl_obj`，因此在`hk_peripheral.c`中，把需要现实的lcd对象指向该指针`g_tftlcd_lvgl_obj`即可。其他的操作，如读点、参数设置，则交给具体的lcd对象实现
2. 在main函数中，只需要做lvgl初始化`lv_init`以及lvgl display初始化`lv_port_disp_init`，并定时调用`lv_task_handler`



