CSRCS += shell_cmd_list.c
CSRCS += shell_companion.c
CSRCS += shell_ext.c
CSRCS += shell.c

DEPPATH += --dep-path $(SDK_DIR)/components/letter-shell/src
VPATH += :$(SDK_DIR)/components/letter-shell/src

IFLAGS += "-I$(SDK_DIR)/components/letter-shell/src"