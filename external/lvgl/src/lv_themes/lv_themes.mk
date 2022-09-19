CSRCS += lv_theme.c

CSRCS += lv_theme_empty.c
CSRCS += lv_theme_material.c
CSRCS += lv_theme_mono.c
CSRCS += lv_theme_template.c

DEPPATH += --dep-path $(SDK_DIR)/external/lvgl/src/lv_themes
VPATH += :$(SDK_DIR)/external/lvgl/src/lv_themes

IFLAGS += "-I$(SDK_DIR)/external/lvgl/src/lv_themes"
