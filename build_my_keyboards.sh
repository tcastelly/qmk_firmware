#!/bin/bash

qmk compile -kb kbdfans/kbd75rgb -km tcy
qmk compile -kb preonic/rev3_drop -km tcy
qmk compile -kb splitkb/aurora/corne -km tcy
qmk compile -kb splitkb/aurora/corne -km tcy_trackpads
qmk compile -kb splitkb/aurora/corne -km tcy_pimoroni_trackball_oled
qmk compile -kb crkbd -km tcy
qmk compile -kb crkbd -e CONVERT_TO=promicro_rp2040 -km tcy_trackpad
qmk compile -kb keyball/keyball44 -km tcy
qmk compile -kb totem -km tcy
qmk compile -kb nullbitsco/tidbit -km oled
qmk compile -kb rollow -km tcy
qmk compile -kb rollow -km tcy_trackpad
qmk compile -kb abitkeys/mc2s_lp -km tcy
