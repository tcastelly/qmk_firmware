CUSTOM_MATRIX = lite
ANALOG_DRIVER_REQUIRED = yes
QUANTUM_PAINTER_DRIVERS += sh1106_i2c

SRC += matrix.c hybrid_switch_matrix.c

SRC += graphics/display.c
SRC += graphics/splash_kenban_de_go.qgf.c
SRC += graphics/splash_cipulot.qgf.c
SRC += graphics/densha.qgf.c


MCUFLAGS += -march=armv7e-m \
            -mcpu=cortex-m4 \
            -mfloat-abi=hard \
            -mfpu=fpv4-sp-d16 \
            -mabi=aapcs \
            -mcmodel=small \
            -mstrict-align \
            -mthumb

OPT_DEFS += -Ofast -ffast-math -funroll-loops \
            -fno-tree-vectorize -fno-signed-zeros -fno-math-errno \
            -fno-common -fomit-frame-pointer -falign-functions=16 \
            -falign-loops=16 -falign-jumps=16 -fno-exceptions -fno-unwind-tables

# This tells the compiler where to look for "socd_cleaner.h"
USER_INCLUDES += users/cipulot/modules/getreuer/socd_cleaner

# Link the source file
COMMON_VPATH += users/cipulot/modules/getreuer/socd_cleaner
