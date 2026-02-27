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

/* ============================================================
 * MCU: STM32F072CBT6 (Cortex-M0, 48MHz, 48-pin LQFP)
 * ============================================================ */

/* ── Matrix ─────────────────────────────────────────────────
 * Right half: 4 rows × 6 cols directly on MCU
 * Left half:  4 rows × 6 cols via MCP23017 over I2C2
 * ─────────────────────────────────────────────────────────── */
#define MATRIX_ROWS 8
#define MATRIX_COLS 12

#define MATRIX_ROW_PINS_MCU { A0, A1, A2, A3 }
#define MATRIX_COL_PINS_MCU { B0, B1, B2, B3, B4, B5 }

#define MATRIX_ROW_PINS_MCP { B0, B1, B2, B3 }
#define MATRIX_COL_PINS_MCP { A0, A1, A2, A3, A4, A5 }

#define MATRIX_ROW_PINS { B0, B1, B2, B3, A0, A1, A2, A3 }
#define MATRIX_COL_PINS { A0, A1, A2, A3, A4, A5, B0, B1, B2, B3, B4, B5 }

#define DIODE_DIRECTION COL2ROW


/* ── I2C1 — Azoteq IQS572 trackpad ─────────────────────────
 * MANDATORY lines for I2C1:
 *   I2C_DRIVER, I2C1_SCL_PIN, I2C1_SDA_PIN,
 *   I2C1_SCL_PAL_MODE, I2C1_SDA_PAL_MODE,
 *   I2C1_TIMINGR_* (all 5 values)
 *
 * Pins: PB6 (SCL) / PB7 (SDA) — R4/R5 5.1k pull-ups on PCB
 * AF1 = I2C1 on PB6/PB7
 * ─────────────────────────────────────────────────────────── */
#define I2C_DRIVER          I2CD1
#define I2C1_SCL_PIN        B6
#define I2C1_SDA_PIN        B7
#define I2C1_SCL_PAL_MODE   1
#define I2C1_SDA_PAL_MODE   1
#define I2C1_TIMINGR_PRESC  0x00U
#define I2C1_TIMINGR_SCLDEL 0x03U
#define I2C1_TIMINGR_SDADEL 0x01U
#define I2C1_TIMINGR_SCLH   0x03U
#define I2C1_TIMINGR_SCLL   0x09U

/* ── Azoteq IQS572 trackpad ─────────────────────────────────
 * MANDATORY: model define selects resolution + finger tracking
 * ─────────────────────────────────────────────────────────── */
#define AZOTEQ_IQS5XX_TPS43


/* ── I2C2 — MCP23017 left half ──────────────────────────────
 * NOTE: I2C2 uses PB10/PB11 which conflict with PS2_PINSET 2.
 * When I2C2 is needed, set PS2_PINSET 1 (PB8/PB9) in ps2.c.
 *
 * These defines are informational — I2C2 driver is configured
 * in mcuconf.h. Uncomment if QMK needs explicit I2C2 defines.
 * ─────────────────────────────────────────────────────────── */
// #define I2C_DRIVER       I2CD2
// #define I2C2_SCL_PIN     B10
// #define I2C2_SDA_PIN     B11
// #define I2C2_SCL_PAL_MODE 1
// #define I2C2_SDA_PAL_MODE 1
// #define I2C2_TIMINGR_PRESC  2U
// #define I2C2_TIMINGR_SCLDEL 1U
// #define I2C2_TIMINGR_SDADEL 0U
// #define I2C2_TIMINGR_SCLH   9U
// #define I2C2_TIMINGR_SCLL   26U


/* ── PS/2 trackpoint (custom driver in ps2.c) ───────────────
 * MANDATORY: PS2_PINSET selects the CLK/DAT pin pair.
 *
 *   PS2_PINSET 1 → PB8  (CLK) / PB9  (DAT)  free pins, no conflict
 *   PS2_PINSET 2 → PB10 (CLK) / PB11 (DAT)  R6/R7 pull-ups, conflicts I2C2
 *
 * Must also match STM32_EXTI_USE_EXTIx in mcuconf.h.
 * ─────────────────────────────────────────────────────────── */
#define PS2_PINSET 2
#define PS2_MOUSE_DEBUG
#define PS2_DELAY  40

#define POINTING_DEVICE_TASK_EVERY_MS 1


/* ── SPI1 — PMW3360 trackball ───────────────────────────────
 * MANDATORY lines for SPI:
 *   SPI_DRIVER, SPI_SCK_PIN, SPI_MOSI_PIN, SPI_MISO_PIN
 *
 * MANDATORY for PMW3360:
 *   PMW33XX_CS_PIN, PMW33XX_CPI
 *
 * Pins: PA4 (CS) / PA5 (SCK) / PA6 (MISO) / PA7 (MOSI)
 * ─────────────────────────────────────────────────────────── */
#define SPI_DRIVER   SPID1
#define SPI_SCK_PIN  A5
#define SPI_MOSI_PIN A7
#define SPI_MISO_PIN A6

#define PMW33XX_CS_PIN  A4
#define PMW33XX_CPI     1600


/* ── Audio — PKLCS1212E4001 piezo buzzer ────────────────────
 * MANDATORY lines for PWM audio:
 *   AUDIO_PIN, AUDIO_PWM_DRIVER, AUDIO_PWM_CHANNEL,
 *   AUDIO_PWM_PAL_MODE (STM32-specific, sets AF number)
 *
 * Pin: PA8 = TIM1_CH1, AF2
 *   AUDIO_PWM_DRIVER  = PWMD[1]  ← from TIM[1]
 *   AUDIO_PWM_CHANNEL = [1]      ← from CH[1]
 *   AUDIO_PWM_PAL_MODE = [2]     ← from AF[2]
 * ─────────────────────────────────────────────────────────── */
#define AUDIO_PIN          A8
#define AUDIO_PWM_DRIVER   PWMD1
#define AUDIO_PWM_CHANNEL  1
#define AUDIO_PWM_PAL_MODE 2

#define AUDIO_VOICES
#define AUDIO_VOICE_DEFAULT butts_fader

#ifdef AUDIO_ENABLE
  #define STARTUP_SOUND E__NOTE(_E6), E__NOTE(_A6), ED_NOTE(_E7),
#endif
