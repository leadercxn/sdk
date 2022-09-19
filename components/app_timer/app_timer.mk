CSRCS += app_timer_list.c

DEPPATH += --dep-path $(SDK_DIR)/components/app_timer
VPATH += :$(SDK_DIR)/components/app_timer

IFLAGS += "-I$(SDK_DIR)/components/app_timer"
