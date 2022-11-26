CSRCS += 24cxx.c

DEPPATH += --dep-path $(SDK_DIR)/drivers/eeprom
VPATH += :$(SDK_DIR)/drivers/eeprom

IFLAGS += "-I$(SDK_DIR)/drivers/eeprom"
