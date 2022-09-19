include $(SDK_DIR)/external/lvgl/porting/lv_port.mk

include $(SDK_DIR)/external/lvgl/src/lv_core/lv_core.mk
include $(SDK_DIR)/external/lvgl/src/lv_draw/lv_draw.mk
include $(SDK_DIR)/external/lvgl/src/lv_font/lv_font.mk
include $(SDK_DIR)/external/lvgl/src/lv_gpu/lv_gpu.mk
include $(SDK_DIR)/external/lvgl/src/lv_hal/lv_hal.mk
include $(SDK_DIR)/external/lvgl/src/lv_misc/lv_misc.mk
include $(SDK_DIR)/external/lvgl/src/lv_themes/lv_themes.mk
include $(SDK_DIR)/external/lvgl/src/lv_widgets/lv_widgets.mk

IFLAGS += "-I$(SDK_DIR)/external/lvgl"
IFLAGS += "-I$(SDK_DIR)/external/lvgl/src"
