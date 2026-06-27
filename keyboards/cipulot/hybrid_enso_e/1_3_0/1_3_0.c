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

#include "hybrid_switch_matrix.h"
#include "keyboard.h"

#ifdef SPLIT_KEYBOARD
#    include "transactions.h"
#endif

// EEPROM default initialization
void eeconfig_init_kb(void) {
    // Set default board-wide configuration values
    eeprom_hybrid_config.board_mode                       = DEFAULT_BOARD_MODE;
    eeprom_hybrid_config.board_switch_type                = DEFAULT_SWITCH_TYPE;
    eeprom_hybrid_config.board_apc_actuation_threshold    = DEFAULT_APC_ACTUATION_LEVEL;
    eeprom_hybrid_config.board_apc_release_threshold      = DEFAULT_APC_RELEASE_LEVEL;
    eeprom_hybrid_config.board_rt_initial_deadzone_offset = DEFAULT_RT_INITIAL_DEADZONE_OFFSET;
    eeprom_hybrid_config.board_rt_actuation_offset        = DEFAULT_RT_ACTUATION_OFFSET;
    eeprom_hybrid_config.board_rt_release_offset          = DEFAULT_RT_RELEASE_OFFSET;

    // Initialize per-key EEPROM state with default values
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            // Get pointer to key state in EEPROM
            eeprom_key_state_t *key_eeprom = &eeprom_hybrid_config.eeprom_key_state[row][col];
            // Set default values
            key_eeprom->switch_type                   = DEFAULT_SWITCH_TYPE;
            key_eeprom->actuation_mode                = DEFAULT_ACTUATION_MODE;
            key_eeprom->apc_actuation_threshold       = DEFAULT_APC_ACTUATION_LEVEL;
            key_eeprom->apc_release_threshold         = DEFAULT_APC_RELEASE_LEVEL;
            key_eeprom->rt_initial_deadzone_offset    = DEFAULT_RT_INITIAL_DEADZONE_OFFSET;
            key_eeprom->rt_actuation_offset           = DEFAULT_RT_ACTUATION_OFFSET;
            key_eeprom->rt_release_offset             = DEFAULT_RT_RELEASE_OFFSET;
            key_eeprom->bottoming_calibration_reading = DEFAULT_BOTTOMING_CALIBRATION_READING;
        }
    }

    // Initialize the SOCD cleaner pairs
    const struct {
        uint16_t key1;
        uint16_t key2;
    } socd_pairs[] = {
        {KC_A, KC_D},
        {KC_W, KC_S},
        {KC_Z, KC_X},
        {KC_LEFT, KC_RIGHT},
    };

    // Copy default SOCD pairs to EEPROM
    for (int i = 0; i < 4; i++) {
        eeprom_hybrid_config.eeprom_socd_opposing_pairs[i].keys[0]    = socd_pairs[i].key1;
        eeprom_hybrid_config.eeprom_socd_opposing_pairs[i].keys[1]    = socd_pairs[i].key2;
        eeprom_hybrid_config.eeprom_socd_opposing_pairs[i].resolution = SOCD_CLEANER_OFF;
        eeprom_hybrid_config.eeprom_socd_opposing_pairs[i].held[0]    = false;
        eeprom_hybrid_config.eeprom_socd_opposing_pairs[i].held[1]    = false;
    }

    // Write to EEPROM entire datablock
    eeconfig_update_kb_datablock(&eeprom_hybrid_config, 0, EECONFIG_KB_DATA_SIZE);

    // Call user initialization
    eeconfig_init_user();
}

// Keyboard post-initialization
void keyboard_post_init_kb(void) {
    // Read the EEPROM data block
    eeconfig_read_kb_datablock(&eeprom_hybrid_config, 0, EECONFIG_KB_DATA_SIZE);

    // Initialize runtime board-wide configuration values from EEPROM
    runtime_hybrid_config.board_mode                       = eeprom_hybrid_config.board_mode;
    runtime_hybrid_config.board_switch_type                = eeprom_hybrid_config.board_switch_type;
    runtime_hybrid_config.board_actuation_mode             = eeprom_hybrid_config.board_actuation_mode;
    runtime_hybrid_config.board_apc_actuation_threshold    = eeprom_hybrid_config.board_apc_actuation_threshold;
    runtime_hybrid_config.board_apc_release_threshold      = eeprom_hybrid_config.board_apc_release_threshold;
    runtime_hybrid_config.board_rt_initial_deadzone_offset = eeprom_hybrid_config.board_rt_initial_deadzone_offset;
    runtime_hybrid_config.board_rt_actuation_offset        = eeprom_hybrid_config.board_rt_actuation_offset;
    runtime_hybrid_config.board_rt_release_offset          = eeprom_hybrid_config.board_rt_release_offset;
    runtime_hybrid_config.bottoming_calibration            = false;

    // Initialize per-key runtime state from EEPROM
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            // Get pointer to key state in runtime and EEPROM
            runtime_key_state_t *key_runtime = &runtime_hybrid_config.runtime_key_state[row][col];
            eeprom_key_state_t  *key_eeprom  = &eeprom_hybrid_config.eeprom_key_state[row][col];

            // Copy from EEPROM to runtime
            key_runtime->switch_type                   = key_eeprom->switch_type;
            key_runtime->actuation_mode                = key_eeprom->actuation_mode;
            key_runtime->apc_actuation_threshold       = key_eeprom->apc_actuation_threshold;
            key_runtime->apc_release_threshold         = key_eeprom->apc_release_threshold;
            key_runtime->rt_initial_deadzone_offset    = key_eeprom->rt_initial_deadzone_offset;
            key_runtime->rt_actuation_offset           = key_eeprom->rt_actuation_offset;
            key_runtime->rt_release_offset             = key_eeprom->rt_release_offset;
            key_runtime->bottoming_calibration_reading = key_eeprom->bottoming_calibration_reading;
            key_runtime->extremum                      = DEFAULT_EXTREMUM;
            key_runtime->bottoming_calibration_starter = DEFAULT_CALIBRATION_STARTER;

            // Rescale all key thresholds based on noise floor and bottoming reading
            bulk_rescale_key_thresholds(key_runtime, key_eeprom, RESCALE_MODE_ALL);
        }
    }
    // Register RPC handler for VIA commands if split keyboard
#ifdef SPLIT_KEYBOARD
    transaction_register_rpc(RPC_ID_VIA_CMD, via_cmd_slave_handler);
#endif

    // Copy SOCD cleaner pairs to runtime instance
    memcpy(socd_opposing_pairs, eeprom_hybrid_config.eeprom_socd_opposing_pairs, sizeof(socd_opposing_pairs));

    // Call user post-initialization
    keyboard_post_init_user();
}
