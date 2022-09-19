CSRCS += lv_gpu_stm32_dma2d.c

DEPPATH += --dep-path $(SDK_DIR)/external/lvgl/src/lv_gpu
VPATH += :$(SDK_DIR)/external/lvgl/src/lv_gpu

IFLAGS += "-I$(SDK_DIR)/external/lvgl/src/lv_gpu"
