CSRCS += encoder.c

DEPPATH += --dep-path $(SDK_DIR)/drivers/encoder
VPATH += :$(SDK_DIR)/drivers/encoder

IFLAGS += "-I$(SDK_DIR)/drivers/encoder"
