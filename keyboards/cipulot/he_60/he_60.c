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

#include "he_switch_matrix.h"

void eeconfig_init_kb(void) {
    // Default values
    eeprom_he_config.actuation_mode             = DEFAULT_ACTUATION_MODE;
    eeprom_he_config.apc_actuation_threshold    = DEFAULT_APC_ACTUATION_LEVEL;
    eeprom_he_config.apc_release_threshold      = DEFAULT_APC_RELEASE_LEVEL;
    eeprom_he_config.rt_initial_deadzone_offset = DEFAULT_RT_INITIAL_DEADZONE_OFFSET;
    eeprom_he_config.rt_actuation_offset        = DEFAULT_RT_ACTUATION_OFFSET;
    eeprom_he_config.rt_release_offset          = DEFAULT_RT_RELEASE_OFFSET;

    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            eeprom_he_config.bottoming_calibration_reading[row][col] = DEFAULT_BOTTOMING_CALIBRATION_READING;
        }
    }
    // Write default value to EEPROM now
    eeconfig_update_kb_datablock(&eeprom_he_config, 0, EECONFIG_KB_DATA_SIZE);
}

// On Keyboard startup
void keyboard_post_init_kb(void) {
    // Read custom menu variables from memory
    eeconfig_read_kb_datablock(&eeprom_he_config, 0, EECONFIG_KB_DATA_SIZE);

    // Set runtime values to EEPROM values
    he_config.actuation_mode             = eeprom_he_config.actuation_mode;
    he_config.apc_actuation_threshold    = eeprom_he_config.apc_actuation_threshold;
    he_config.apc_release_threshold      = eeprom_he_config.apc_release_threshold;
    he_config.rt_initial_deadzone_offset = eeprom_he_config.rt_initial_deadzone_offset;
    he_config.rt_actuation_offset        = eeprom_he_config.rt_actuation_offset;
    he_config.rt_release_offset          = eeprom_he_config.rt_release_offset;
    he_config.bottoming_calibration      = false;
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            he_config.bottoming_calibration_starter[row][col]       = true;
            he_config.bottoming_calibration_reading[row][col]       = eeprom_he_config.bottoming_calibration_reading[row][col];
            he_config.rescaled_apc_actuation_threshold[row][col]    = rescale(he_config.apc_actuation_threshold, he_config.noise_floor[row][col], eeprom_he_config.bottoming_calibration_reading[row][col]);
            he_config.rescaled_apc_release_threshold[row][col]      = rescale(he_config.apc_release_threshold, he_config.noise_floor[row][col], eeprom_he_config.bottoming_calibration_reading[row][col]);
            he_config.rescaled_rt_initial_deadzone_offset[row][col] = rescale(he_config.rt_initial_deadzone_offset, he_config.noise_floor[row][col], eeprom_he_config.bottoming_calibration_reading[row][col]);
            he_config.rescaled_rt_actuation_offset[row][col]        = rescale(he_config.rt_actuation_offset, he_config.noise_floor[row][col], eeprom_he_config.bottoming_calibration_reading[row][col]);
            he_config.rescaled_rt_release_offset[row][col]          = rescale(he_config.rt_release_offset, he_config.noise_floor[row][col], eeprom_he_config.bottoming_calibration_reading[row][col]);
        }
    }
}
