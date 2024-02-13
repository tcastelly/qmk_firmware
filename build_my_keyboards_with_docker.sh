#!/bin/bash

git submodule update --init --recursive

r=`docker image inspect qmkfm/qmk_cli 2>/dev/null`

if [ "$r" == [] ]; then
    docker build -t qmkfm/qmk_cli .
fi

docker run -v $PWD:/qmk_firmware --rm -v $PWD/.build:/qmk_firmware/.build -it qmkfm/qmk_cli qmk compile -kb kbdfans/kbd75rgb -km tcy
docker run -v $PWD:/qmk_firmware --rm -v $PWD/.build:/qmk_firmware/.build -it qmkfm/qmk_cli qmk compile -kb preonic/rev3_drop -km tcy
docker run -v $PWD:/qmk_firmware --rm -v $PWD/.build:/qmk_firmware/.build -it qmkfm/qmk_cli qmk compile -kb splitkb/aurora/corne -km tcy -e CONVERT_TO=elite_pi
docker run -v $PWD:/qmk_firmware --rm -v $PWD/.build:/qmk_firmware/.build -it qmkfm/qmk_cli qmk compile -kb crkbd -km tcy
docker run -v $PWD:/qmk_firmware --rm -v $PWD/.build:/qmk_firmware/.build -it qmkfm/qmk_cli qmk compile -kb keyball/keyball44 -km tcy
docker run -v $PWD:/qmk_firmware --rm -v $PWD/.build:/qmk_firmware/.build -it qmkfm/qmk_cli qmk compile -kb totem -km tcy
