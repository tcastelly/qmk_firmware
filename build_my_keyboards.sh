#!/bin/bash

qmk compile -kb kbdfans/kbd75rgb -km tcy
qmk compile -kb preonic/rev3_drop -km tcy
qmk compile -kb splitkb/aurora/corne -km tcy_trackpads -e CONVERT_TO=elite_pi
qmk compile -kb splitkb/aurora/corne -km tcy_trackpoint -e CONVERT_TO=elite_pi
qmk compile -kb splitkb/aurora/corne -km tcy_promorini_trackball_oled -e CONVERT_TO=elite_pi
qmk compile -kb crkbd -km tcy
qmk compile -kb keyball/keyball44 -km tcy
qmk compile -kb totem -km tcy
qmk compile -kb nullbitsco/tidbit -km oled
