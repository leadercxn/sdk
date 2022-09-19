CSRCS += lv_group.c
CSRCS += lv_indev.c
CSRCS += lv_disp.c
CSRCS += lv_obj.c
CSRCS += lv_refr.c
CSRCS += lv_style.c

DEPPATH += --dep-path $(SDK_DIR)/external/lvgl/src/lv_core
VPATH += :$(SDK_DIR)/external/lvgl/src/lv_core

IFLAGS += "-I$(SDK_DIR)/external/lvgl/src/lv_core"
