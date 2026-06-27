# DYNA 65

![DYNA 65 PCB]()

DYNA 65 is a Hybrid MX/EC 65% PCB

* Keyboard Maintainer: [cipulot](https://github.com/cipulot)
* Hardware Supported: DYNA 65
* Hardware Availability: TBD

Make example for this keyboard (after setting up your build environment):

    make cipulot/dyna_65/ansi:default
    make cipulot/dyna_65/bauer_lite_ansi:default
    make cipulot/dyna_65/iso:default
    make cipulot/dyna_65/bauer_lite_iso:default

Flashing example for this keyboard:

    make cipulot/dyna_65/ansi:default:flash
    make cipulot/dyna_65/bauer_lite_ansi:default:flash
    make cipulot/dyna_65/iso:default:flash
    make cipulot/dyna_65/bauer_lite_iso:default:flash

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Bootloader

Enter the bootloader in 2 ways:

* **Physical button**: Long press the button on the back of the PCB
* **Keycode in layout**: Press the key mapped to `QK_BOOT` if it is available
