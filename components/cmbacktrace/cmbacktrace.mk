CSRCS += cm_backtrace.c


DEPPATH += --dep-path $(SDK_DIR)/components/cmbacktrace
VPATH += :$(SDK_DIR)/components/cmbacktrace

IFLAGS += "-I$(SDK_DIR)/components/cmbacktrace"
IFLAGS += "-I$(SDK_DIR)/components/cmbacktrace/Languages"