#!/bin/bash

qmk compile -kb kbdfans/kbd75rgb -km tcy
qmk compile -kb preonic/rev3_drop -km tcy
qmk compile -kb splitkb/aurora/corne -km tcy -e CONVERT_TO=elite_pi
qmk compile -kb crkbd -km tcy
qmk compile -kb keyball/keyball44 -km tcy
qmk compile -kb totem -km tcy
