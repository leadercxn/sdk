include $(SDK_DIR)/external/fatfs/src/fatfs.mk


ifdef LVGL_ENABLE
include $(SDK_DIR)/external/lvgl/lvgl.mk
else
include $(SDK_DIR)/external/picture/picture.mk
include $(SDK_DIR)/external/littlemalloc/little_malloc.mk
endif

