CSRCS += lv_draw_mask.c
CSRCS += lv_draw_blend.c
CSRCS += lv_draw_rect.c
CSRCS += lv_draw_label.c
CSRCS += lv_draw_line.c
CSRCS += lv_draw_img.c
CSRCS += lv_draw_arc.c
CSRCS += lv_draw_triangle.c
CSRCS += lv_img_decoder.c
CSRCS += lv_img_cache.c
CSRCS += lv_img_buf.c

DEPPATH += --dep-path $(SDK_DIR)/external/lvgl/src/lv_draw
VPATH += :$(SDK_DIR)/external/lvgl/src/lv_draw

IFLAGS += "-I$(SDK_DIR)/external/lvgl/src/lv_draw"