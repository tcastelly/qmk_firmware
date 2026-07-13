# Cipulot common EC driver — the same sources the other EC boards in this tree
# build from, so it rides qmk master. keyboards/cipulot/common/common_cipulot.mk
# itself is NOT included: it hardcodes Cortex-M4 FPU MCUFLAGS and -Ofast, and
# the STM32F072 is a Cortex-M0.
CUSTOM_MATRIX = lite
ANALOG_DRIVER_REQUIRED = yes
VPATH += keyboards/cipulot/common
SRC += matrix.c ec_board.c ec_switch_matrix.c

ifeq ($(strip $(VIA_ENABLE)), yes)
    SRC += via_ec.c
endif

# socd_cleaner.h (types used by the EC eeprom config struct) lives in the
# getreuer module — same include trick as hybrid_kenban_de_go.
USER_INCLUDES += modules/getreuer/socd_cleaner
COMMON_VPATH += modules/getreuer/socd_cleaner
