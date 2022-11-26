CSRCS += lv_hal_disp.c
CSRCS += lv_hal_indev.c
CSRCS += lv_hal_tick.c

DEPPATH += --dep-path $(SDK_DIR)/external/lvgl/src/lv_hal
VPATH += :$(SDK_DIR)/external/lvgl/src/lv_hal

IFLAGS += "-I$(SDK_DIR)/external/lvgl/src/lv_hal"
