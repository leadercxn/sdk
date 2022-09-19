CSRCS += app_scheduler.c

DEPPATH += --dep-path $(SDK_DIR)/components/app_scheduler
VPATH += :$(SDK_DIR)/components/app_scheduler

IFLAGS += "-I$(SDK_DIR)/components/app_scheduler"