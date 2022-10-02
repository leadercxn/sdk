# 添加对应的hal头文件路径，在此文件中添加

# usart重定向
CSRCS += retarget.c

DEPPATH += --dep-path $(SDK_DIR)/customized/hal/usart
VPATH += :$(SDK_DIR)/customized/hal/usart

IFLAGS += "-I$(SDK_DIR)/customized/hal/usart"


# tftlcd
CSRCS += tftlcd.c

DEPPATH += --dep-path $(SDK_DIR)/customized/hal/tftlcd
VPATH += :$(SDK_DIR)/customized/hal/tftlcd

IFLAGS += "-I$(SDK_DIR)/customized/hal/tftlcd"


# add other hal header file
IFLAGS += "-I$(SDK_DIR)/customized/hal/gpio"
IFLAGS += "-I$(SDK_DIR)/customized/hal/systick"		
IFLAGS += "-I$(SDK_DIR)/customized/hal/timer"	
IFLAGS += "-I$(SDK_DIR)/customized/hal/i2c"	
IFLAGS += "-I$(SDK_DIR)/customized/hal/touch"	
IFLAGS += "-I$(SDK_DIR)/customized/hal/sdio"	
IFLAGS += "-I$(SDK_DIR)/customized/hal/exit"	
IFLAGS += "-I$(SDK_DIR)/customized/hal/flash"	
IFLAGS += "-I$(SDK_DIR)/customized/hal/dma"	
