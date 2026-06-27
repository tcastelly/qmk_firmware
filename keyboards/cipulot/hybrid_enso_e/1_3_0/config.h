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

#define MATRIX_ROWS 5
#define MATRIX_COLS 26

#define UNUSED_POSITIONS_LIST {{0, 14}, {0, 15}, {0, 16}, {0, 17}, {0, 18}, {0, 19}, {0, 20}, {0, 21}, {0, 22}, {0, 23}, {0, 24}, {0, 25}, {1, 14}, {1, 15}, {1, 16}, {1, 17}, {1, 18}, {1, 19}, {1, 20}, {1, 21}, {1, 22}, {1, 23}, {1, 24}, {1, 25}, {2, 12}, {2, 14}, {2, 15}, {2, 16}, {2, 17}, {2, 18}, {2, 19}, {2, 20}, {2, 21}, {2, 22}, {2, 23}, {2, 24}, {2, 25}, {3, 0}, {3, 12}, {3, 13}, {3, 17}, {3, 18}, {3, 19}, {3, 20}, {3, 21}, {3, 22}, {3, 23}, {3, 24}, {3, 25}, {4, 0}, {4, 1}, {4, 2}, {4, 3}, {4, 4}, {4, 5}, {4, 6}, {4, 7}, {4, 8}, {4, 9}, {4, 10}, {4, 11}, {4, 12}, {4, 13}, {4, 14}, {4, 15}, {4, 16}}

// Order iportant for special positions handling, don't change it unless you know what you're doing
#define SPECIAL_POSITIONS_LIST {{3, 14}, {4, 17}, {3, 15}, {3, 16}, {4, 18}, {4, 19}, {4, 20}, {4, 21}, {4, 22}, {4, 23}, {4, 24}, {4, 25}}

#define MATRIX_ROW_PINS {B5, B6, B7, A9, A10}

#define AMUX_COUNT 2
#define AMUX_MAX_COLS_COUNT 14

#define AMUX_EN_PINS {A8, B10}

#define AMUX_SEL_PINS {B12, B13, B15, B14}

#define AMUX_COL_CHANNELS_SIZES {14, 12}

#define AMUX_0_COL_CHANNELS {13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 0, 2, 1}
#define AMUX_1_COL_CHANNELS {9, 5, 6, 10, 11, 12, 13, 14, 1, 2, 3, 4}

#define AMUX_COL_CHANNELS AMUX_0_COL_CHANNELS, AMUX_1_COL_CHANNELS

#define DISCHARGE_PIN A2
#define ANALOG_PORT A3

#define BOARD_MODE_EC 0
#define BOARD_MODE_MX 1
#define BOARD_MODE_HYBRID 2
#define SWITCH_TYPE_EC 0
#define SWITCH_TYPE_MX 1
#define ACTUATION_MODE_APC 0
#define ACTUATION_MODE_RT 1

#define DEFAULT_BOARD_MODE BOARD_MODE_MX // 0: Full EC, 1: Full MX, 2: Hybrid
#define DEFAULT_SWITCH_TYPE SWITCH_TYPE_MX // 0: EC, 1: MX
#define DEFAULT_ACTUATION_MODE ACTUATION_MODE_APC // 0: APC, 1: Rapid Trigger
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
#define EECONFIG_KB_DATA_SIZE (31 + (12 * MATRIX_ROWS * MATRIX_COLS))
