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

#define MATRIX_ROWS 8
#define MATRIX_ROWS_PER_HAND (MATRIX_ROWS / 2)
#define MATRIX_COLS 7

#define DEFAULT_SWITCH_TYPE 0
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

#define EECONFIG_KB_DATA_SIZE (20 + (12 * MATRIX_ROWS_PER_HAND * MATRIX_COLS))

// OLED configuration
#ifdef QUANTUM_PAINTER_ENABLE
#    define DISPLAY_ADDRESS 0x3C
#    define DISPLAY_WIDTH 128
#    define DISPLAY_HEIGHT 32
#    define QUANTUM_PAINTER_TASK_THROTTLE 50
#endif

#define SPLIT_TRANSACTION_IDS_KB RPC_ID_VIA_CMD, RPC_ID_KB_EEPROM_SYNC
#define SPLIT_MAX_CONNECTION_ERRORS 20

#define EEPROM_CHUNK_SIZE 48
#define RPC_M2S_BUFFER_SIZE 48
#define RPC_S2M_BUFFER_SIZE 48
