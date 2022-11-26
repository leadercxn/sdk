include $(SDK_DIR)/components/trace/trace.mk
include $(SDK_DIR)/components/app_scheduler/app_scheduler.mk
include $(SDK_DIR)/components/app_timer/app_timer.mk
include $(SDK_DIR)/components/letter-shell/src/letter-shell.mk
include $(SDK_DIR)/components/cmbacktrace/cmbacktrace.mk

IFLAGS += "-I$(SDK_DIR)/components/util"
IFLAGS += "-I$(SDK_DIR)/components/lib_err"
IFLAGS += "-I$(SDK_DIR)/components/queue"
