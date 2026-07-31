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

/* ── Cirque Gen4 (TM105065) I2C trackpad ────────────────────
 * EXTRA pointing device (custom driver in
 * users/tcy/drivers/sensors/cirque_gen4). Shares I2C1 (PB6/PB7) with the
 * Azoteq trackpad — bus 1 = QMK i2c_master. Distinct slave address (0x2C)
 * so both live on the same bus.
 *
 *   CIRQUE_GEN4_I2C_BUS 1 → QMK i2c_master (I2C1, config_i2c1.h)
 *   CIRQUE_GEN4_I2C_BUS 2 → ChibiOS I2CD2 (i2c2_handler.c, shared with MCP)
 *
 * DR (Data Ready) is pin 7 of the pad's 10-pin FFC — the pad next to SCL, on
 * the side away from SDA (order: ... 7=DR 8=SCL 9=SDA 10=GND).
 *
 * It matters MORE on this board than on a bare test board: without it the
 * driver reads blind on every poll, and an I2C-HID device with nothing to
 * report stretches the clock while it answers. QMK's I2C is blocking, so that
 * stalls the whole main loop — and here the bus is already shared with the
 * Azoteq. Measured on try_cirquegen4, DR took idle traffic from 100 reads/sec
 * to zero.
 *
 * A10 is free: RGB is not compiled on this board, so the WS2812_DI_PIN A10 in
 * users/tcy/stm32/config.h claims nothing. Note it is only RESERVED for RGB by
 * convention — if this board ever gets LEDs, one of the two must move. B13-B15
 * are unclaimed alternatives.
 *
 * Power the pad from 3.3V (spec is 3.0-5.5V) so DR and the I2C lines stay at
 * 3.3V logic.
 *
 * If the cursor never moves, or only responds when idle, the interrupt
 * polarity is inverted: add #define CIRQUE_GEN4_DR_ACTIVE_LOW 0
 * ─────────────────────────────────────────────────────────── */
#define CIRQUE_GEN4_I2C_BUS 1
#define CIRQUE_GEN4_ADDR    0x2A   /* confirmed by boot I2C scan (no addr resistor); 0x2C is the alt */
#define CIRQUE_GEN4_DR_PIN  A10

/* ── PS/2 trackpoint (custom driver in ps2.c) ───────────────
 * MANDATORY: PS2_PINSET selects the CLK/DAT pin pair.
 *
 *   PS2_PINSET 1 → PB8 (CLK) / PB9  (DAT)  ⚠ PB9 output driver dead (ESD)
 *   PS2_PINSET 2 → PB10(CLK) / PB11 (DAT)  shares pins with I2C2/MCP
 *   PS2_PINSET 4 → PB8 (CLK) / PB12 (DAT)  ← in use
 *
 * PB9's output driver is dead, so DATA can't be driven there. Fix without new
 * parts: trackpoint stays on the original pads (CLK→PB8/R8, DATA→PB9/R9), a
 * bridge wire ties PB9→PB12, and the driver uses the healthy PB12 for DATA.
 * Both existing pull-ups (R8/R9) reused; PB10/PB11 stay free for I2C2/MCP.
 * CLK on PB8 = EXTI8 (already declared in mcuconf); PB12 needs no EXTI.
 * ─────────────────────────────────────────────────────────── */
#define PS2_PINSETS { 4 }

#include "users/tcy/stm32/config.h"
