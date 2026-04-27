SRC += users/tcy/stm32/pmw3360_srom_v04.c
SRC += drivers/sensors/pmw33xx_common.c
SRC += drivers/sensors/pmw3360.c
VPATH += drivers/sensors

PMW3360_CUSTOM_ENABLE = yes

ifeq ($(PMW3360_CUSTOM_ENABLE), yes)
    OPT_DEFS += -DPMW3360_CUSTOM_ENABLE
endif


SPI_DRIVER_REQUIRED = yes

