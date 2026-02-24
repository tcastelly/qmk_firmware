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

// --- MCP23017 configuration (left half) ---
/* i2c settings */

// MCP as default 
// #define I2C_DRIVER I2CD2
// #define I2C1_SCL_PIN B10
// #define I2C1_SDA_PIN B11
// #define I2C1_TIMINGR_PRESC 2U
// #define I2C1_TIMINGR_SCLDEL 1U
// #define I2C1_TIMINGR_SDADEL 0U
// #define I2C1_TIMINGR_SCLH 9U
// #define I2C1_TIMINGR_SCLL 26U
// #define I2C1_SCL_PAL_MODE 1
// #define I2C1_SDA_PAL_MODE 1

// Trackpad as default
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

#define F_SCL 100000
                                  
#define MATRIX_ROWS 8
#define MATRIX_COLS 12

// Right halve (on MCU)
#define MATRIX_ROW_PINS_MCU { A0, A1, A2, A3 }
#define MATRIX_COL_PINS_MCU { B0, B1, B2, B3, B4, B5 }

// Left halve (on MCP23017)
#define MATRIX_ROW_PINS_MCP { B0, B1, B2, B3 }
#define MATRIX_COL_PINS_MCP { A0, A1, A2, A3, A4, A5 }
                                                        
#define MATRIX_ROW_PINS { B0, B1, B2, B3, A0, A1, A2, A3 }
#define MATRIX_COL_PINS { A0, A1, A2, A3, A4, A5, B0, B1, B2, B3, B4, B5 }

#define DIODE_DIRECTION COL2ROW

# define AZOTEQ_IQS5XX_TPS43

// PS/2 Configuration
// #define PS2_PINSET 1   // → PB8/PB9
#define PS2_PINSET 2   // → PB10/PB11
#define PS2_DATA_PIN  B11
#define PS2_CLOCK_PIN B10
#define PS2_MOUSE_DEBUG 

// These are still good to keep for the manual logic
#define PS2_DELAY 40 
#define POINTING_DEVICE_TASK_EVERY_MS 1
