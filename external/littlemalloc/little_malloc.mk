CSRCS += little_malloc.c

DEPPATH += --dep-path $(SDK_DIR)/external/littlemalloc
VPATH += :$(SDK_DIR)/external/littlemalloc

IFLAGS += "-I$(SDK_DIR)/external/littlemalloc"