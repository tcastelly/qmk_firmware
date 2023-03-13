#!/bin/bash
set -exo pipefail

git submodule update --init --recursive

# docker build -t qmkfm/qmk_cli .
docker run -v $PWD:/qmk_firmware -v $PWD/.build:/qmk_firmware/.build -it qmkfm/qmk_cli qmk compile -kb kbdfans/kbd75rgb -km tcy
docker run -v $PWD:/qmk_firmware -v $PWD/.build:/qmk_firmware/.build -it qmkfm/qmk_cli qmk compile -kb preonic/rev3_drop -km tcy
docker run -v $PWD:/qmk_firmware -v $PWD/.build:/qmk_firmware/.build -it qmkfm/qmk_cli qmk compile -kb splitkb/aurora/corne -km tcy -e CONVERT_TO=elite_pi
