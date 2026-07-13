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

/* ── Azoteq IQS572 trackpad ─────────────────────────────────
 * MANDATORY: model define selects resolution + finger tracking
 * ─────────────────────────────────────────────────────────── */
#define AZOTEQ_IQS5XX_TPS65
#define AZOTEQ_IQS5XX_ROTATION_90

/* ── PS/2 trackpoint (custom driver in ps2.c) ───────────────
 * MANDATORY: PS2_PINSET selects the CLK/DAT pin pair.
 *
 *   PS2_PINSET 1 → PB8  (CLK) / PB9  (DAT)  free pins, no conflict
 *   PS2_PINSET 2 → PB10 (CLK) / PB11 (DAT)  R6/R7 pull-ups, conflicts I2C2
 *
 * Must also match STM32_EXTI_USE_EXTIx in mcuconf.h.
 * ─────────────────────────────────────────────────────────── */
#define PS2_PINSETS { 1 }

#include "users/tcy/stm32/config.h"
