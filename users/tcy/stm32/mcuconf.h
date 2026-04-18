#pragma once

/* ============================================================
 * mcuconf.h — STM32F072 peripheral clock enable/config
 * ============================================================ */

#include_next <mcuconf.h>

/* ── I2C ────────────────────────────────────────────────────
 * I2C1 — MANDATORY for Azoteq IQS572 trackpad
 *   Pins: PB6 (SCL) / PB7 (SDA) — R4/R5 pull-ups on PCB
 *
 * I2C2 — MANDATORY for MCP23017 left half
 *   Pins: PB10 (SCL) / PB11 (SDA) — R6/R7 pull-ups on PCB
 *   NOTE: PB10/PB11 are also PS/2 CLK/DAT when PS2_PINSET 2.
 *         I2C2 and PS2_PINSET 2 CANNOT be used simultaneously.
 *         Use PS2_PINSET 1 (PB8/PB9) when I2C2 is needed.
 * ─────────────────────────────────────────────────────────── */
#undef STM32_I2C_USE_I2C1
#define STM32_I2C_USE_I2C1 TRUE

#undef STM32_I2C_USE_I2C2
#define STM32_I2C_USE_I2C2 TRUE

// I2C2 DMA streams (required for async I2C2 operation)
#undef STM32_I2C_I2C2_RX_DMA_STREAM
#define STM32_I2C_I2C2_RX_DMA_STREAM STM32_DMA_STREAM_ID(1, 5)

#undef STM32_I2C_I2C2_TX_DMA_STREAM
#define STM32_I2C_I2C2_TX_DMA_STREAM STM32_DMA_STREAM_ID(1, 4)

#undef STM32_I2C_I2C2_DMA_PRIORITY
#define STM32_I2C_I2C2_DMA_PRIORITY 1

// IRQ priority: must be 3 on F0 chips (range is 0-3)
#undef STM32_I2C_I2C2_IRQ_PRIORITY
#define STM32_I2C_I2C2_IRQ_PRIORITY 3

/* ── SPI ────────────────────────────────────────────────────
 * SPI1 — MANDATORY for PMW3360 trackball
 *   Pins: PA4 (CS) / PA5 (SCK) / PA6 (MISO) / PA7 (MOSI)
 * ─────────────────────────────────────────────────────────── */
#undef STM32_SPI_USE_SPI1
#define STM32_SPI_USE_SPI1 TRUE

#define STM32_SPI_SPI1_RX_DMA_STREAM STM32_DMA_STREAM_ID(1, 2)
#define STM32_SPI_SPI1_TX_DMA_STREAM STM32_DMA_STREAM_ID(1, 3)
#define STM32_SPI_SPI1_DMA_PRIORITY 1

/* ── PWM ────────────────────────────────────────────────────
 * TIM1 — MANDATORY for PKLCS1212E4001 audio buzzer
 *   Pin: PA8 (TIM1_CH1, AF2)
 *   Driver: PWMD1, Channel: 1
 * ─────────────────────────────────────────────────────────── */
#undef STM32_PWM_USE_TIM1
#define STM32_PWM_USE_TIM1 TRUE

/* ── EXTI ────────────────────────────────────────────────────
 * MANDATORY for PS/2 trackpoint CLK interrupt
 *
 * PS2_PINSETS 1 = PB8  = EXTI8  → uncomment EXTI8,  comment EXTI10
 * PS2_PINSETS 2 = PB10 = EXTI10 → uncomment EXTI10, comment EXTI8
 * PS2_PINSETS 3 = PA4  (CLK) / PA5  (DAT)  — SPI1 pins, not 5V tolerant
 *
 * Must match PS2_PINSET in config.h.
 * ─────────────────────────────────────────────────────────── */
#undef STM32_EXTI_REQUIRED
#define STM32_EXTI_REQUIRED TRUE

/* ── PS/2 EXTI lines — must match PS2_PINSETS in config.h ── */
#undef STM32_EXTI_USE_EXTI8
#define STM32_EXTI_USE_EXTI8  TRUE   /* pinset 1 — PB8  */

#undef STM32_EXTI_USE_EXTI10
#define STM32_EXTI_USE_EXTI10 TRUE   /* pinset 2 — PB10 */

#undef STM32_EXTI_USE_EXTI4
#define STM32_EXTI_USE_EXTI4  TRUE   /* pinset 3 — PA4  */

/* ── SERIAL ─────────────────────────────────────────────────
 * USART3 disabled — it shares PB10/PB11 with I2C2.
 * ─────────────────────────────────────────────────────────── */
#undef STM32_SERIAL_USE_USART3
#define STM32_SERIAL_USE_USART3 FALSE

#undef STM32_SERIAL_USART3_PRIORITY
#define STM32_SERIAL_USART3_PRIORITY 1
