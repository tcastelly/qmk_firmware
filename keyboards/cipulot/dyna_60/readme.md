# DYNA 60

![DYNA 60 PCB]()

DYNA 60 is a Hybrid MX/EC 60% PCB

* Keyboard Maintainer: [cipulot](https://github.com/cipulot)
* Hardware Supported: DYNA 60
* Hardware Availability: TBD

Make example for this keyboard (after setting up your build environment):

    make cipulot/dyna_60/ansi:default
    make cipulot/dyna_60/iso:default

Flashing example for this keyboard:

    make cipulot/dyna_60/ansi:default:flash
    make cipulot/dyna_60/iso:default:flash

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Bootloader

Enter the bootloader in 2 ways:

* **Physical button**: Long press the button on the back of the PCB
* **Keycode in layout**: Press the key mapped to `QK_BOOT` if it is available
