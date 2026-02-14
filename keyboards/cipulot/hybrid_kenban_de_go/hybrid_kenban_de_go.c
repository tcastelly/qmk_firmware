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
#include "quantum.h"

#ifdef QUANTUM_PAINTER_ENABLE
#    include "graphics/display.h"
#endif

#ifdef SPLIT_KEYBOARD
#    include "transactions.h"
#endif

// EEPROM default initialization
void eeconfig_init_kb(void) {
    for (uint8_t row = 0; row < MATRIX_ROWS_PER_HAND; row++) {
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
    // Register RPC handler for VIA commands if split keyboard
#ifdef SPLIT_KEYBOARD
    transaction_register_rpc(RPC_ID_VIA_CMD, via_cmd_slave_handler);
    transaction_register_rpc(RPC_ID_KB_EEPROM_SYNC, kb_eeprom_cmd_slave_handler);
#endif
    // Read the EEPROM data block
    eeconfig_read_kb_datablock(&eeprom_hybrid_config, 0, EECONFIG_KB_DATA_SIZE);

    runtime_hybrid_config.bottoming_calibration = false;

    for (uint8_t row = 0; row < MATRIX_ROWS_PER_HAND; row++) {
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

#ifdef SPLIT_KEYBOARD
    // If master, send EEPROM config to slave side
    if (is_keyboard_master()) {
        // Sync EEPROM config to slave side
        // This is done to ensure both sides have the same configuration in the case one side (either the master or the slave) is connected by itself and has its own EEPROM data.
        // transaction_rpc_send(RPC_ID_KB_EEPROM_SYNC, sizeof(eeprom_hybrid_config_t), &eeprom_hybrid_config);
        uint8_t *data_ptr   = (uint8_t *)&eeprom_hybrid_config;
        size_t   total_size = sizeof(eeprom_hybrid_config_t);
        uint8_t  seq        = 0;
        for (size_t offset = 0; offset < total_size; offset += EEPROM_CHUNK_SIZE) {
            size_t  chunk_size = (total_size - offset > EEPROM_CHUNK_SIZE) ? EEPROM_CHUNK_SIZE : (total_size - offset);
            uint8_t tx_buf[EEPROM_CHUNK_SIZE + 1];
            tx_buf[0] = seq; // sequence number
            memcpy(&tx_buf[1], data_ptr + offset, chunk_size);
            transaction_rpc_send(RPC_ID_KB_EEPROM_SYNC, chunk_size + 1, tx_buf);
            seq++;
        }
        // Send final sync complete message
        uint8_t end_buf[1] = {0xFF};
        transaction_rpc_send(RPC_ID_KB_EEPROM_SYNC, 1, end_buf);
    }
#endif

    // Initialize PAINTER if enabled
#ifdef QUANTUM_PAINTER_ENABLE
    display_init_kb();
#endif
    // Call user post-initialization
    keyboard_post_init_user();
}

#ifdef SPLIT_KEYBOARD
static uint8_t eeprom_sync_buffer[sizeof(eeprom_hybrid_config_t)];
static size_t  eeprom_sync_offset = 0;
static uint8_t expected_seq       = 0;

void kb_eeprom_cmd_slave_handler(uint8_t m2s_size, const void *m2s_buffer, uint8_t s2m_size, void *s2m_buffer) {
    if (m2s_size == 1 && ((uint8_t *)m2s_buffer)[0] == 0xFF) {
        // End of sync message
        if (eeprom_sync_offset == sizeof(eeprom_hybrid_config_t)) {
            memcpy(&eeprom_hybrid_config, eeprom_sync_buffer, sizeof(eeprom_hybrid_config_t));
            eeconfig_update_kb_datablock(&eeprom_hybrid_config, 0, EECONFIG_KB_DATA_SIZE);
            uprintf("EEPROM sync complete\n");
        } else {
            uprintf("EEPROM sync incomplete: received %u bytes\n", (unsigned int)eeprom_sync_offset);
        }
        eeprom_sync_offset = 0;
        expected_seq       = 0;
        return;
    }
    if (m2s_size > 1) {
        uint8_t        seq        = ((uint8_t *)m2s_buffer)[0];
        const uint8_t *chunk      = &((uint8_t *)m2s_buffer)[1];
        size_t         chunk_size = m2s_size - 1;
        if (seq != expected_seq) {
            uprintf("EEPROM sync sequence error: expected %u, got %u\n", (unsigned int)expected_seq, (unsigned int)seq);
            eeprom_sync_offset = 0;
            expected_seq       = 0;
            return;
        }
        if (eeprom_sync_offset + chunk_size <= sizeof(eeprom_hybrid_config_t)) {
            memcpy(eeprom_sync_buffer + eeprom_sync_offset, chunk, chunk_size);
            eeprom_sync_offset += chunk_size;
            expected_seq++;
        } else {
            uprintf("EEPROM sync overflow or unexpected chunk size\n");
            eeprom_sync_offset = 0;
            expected_seq       = 0;
        }
    } else {
        uprintf("EEPROM sync: invalid chunk size\n");
        eeprom_sync_offset = 0;
        expected_seq       = 0;
    }
}
#endif
