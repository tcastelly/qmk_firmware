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

#define SPI_SCK_PAL_MODE  0
#define SPI_MOSI_PAL_MODE 0
#define SPI_MISO_PAL_MODE 0

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

