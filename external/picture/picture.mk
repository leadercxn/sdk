CSRCS += piclib.c

DEPPATH += --dep-path $(SDK_DIR)/external/picture
VPATH += :$(SDK_DIR)/external/picture

IFLAGS += "-I$(SDK_DIR)/external/picture"


CSRCS += bmp.c

DEPPATH += --dep-path $(SDK_DIR)/external/picture/bmp
VPATH += :$(SDK_DIR)/external/picture/bmp

IFLAGS += "-I$(SDK_DIR)/external/picture/bmp"


CSRCS += gif.c

DEPPATH += --dep-path $(SDK_DIR)/external/picture/gif
VPATH += :$(SDK_DIR)/external/picture/gif

IFLAGS += "-I$(SDK_DIR)/external/picture/gif"


CSRCS += tjpgd.c

DEPPATH += --dep-path $(SDK_DIR)/external/picture/jpg
VPATH += :$(SDK_DIR)/external/picture/jpg

IFLAGS += "-I$(SDK_DIR)/external/picture/jpg"
