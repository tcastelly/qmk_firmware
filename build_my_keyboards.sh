#!/bin/bash

# if EE_HANDS in config.h
# eg:
# qmk flash -j 0 -kb totem -km tcy -bl uf2-split-left
# qmk flash -j 0 -kb crkbd -e CONVERT_TO=promicro_rp2040 -km tcy  -bl uf2-split-left

qmk compile -j 0 -kb kbdfans/kbd75rgb -km tcy
qmk compile -j 0 -kb preonic/rev3_drop -km tcy
qmk compile -j 0 -kb splitkb/aurora/corne -km tcy
qmk compile -j 0 -kb splitkb/aurora/corne -km tcy_trackpads
qmk compile -j 0 -kb splitkb/aurora/corne -km tcy_pimoroni_trackball_oled

# for atmega (unicorne)
qmk compile -j 0 -kb crkbd -km tcy

# for rp2040
qmk compile -j 0 -kb crkbd -e CONVERT_TO=promicro_rp2040 -km tcy
qmk compile -j 0 -kb crkbd -e CONVERT_TO=promicro_rp2040 -km tcy_trackpad

qmk compile -j 0 -kb keyball/keyball44 -km tcy
qmk compile -j 0 -kb totem -km tcy
qmk compile -j 0 -kb nullbitsco/tidbit -km tcy
qmk compile -j 0 -kb rollow -km tcy
qmk compile -j 0 -kb rollow -km tcy_trackpad
qmk compile -j 0 -kb abitkeys/mc2s_lp -km tcy
qmk compile -j 0 -kb santoku -km tcy
qmk compile -j 0 -kb bastardkb/charybdis/3x6 -km tcy
