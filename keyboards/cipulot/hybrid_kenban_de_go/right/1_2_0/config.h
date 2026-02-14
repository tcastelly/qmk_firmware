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

// #define UNUSED_POSITIONS_LIST { {6, 0}, {7, 0}, {7, 1}, {7, 2}, {7, 6} }

#define MATRIX_ROW_PINS {A10, B14, B3, B1, C13, C13, C13, C13}

#define AMUX_COUNT 1
#define AMUX_MAX_COLS_COUNT 7

#define AMUX_EN_PINS {B9}

#define AMUX_SEL_PINS {B8, B5, B4}

#define AMUX_COL_CHANNELS_SIZES {7}

#define AMUX_0_COL_CHANNELS {0, 1, 3, 2, 7, 6, 4}

#define AMUX_COL_CHANNELS AMUX_0_COL_CHANNELS

#define DISCHARGE_PIN A2
#define ANALOG_PORT A3

#ifdef POINTING_DEVICE_ENABLE
#    define SPI_DRIVER SPID1
#    define SPI_SCK_PIN A5
#    define SPI_SCK_PAL_MODE 5
#    define SPI_MISO_PIN A6
#    define SPI_MISO_PAL_MODE 5
#    define SPI_MOSI_PIN A7
#    define SPI_MOSI_PAL_MODE 5
#    define POINTING_DEVICE_CS_PIN B8
#endif
