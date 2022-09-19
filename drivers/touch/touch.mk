CSRCS += gt9147.c

DEPPATH += --dep-path $(SDK_DIR)/drivers/touch
VPATH += :$(SDK_DIR)/drivers/touch

IFLAGS += "-I$(SDK_DIR)/drivers/touch"
