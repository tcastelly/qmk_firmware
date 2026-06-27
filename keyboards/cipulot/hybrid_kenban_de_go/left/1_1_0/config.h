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

// #define UNUSED_POSITIONS_LIST { {2, 6}, {3, 0}, {3, 4}, {3, 5}, {3, 6} }

#define MATRIX_ROW_PINS {B15, B14, B13, A1}

#define AMUX_COUNT 1
#define AMUX_MAX_COLS_COUNT 7

#define AMUX_EN_PINS {A5}

#define AMUX_SEL_PINS {A4, A3, A2}

#define AMUX_COL_CHANNELS_SIZES {7}

#define AMUX_0_COL_CHANNELS {2, 1, 0, 3, 6, 4, 7}

#define AMUX_COL_CHANNELS AMUX_0_COL_CHANNELS

#define DISCHARGE_PIN A7
#define ANALOG_PORT B0
