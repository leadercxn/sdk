# ILI9341
CSRCS += ili9341_fsmc.c

DEPPATH += --dep-path $(SDK_DIR)/drivers/tftlcd/ili9341
VPATH += :$(SDK_DIR)/drivers/tftlcd/ili9341

IFLAGS += "-I$(SDK_DIR)/drivers/tftlcd/ili9341"


# NT35510
CSRCS += nt35510_fsmc.c

DEPPATH += --dep-path $(SDK_DIR)/drivers/tftlcd/nt35510
VPATH += :$(SDK_DIR)/drivers/tftlcd/nt35510

IFLAGS += "-I$(SDK_DIR)/drivers/tftlcd/nt35510"


# ST7789
CSRCS += st7789_8080.c
CSRCS += st7789_fsmc.c

DEPPATH += --dep-path $(SDK_DIR)/drivers/tftlcd/st7789
VPATH += :$(SDK_DIR)/drivers/tftlcd/st7789

IFLAGS += "-I$(SDK_DIR)/drivers/tftlcd/st7789"

