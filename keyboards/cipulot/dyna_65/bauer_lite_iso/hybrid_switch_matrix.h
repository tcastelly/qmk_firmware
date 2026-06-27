/* Copyright 2026 Cipulot
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "matrix.h"
#include "eeconfig.h"
#include "util.h"
#include "socd_cleaner.h"

// Rescale mode enumeration
typedef enum {
    // clang-format off
    RESCALE_MODE_APC = 0, // APC
    RESCALE_MODE_RT = 1,  // RT
    RESCALE_MODE_ALL = 2  // All thresholds
    // clang-format on
} rescale_mode_t;

// Unified update mode for key field updates
typedef enum {
    // clang-format off
    HYBRID_UPDATE_RUNTIME_ONLY  = 0, // Update runtime only
    HYBRID_UPDATE_SHARED_OFFSET = 1, // Runtime + EEPROM share same field offset
    HYBRID_UPDATE_DUAL_OFFSET   = 2  // Runtime + EEPROM use different field offsets
    // clang-format on
} update_mode_t;

// Key coordinate structure for ghost key detection
typedef struct {
    uint8_t row;
    uint8_t col;
} KeyCoord;

// Runtime key state structure definitions
typedef struct PACKED {
    uint8_t  switch_type;                // 0: EC, 1: MX
    uint8_t  actuation_mode;             // 0: APC, 1: Rapid Trigger
    uint16_t apc_actuation_threshold;    // APC actuation threshold
    uint16_t apc_release_threshold;      // APC release threshold
    uint16_t rt_initial_deadzone_offset; // RT initial deadzone offset
    uint8_t  rt_actuation_offset;        // RT actuation offset
    uint8_t  rt_release_offset;          // RT release offset

    uint16_t rescaled_apc_actuation_threshold;    // Rescaled APC actuation threshold
    uint16_t rescaled_apc_release_threshold;      // Rescaled APC release threshold
    uint16_t rescaled_rt_initial_deadzone_offset; // Rescaled RT initial deadzone offset
    uint8_t  rescaled_rt_actuation_offset;        // Rescaled RT actuation offset
    uint8_t  rescaled_rt_release_offset;          // Rescaled RT release offset

    uint16_t noise_floor;                   // Real time noise floor
    uint16_t extremum;                      // Extremum value for RT
    bool     bottoming_calibration_starter; // Flag to start bottoming calibration
    uint16_t bottoming_calibration_reading; // Bottoming reading for rescaling
} runtime_key_state_t;

// EEPROM key state structure definitions (reduced parameters to save space, missing values are calculated at runtime)
typedef struct PACKED {
    uint8_t  switch_type;                // 0: EC, 1: MX
    uint8_t  actuation_mode;             // 0: APC, 1: Rapid Trigger
    uint16_t apc_actuation_threshold;    // APC actuation threshold
    uint16_t apc_release_threshold;      // APC release threshold
    uint16_t rt_initial_deadzone_offset; // RT initial deadzone offset
    uint8_t  rt_actuation_offset;        // RT actuation offset
    uint8_t  rt_release_offset;          // RT release offset

    uint16_t bottoming_calibration_reading; // Bottoming reading for rescaling
} eeprom_key_state_t;

// Runtime configuration structure definitions
typedef struct PACKED {
    bool                bottoming_calibration;                       // Runtime board level flag for bottoming calibration
    runtime_key_state_t runtime_key_state[MATRIX_ROWS][MATRIX_COLS]; // Per-key runtime state
} runtime_hybrid_config_t;

// EEPROM configuration structure definitions
typedef struct PACKED {
    eeprom_key_state_t eeprom_key_state[MATRIX_ROWS][MATRIX_COLS]; // Per-key EEPROM state
    socd_cleaner_t     eeprom_socd_opposing_pairs[4];              // SOCD cleaner pairs
} eeprom_hybrid_config_t;

// Compile-time check for EECONFIG_KB_DATA_SIZE
// EECONFIG_KB_DATA_SIZE = 20 + (12 * MATRIX_ROWS * MATRIX_COLS)
_Static_assert(sizeof(eeprom_hybrid_config_t) == EECONFIG_KB_DATA_SIZE, "Mismatch in keyboard EECONFIG stored data");

// Extern declarations
extern eeprom_hybrid_config_t  eeprom_hybrid_config;  // EEPROM configuration instance
extern runtime_hybrid_config_t runtime_hybrid_config; // Runtime configuration instance
// Runtime SOCD cleaner pairs
// For now it can't be part of runtime_hybrid_config_t due to how the submodule checks for the existance of the structure
extern socd_cleaner_t socd_opposing_pairs[4];

// Function prototypes
void init_row(void);
void disable_unused_row(uint8_t row);
void init_amux(void);
void select_amux_channel(uint8_t channel, uint8_t col);
void disable_unused_amux(uint8_t channel);
void charge_capacitor(uint8_t row);
void discharge_capacitor(void);

int      hybrid_init(void);
void     hybrid_noise_floor_calibration(void);
bool     hybrid_matrix_scan(matrix_row_t current_matrix[]);
uint16_t hybrid_readkey_raw(uint8_t channel, uint8_t row, uint8_t col);
bool     hybrid_update_key(matrix_row_t *current_row, uint8_t row, uint8_t col, uint16_t sw_value);
bool     hybrid_update_key_apc(matrix_row_t *current_row, uint8_t col, uint16_t sw_value, runtime_key_state_t *key_runtime, bool pressed);
bool     hybrid_update_key_rt(matrix_row_t *current_row, uint8_t col, uint16_t sw_value, runtime_key_state_t *key_runtime, bool pressed);
bool     hybrid_update_key_mx(matrix_row_t *current_row, uint8_t col, uint16_t sw_value, bool pressed);
void     bulk_rescale_key_thresholds(runtime_key_state_t *key_runtime, eeprom_key_state_t *key_eeprom, rescale_mode_t mode);
void     update_keys_field(update_mode_t mode, size_t runtime_offset, size_t eeprom_offset, const void *value, size_t field_size);
bool     forms_square(KeyCoord key1, KeyCoord key2, KeyCoord key3, KeyCoord key4);
void     hybrid_print_matrix(void);
uint16_t rescale(uint16_t x, uint16_t out_min, uint16_t out_max);

#ifdef UNUSED_POSITIONS_LIST
bool is_unused_position(uint8_t row, uint8_t col);
#endif

#ifdef SPLIT_KEYBOARD
void via_cmd_slave_handler(uint8_t m2s_size, const void *m2s_buffer, uint8_t s2m_size, void *s2m_buffer);
#endif
