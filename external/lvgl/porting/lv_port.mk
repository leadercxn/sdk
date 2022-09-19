CSRCS += lv_port_disp.c
CSRCS += lv_port_fs.c
CSRCS += lv_port_indev.c

DEPPATH += --dep-path $(SDK_DIR)/external/lvgl/porting
VPATH += :$(SDK_DIR)/external/lvgl/porting

IFLAGS += "-I$(SDK_DIR)/external/lvgl/porting"