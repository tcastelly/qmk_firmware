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
 * If the cursor never moves, or only responds when idle, the interrupt
 * polarity is inverted: add #define CIRQUE_GEN4_DR_ACTIVE_LOW 0
 * ─────────────────────────────────────────────────────────── */
#define CIRQUE_GEN4_I2C_BUS 1
#define CIRQUE_GEN4_ADDR    0x2A   /* confirmed by boot I2C scan (no addr resistor); 0x2C is the alt */
#define CIRQUE_GEN4_DR_PIN  A10

/* ── PS/2 trackpoint (custom driver in ps2.c) ───────────────
 * MANDATORY: PS2_PINSET selects the CLK/DAT pin pair.
 *
 *   PS2_PINSET 1 → PB8 (CLK) / PB9  (DAT)
 *   PS2_PINSET 2 → PB10(CLK) / PB11 (DAT)  shares pins with I2C2/MCP
 * ─────────────────────────────────────────────────────────── */
#define PS2_PINSETS { 1 }

#include "users/tcy/stm32/config.h"
