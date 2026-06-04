#!/bin/bash

# if EE_HANDS in config.h
# eg:
# qmk flash -j 0 -kb totem -km tcy -bl uf2-split-left
# qmk flash -j 0 -kb crkbd -e CONVERT_TO=promicro_rp2040 -km tcy -bl uf2-split-left
#


# For CLion compatibility, add as sufix
# --compiledb && python3 gen_cmake.py

# Crone
# with right Azoteq trackpad
qmk compile -j 0 -kb crkbd/rev1 -km tcy -e CONVERT_TO=rp2040_ce

#
# Aurora Corne
#
# Trackpads + RGB
qmk compile -j 0 -kb splitkb/aurora/corne -km tcy_trackpads_left -e CONVERT_TO=rp2040_ce && \
qmk compile -j 0 -kb splitkb/aurora/corne -km tcy_trackpads_right -e CONVERT_TO=rp2040_ce && \

# Trackball + RGB
qmk compile -j 0 -kb splitkb/aurora/corne -km tcy_pmw3360 -e CONVERT_TO=rp2040_ce && \
#

# 
# Other
#
qmk compile -j 0 -kb tcy/other/rollow -km tcy && \
qmk compile -j 0 -kb tcy/other/keyball/keyball44 -km tcy && \
#

# TCY keyboards
#
# STM32 with Choc V1 switches (Mini Choc spaces) + Trackpoint + Hotswap module
qmk compile -j 0 -kb tcy/tp_mini_choc -km tcy && \

# STM32 with PG1316S switches + FPC Trackpad
qmk compile -j 0 -kb tcy/stm32 -km tcy && \

# RP2040 with Choc V1 switches (MX spaces) + Trackpoint + Hotswap module
qmk compile -j 0 -kb tcy/tp -km tcy && \

# RP2040 with PG1316S switches + Trackpoint + soldered Trackpad
qmk compile -j 0 -kb tcy/tp_pg1316s -km tcy
