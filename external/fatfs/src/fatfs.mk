CSRCS += diskio.c
CSRCS += ff.c

CSRCS += cc936.c

ifndef LVGL_ENABLE
CSRCS += exfuns.c
endif

DEPPATH += --dep-path $(SDK_DIR)/external/fatfs/src
VPATH += :$(SDK_DIR)/external/fatfs/src

DEPPATH += --dep-path $(SDK_DIR)/external/fatfs/src/option
VPATH += :$(SDK_DIR)/external/fatfs/src/option

IFLAGS += "-I$(SDK_DIR)/external/fatfs/src"
IFLAGS += "-I$(SDK_DIR)/external/fatfs/src/option"
