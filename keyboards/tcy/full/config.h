/*
This is the c configuration file for the keymap

Copyright 2012 Jun Wako <wakojun@gmail.com>
Copyright 2015 Jack Humbert

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

/* ── PS/2: TWO devices at once ──────────────────────────────
 * Same custom driver as tp_mini_choc (users/tcy/stm32/ps2.c), but this board
 * carries a trackpoint AND an Elantech touchpad, so the pinset list has two
 * entries. Both the order and the pin choice are load-bearing:
 *
 *   index 0 → pinset 1 = PB8 (CLK) / PB9  (DAT) → EXTI8 → TRACKPOINT
 *   index 1 → pinset 5 = PA9 (CLK) / PA10 (DAT) → EXTI9 → ELANTECH TOUCHPAD
 *
 * The TRACKPOINT MUST BE PINSET 1. ps2.c gates the post-processing on the
 * pinset NUMBER, not on the device index:
 *
 *     if (pinset == 1) dev_report = ps2_acceleration_task(dev_report);
 *
 * On any other pinset the trackpoint still reports, but silently loses both
 * its acceleration curve and its axis rotation — i.e. it points the wrong way.
 *
 * Pinsets 1 and 5 are the only pair that can run simultaneously: their CLK
 * lines land on EXTI8 and EXTI9, which are distinct. Pinset 2 (PB10/PB11) is
 * out because that is I2C2 — needed here for the MCP23017 matrix, the MCP23008
 * mouse and both ATtiny85s. Pinset 5 costs the USART1 pins, which nothing here
 * uses: the split link is I2C2, not serial.
 * ─────────────────────────────────────────────────────────── */
#define PS2_PINSETS { 1, 5 }

/* INDEX INTO PS2_PINSETS — not a pinset number. 1 = the second entry = pinset
 * 5 = the Elantech pad. That device is switched to raw byte mode and dropped
 * from the relative-packet path, so elantech.c can own its reset / absolute-
 * mode / reporting sequence through the ps2_host_* shim (ps2_host_compat.h).
 * Set this to 0 only if the pad and the trackpoint swap headers. */
#define PS2_HOST_API_DEV 1

/* ── RGB — ATtiny85 slaves, not driven by this MCU ──────────
 * The STM32 never touches a WS2812 line: each half has its own ATtiny85 that
 * owns its chain, and QMK ships it a colour over I2C2 (users/tcy/attiny85_rgb).
 * RGBLIGHT_DRIVER = custom in rules.mk is what makes that legal — it drops
 * rgblight's ws2812 back end entirely, so there is no pin to assign and, more
 * importantly, no bitbang write holding interrupts off ~30 µs per LED. That
 * matters here: the PS/2 driver is EXTI-interrupt driven, and 58 LEDs of
 * bitbang would mask interrupts for ~1.7 ms per refresh — long enough to shred
 * PS/2 packets on both devices.
 *
 * 29 LEDs per half, matching NUM_LEDS_LEFT/RIGHT in the ATtiny sketch.
 * ─────────────────────────────────────────────────────────── */
#define RGBLED_NUM 58

/* Effects are limited to the ones that light the whole chain ONE colour. The
 * slave protocol is a single "set all LEDs" register (255), so per-LED effects
 * (swirl, knight, snake, static gradient) would collapse to whatever colour
 * LED 0 happens to hold and just look broken. Breathing and rainbow mood are
 * global by construction and survive the round trip intact. */
#define RGBLIGHT_EFFECT_BREATHING
#define RGBLIGHT_EFFECT_RAINBOW_MOOD

#include "users/tcy/stm32/config.h"
