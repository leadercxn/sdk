CSRCS += trace.c

DEPPATH += --dep-path $(SDK_DIR)/components/trace
VPATH += :$(SDK_DIR)/components/trace

IFLAGS += "-I$(SDK_DIR)/components/trace"