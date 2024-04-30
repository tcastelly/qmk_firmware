#!/bin/bash

qmk compile -kb kbdfans/kbd75rgb -km tcy
qmk compile -kb preonic/rev3_drop -km tcy
qmk compile -kb splitkb/aurora/corne -km tcy
qmk compile -kb splitkb/aurora/corne -km tcy_trackpads
qmk compile -kb splitkb/aurora/corne -km tcy_pimoroni_trackball_oled
qmk compile -kb crkbd -km tcy
qmk compile -kb keyball/keyball44 -km tcy
qmk compile -kb totem -km tcy
qmk compile -kb nullbitsco/tidbit -km oled
qmk compile -kb rollow -km default -e CONVERT_TO=elite_pi
qmk compile -kb abitkeys/mc2s_lp -km tcy
