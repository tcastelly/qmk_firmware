#!/bin/bash

# if EE_HANDS in config.h
# eg:
# qmk flash -j 0 -kb totem -km tcy -bl uf2-split-left
# qmk flash -j 0 -kb crkbd -e CONVERT_TO=promicro_rp2040 -km tcy -bl uf2-split-left
#

# Aurora Corne
#
# Trackpads 
qmk compile -j 0 -kb splitkb/aurora/corne -km tcy_trackpads_left -e CONVERT_TO=rp2040_ce
qmk compile -j 0 -kb splitkb/aurora/corne -km tcy_trackpads_right -e CONVERT_TO=rp2040_ce

qmk compile -j 0 -kb tcy/tp_mini_choc -km tcy
qmk compile -j 0 -kb tcy/stm32 -km tcy
qmk compile -j 0 -kb tcy/tp -km tcy
