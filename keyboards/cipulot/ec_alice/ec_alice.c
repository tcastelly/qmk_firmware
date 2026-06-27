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

#include "ec_switch_matrix.h"
#include "keyboard.h"

#ifdef SPLIT_KEYBOARD
#    include "transactions.h"
#endif

// EEPROM default initialization
void eeconfig_init_kb(void) {
    // Initialize indicator defaults
    eeprom_ec_config.ind1.h       = 0;
    eeprom_ec_config.ind1.s       = 255;
    eeprom_ec_config.ind1.v       = 150;
    eeprom_ec_config.ind1.func    = 0x04;
    eeprom_ec_config.ind1.index   = 0;
    eeprom_ec_config.ind1.enabled = true;

    eeprom_ec_config.ind2.h       = 86;
    eeprom_ec_config.ind2.s       = 255;
    eeprom_ec_config.ind2.v       = 150;
    eeprom_ec_config.ind2.func    = 0x04;
    eeprom_ec_config.ind2.index   = 1;
    eeprom_ec_config.ind2.enabled = true;

    eeprom_ec_config.ind3.h       = 166;
    eeprom_ec_config.ind3.s       = 254;
    eeprom_ec_config.ind3.v       = 150;
    eeprom_ec_config.ind3.func    = 0x04;
    eeprom_ec_config.ind3.index   = 2;
    eeprom_ec_config.ind3.enabled = true;

    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            // Get pointer to key state in EEPROM
            eeprom_key_state_t *key_eeprom = &eeprom_ec_config.eeprom_key_state[row][col];
            // Set default values
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
        eeprom_ec_config.eeprom_socd_opposing_pairs[i].keys[0]    = socd_pairs[i].key1;
        eeprom_ec_config.eeprom_socd_opposing_pairs[i].keys[1]    = socd_pairs[i].key2;
        eeprom_ec_config.eeprom_socd_opposing_pairs[i].resolution = SOCD_CLEANER_OFF;
        eeprom_ec_config.eeprom_socd_opposing_pairs[i].held[0]    = false;
        eeprom_ec_config.eeprom_socd_opposing_pairs[i].held[1]    = false;
    }

    // Write to EEPROM entire datablock
    eeconfig_update_kb_datablock(&eeprom_ec_config, 0, EECONFIG_KB_DATA_SIZE);

    // Call user initialization
    eeconfig_init_user();
}

// Keyboard post-initialization
void keyboard_post_init_kb(void) {
    // Read the EEPROM data block
    eeconfig_read_kb_datablock(&eeprom_ec_config, 0, EECONFIG_KB_DATA_SIZE);

    runtime_ec_config.bottoming_calibration = false;

    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            // Get pointer to key state in runtime and EEPROM
            runtime_key_state_t *key_runtime = &runtime_ec_config.runtime_key_state[row][col];
            eeprom_key_state_t  *key_eeprom  = &eeprom_ec_config.eeprom_key_state[row][col];

            // Copy from EEPROM to runtime
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
    memcpy(socd_opposing_pairs, eeprom_ec_config.eeprom_socd_opposing_pairs, sizeof(socd_opposing_pairs));

    // Set the RGB LEDs range that will be used for the effects
    rgblight_set_effect_range(3, 36);

    // Call the indicator callback to set the indicator color
    indicators_callback();

    // Call user post-initialization
    keyboard_post_init_user();
}

// This function gets called when caps, num, scroll change
bool led_update_kb(led_t led_state) {
    indicators_callback();
    return true;
}

// This function is called when layers change
__attribute__((weak)) layer_state_t layer_state_set_user(layer_state_t state) {
    indicators_callback();
    return state;
}

bool func_switch(uint8_t func) {
    switch (func) {
        case 0x00: {
            return false;
            break;
        }
        case 0x01: // If indicator is set as caps lock
        {
            if (host_keyboard_led_state().caps_lock) return true;
            break;
        }
        case 0x02: // If indicator is set as num lock
        {
            if (host_keyboard_led_state().num_lock) return true;
            break;
        }
        case 0x03: // If indicator is set as scroll lock
        {
            if (host_keyboard_led_state().scroll_lock) return true;
            break;
        }
        case 0x04: // If indicator is set as layer 0
        case 0x05: // layer 1
        case 0x06: // layer 2
        case 0x07: // layer 3
        case 0x08: // layer 4
        case 0x09: // layer 5
        case 0x0A: // layer 6
        case 0x0B: // layer 7
        {
            if (IS_LAYER_ON((int)(func)-4)) return true;
            break;
        }
        default: {
            return false;
        }
    }
    return false;
}

bool set_indicator(indicator_config indicator) {
    if (!indicator.enabled) return false;
    return func_switch(indicator.func & 0x0F);
}

bool indicators_callback(void) {
    // Basic functioning: for each indicator, set_indicator is used to decide if the current indicator should be lit or off.
    indicator_config *current_indicator_p;
    int               index;
    RGB               color;
    for (index = 0; index < 3; index++) {
        current_indicator_p = get_indicator_p(index);
        if (set_indicator(*(current_indicator_p))) {
            /*
               Issue: while the VIA custom GUI returns HSV values, the QMK direct operation funcs are RGB.
               So this line converts the current indicator to RGB. This was not done at the indicator_config_set_value VIA callback function
               because at the indicator_config_get_value the RGB to HSV would be required and this throttles the keyboard
               when the user is adjusting the color on the GUI.
            */
            color = hsv_to_rgb((HSV){current_indicator_p->h, current_indicator_p->s, current_indicator_p->v});
            rgblight_setrgb_at(color.r, color.g, color.b, current_indicator_p->index);
        } else
            rgblight_setrgb_at(RGB_OFF, current_indicator_p->index);
    }
    return true;
}
