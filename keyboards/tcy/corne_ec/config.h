/* Corne EC F072 — split 3x6+3 EC keyboard, STM32F072CBT6 on each half.
 *
 * Runs the unmodified cipulot common EC driver (keyboards/cipulot/common),
 * hardware topology per half: 1x 74HC4051 analog mux -> OPA350 op-amp buffer
 * -> ADC, one discharge pin, 4 drive (row) lines. Same electrical design as
 * ssbb/tako rev0 / cipulot ec_split_60, retargeted to the F072 (Cortex-M0).
 *
 * Both halves are electrically identical and run the SAME firmware image;
 * handedness comes from the C13 strap (high = left, low = right).
 */
#pragma once

#define MATRIX_ROWS 8
#define MATRIX_COLS 6

/* Corne thumb row only has 3 keys per half: left thumbs sit in [3,3..5],
 * right thumbs in [7,3..5] (right half columns are mirrored, col 0 = pinky).
 * The other 6 positions don't exist — skip them in the EC scan. */
#define UNUSED_POSITIONS_LIST {{3, 0}, {3, 1}, {3, 2}, {7, 0}, {7, 1}, {7, 2}}

/* The cipulot driver loops row_pins[MATRIX_ROWS]; only the first
 * MATRIX_ROWS/2 rows exist per half. Pad with B2 (kept low, reads noise
 * floor -> never a press) like ec_split_60 pads with C13. B2 is therefore
 * sacrificed: it gets strobed during the dummy scans, don't wire it. */
#define MATRIX_ROW_PINS {B12, B13, B14, B15, B2, B2, B2, B2}

#define AMUX_COUNT 1
#define AMUX_MAX_COLS_COUNT 6

#define AMUX_EN_PINS {B0}

#define AMUX_SEL_PINS {B3, B4, B5}

#define AMUX_COL_CHANNELS_SIZES {6}

/* Straight col -> 74HC4051 channel mapping; reorder to match PCB routing. */
#define AMUX_0_COL_CHANNELS {0, 1, 2, 3, 4, 5}

#define AMUX_COL_CHANNELS AMUX_0_COL_CHANNELS

#define DISCHARGE_PIN A2
#define ANALOG_PORT A3 /* ADC1_IN3 */

#define DEFAULT_ACTUATION_MODE 0
#define DEFAULT_APC_ACTUATION_LEVEL 550
#define DEFAULT_APC_RELEASE_LEVEL 500
#define DEFAULT_RT_INITIAL_DEADZONE_OFFSET DEFAULT_APC_ACTUATION_LEVEL
#define DEFAULT_RT_ACTUATION_OFFSET 40
#define DEFAULT_RT_RELEASE_OFFSET 40
#define DEFAULT_EXTREMUM 0
#define EXPECTED_NOISE_FLOOR 0
#define NOISE_FLOOR_THRESHOLD 25
#define BOTTOMING_CALIBRATION_THRESHOLD 100
#define DEFAULT_NOISE_FLOOR_SAMPLING_COUNT 30
#define DEFAULT_BOTTOMING_CALIBRATION_READING 1023
#define DEFAULT_CALIBRATION_STARTER true

#define CHARGE_TIME 1
#define DISCHARGE_TIME 10

#define EECONFIG_KB_DATA_SIZE (20 + (11 * MATRIX_ROWS * MATRIX_COLS))

/* Split transport: USART1 full duplex over the TRRS/interconnect.
 * F072 USART1 is AF1 on PA9/PA10 (QMK's PAL-mode default of 7 is F4-only). */
#define SERIAL_USART_DRIVER SD1
#define SERIAL_USART_FULL_DUPLEX
#define SERIAL_USART_TX_PIN A9
#define SERIAL_USART_RX_PIN A10
#define SERIAL_USART_TX_PAL_MODE 1
#define SERIAL_USART_RX_PAL_MODE 1

#define SPLIT_TRANSACTION_IDS_KB RPC_ID_VIA_CMD

/* ── Reserved peripherals (users/tcy STM32F072 conventions) ─────────────
 * These pins are deliberately NOT used by the EC matrix so a pointing
 * device can be added later without touching the board files:
 *   I2C1 : B6 (SCL) / B7 (SDA)  — e.g. Azoteq IQS5xx trackpad
 *   SPI1 : A4 (CS) / A5 (SCK) / A6 (MISO) / A7 (MOSI) — e.g. PMW3360
 *   Audio: A8 (TIM1_CH1) — piezo buzzer
 * I2C1/SPI1 are already enabled in halconf.h/mcuconf.h. */
#define I2C_DRIVER I2CD1
#define I2C1_SCL_PIN B6
#define I2C1_SDA_PIN B7
#define I2C1_SCL_PAL_MODE 1
#define I2C1_SDA_PAL_MODE 1
#define I2C1_TIMINGR_PRESC 0x00U
#define I2C1_TIMINGR_SCLDEL 0x03U
#define I2C1_TIMINGR_SDADEL 0x01U
#define I2C1_TIMINGR_SCLH 0x03U
#define I2C1_TIMINGR_SCLL 0x09U

#define SPI_DRIVER SPID1
#define SPI_SCK_PIN A5
#define SPI_MOSI_PIN A7
#define SPI_MISO_PIN A6
#define SPI_SCK_PAL_MODE 0
#define SPI_MOSI_PAL_MODE 0
#define SPI_MISO_PAL_MODE 0
