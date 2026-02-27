#pragma once

/* ============================================================
 * halconf.h — ChibiOS HAL driver enable/disable
 *
 * IMPORTANT: #include_next MUST be first so our overrides
 * properly replace the defaults from the QMK base file.
 * ============================================================ */

#include_next <halconf.h>

/* ── I2C ────────────────────────────────────────────────────
 * MANDATORY for:
 *   - Azoteq IQS572 trackpad  (I2C1 on PB6/PB7)
 *   - MCP23017 left half      (I2C2 on PB10/PB11)
 * ─────────────────────────────────────────────────────────── */
#undef HAL_USE_I2C
#define HAL_USE_I2C TRUE

/* ── SPI ────────────────────────────────────────────────────
 * MANDATORY for:
 *   - PMW3360 trackball       (SPI1 on PA4/PA5/PA6/PA7)
 * ─────────────────────────────────────────────────────────── */
#undef HAL_USE_SPI
#define HAL_USE_SPI TRUE

#undef SPI_USE_WAIT
#define SPI_USE_WAIT TRUE

/* ── PWM ────────────────────────────────────────────────────
 * MANDATORY for:
 *   - PKLCS1212E4001 buzzer   (TIM1_CH1 on PA8)
 * ─────────────────────────────────────────────────────────── */
#undef HAL_USE_PWM
#define HAL_USE_PWM TRUE

/* ── PAL ────────────────────────────────────────────────────
 * MANDATORY for:
 *   - PS/2 trackpoint EXTI interrupt (CLK falling edge)
 *   - palSetLineCallback / palEnableLineEvent
 * ─────────────────────────────────────────────────────────── */
#undef HAL_USE_PAL
#define HAL_USE_PAL TRUE

#undef PAL_USE_CALLBACKS
#define PAL_USE_CALLBACKS TRUE

#undef PAL_USE_WAIT
#define PAL_USE_WAIT TRUE

/* ── SERIAL ─────────────────────────────────────────────────
 * Disabled: USART3 is on PB10/PB11 which are used for I2C2.
 * ─────────────────────────────────────────────────────────── */
#undef HAL_USE_SERIAL
#define HAL_USE_SERIAL FALSE

#undef STM32_SERIAL_USE_USART3
#define STM32_SERIAL_USE_USART3 FALSE
