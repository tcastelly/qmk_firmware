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
#include "analog.h"
#include "atomic_util.h"
#include "math.h"
#include "print.h"
#include "wait.h"
#include <string.h>

#if defined(__AVR__)
#    error "AVR platforms not supported due to a variety of reasons. Among them there are limited memory, limited number of pins and ADC not being able to give satisfactory results."
#endif

// Define if open-drain pin mode is supported
#define OPEN_DRAIN_SUPPORT defined(PAL_MODE_OUTPUT_OPENDRAIN)

eeprom_hybrid_config_t  eeprom_hybrid_config;   // Definition of EEPROM shared instance
runtime_hybrid_config_t runtime_hybrid_config;  // Definition of runtime shared instance
socd_cleaner_t          socd_opposing_pairs[4]; // Definition of SOCD shared instance

// Pin and port array
const pin_t row_pins[]                                 = MATRIX_ROW_PINS;
const pin_t amux_sel_pins[]                            = AMUX_SEL_PINS;
const pin_t amux_en_pins[]                             = AMUX_EN_PINS;
const pin_t amux_n_col_sizes[]                         = AMUX_COL_CHANNELS_SIZES;
const pin_t amux_n_col_channels[][AMUX_MAX_COLS_COUNT] = {AMUX_COL_CHANNELS};

// Define unused positions array if specified
#ifdef UNUSED_POSITIONS_LIST
uint8_t UNUSED_POSITIONS[][2] = UNUSED_POSITIONS_LIST;
#endif

// Define special positions array if specified
#ifdef SPECIAL_POSITIONS_LIST
uint8_t SPECIAL_POSITIONS[][2] = SPECIAL_POSITIONS_LIST;
#endif

// Number of AMUX selection pins
#define AMUX_SEL_PINS_COUNT ARRAY_SIZE(amux_sel_pins)
// Expected number of AMUX selection pins
#define EXPECTED_AMUX_SEL_PINS_COUNT ceil(log2(AMUX_MAX_COLS_COUNT))

// Checks for the correctness of the configuration
_Static_assert(ARRAY_SIZE(amux_en_pins) == AMUX_COUNT, "AMUX_EN_PINS doesn't have the minimum number of bits required to enable all the multiplexers available");
// Check that number of select pins is enough to select all the channels
_Static_assert(AMUX_SEL_PINS_COUNT == EXPECTED_AMUX_SEL_PINS_COUNT, "AMUX_SEL_PINS doesn't have the minimum number of bits required address all the channels");
// Check that number of elements in AMUX_COL_CHANNELS_SIZES is enough to specify the number of channels for all the multiplexers available
_Static_assert(ARRAY_SIZE(amux_n_col_sizes) == AMUX_COUNT, "AMUX_COL_CHANNELS_SIZES doesn't have the minimum number of elements required to specify the number of channels for all the multiplexers available");

// Matrix switch value storage
static uint16_t sw_value[MATRIX_ROWS][MATRIX_COLS];

// ADC multiplexer instance
static adc_mux adcMux;

// Initialize the row pins
void init_row(void) {
    // Set all row pins as output and low
    for (uint8_t idx = 0; idx < MATRIX_ROWS; idx++) {
        gpio_set_pin_output(row_pins[idx]);
        gpio_write_pin_low(row_pins[idx]);
    }
}

// Disable all the unused rows
void disable_unused_row(uint8_t row) {
    // disable all the other rows apart from the current selected one
    for (uint8_t idx = 0; idx < MATRIX_ROWS; idx++) {
        if (idx != row) {
            gpio_write_pin_low(row_pins[idx]);
        }
    }
}

// Initialize the AMUXs
void init_amux(void) {
    // Set all AMUX enable pins as output and disable all AMUXs
    for (uint8_t idx = 0; idx < AMUX_COUNT; idx++) {
        gpio_set_pin_output(amux_en_pins[idx]);
        gpio_write_pin_low(amux_en_pins[idx]);
    }
    // Set all AMUX selection pins as output
    for (uint8_t idx = 0; idx < AMUX_SEL_PINS_COUNT; idx++) {
        gpio_set_pin_output(amux_sel_pins[idx]);
    }
}

// Select the AMUX channel
void select_amux_channel(uint8_t channel, uint8_t col) {
    // Get the channel to select
    uint8_t ch = amux_n_col_channels[channel][col];
    // Disable the AMUX before changing the selection
    gpio_write_pin_high(amux_en_pins[channel]);
    // Set the selection pins
    for (uint8_t i = 0; i < AMUX_SEL_PINS_COUNT; i++) {
        gpio_write_pin(amux_sel_pins[i], ch & (1 << i));
    }
    // Enable the AMUX after changing the selection
    gpio_write_pin_low(amux_en_pins[channel]);
}

// Disable all the unused AMUXs
void disable_unused_amux(uint8_t channel) {
    // disable all the other AMUXs apart from the current selected one
    for (uint8_t idx = 0; idx < AMUX_COUNT; idx++) {
        if (idx != channel) {
            gpio_write_pin_high(amux_en_pins[idx]);
        }
    }
}

// Charge the peak hold capacitor
void charge_capacitor(uint8_t row) {
    // Set the row pin to high state to charge the capacitor
#ifdef OPEN_DRAIN_SUPPORT
    gpio_write_pin_high(DISCHARGE_PIN);
#else
    gpio_set_pin_input(DISCHARGE_PIN);
#endif
    gpio_write_pin_high(row_pins[row]);
}

// Discharge the peak hold capacitor
void discharge_capacitor(void) {
    // Set the discharge pin to low state to discharge the capacitor
#ifdef OPEN_DRAIN_SUPPORT
    gpio_write_pin_low(DISCHARGE_PIN);
#else
    gpio_write_pin_low(DISCHARGE_PIN);
    gpio_set_pin_output(DISCHARGE_PIN);
#endif
}

// Initialize the Hybrid switch matrix
int hybrid_init(void) {
    // Initialize the ADC peripheral
    palSetLineMode(ANALOG_PORT, PAL_MODE_INPUT_ANALOG);
    adcMux = pinToMux(ANALOG_PORT);

    // Dummy call to make sure that adcStart() has been called in the appropriate state
    adc_read(adcMux);

    // Initialize the discharge pin
    gpio_write_pin_low(DISCHARGE_PIN);
#ifdef OPEN_DRAIN_SUPPORT
    gpio_set_pin_output_open_drain(DISCHARGE_PIN);
#else
    gpio_set_pin_output(DISCHARGE_PIN);
#endif

    // Initialize row pins
    init_row();

    // Initialize AMUXs
    init_amux();

    return 0;
}

// Initialize the noise floor and rescale per-key thresholds
void hybrid_noise_floor_calibration(void) {
    // Column offsets for each AMUX
    uint8_t col_offsets[AMUX_COUNT];
    col_offsets[0] = 0;
    // Calculate column offsets
    for (uint8_t i = 1; i < AMUX_COUNT; i++) {
        col_offsets[i] = col_offsets[i - 1] + amux_n_col_sizes[i - 1];
    }

    // Initialize all keys' noise floor to expected value
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            runtime_hybrid_config.runtime_key_state[row][col].noise_floor = EXPECTED_NOISE_FLOOR;
        }
    }

    // Sample multiple times to get an average noise floor
    for (uint8_t i = 0; i < DEFAULT_NOISE_FLOOR_SAMPLING_COUNT; i++) {
        // Iterate through all AMUXs and columns
        for (uint8_t amux = 0; amux < AMUX_COUNT; amux++) {
            // Disable unused AMUXs
            disable_unused_amux(amux);
            // Iterate through all columns of the current AMUX
            for (uint8_t col = 0; col < amux_n_col_sizes[amux]; col++) {
                // Adjusted column index in the full matrix
                uint8_t adjusted_col = col + col_offsets[amux];
                for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
                    // Skip unused positions if specified
#ifdef UNUSED_POSITIONS_LIST
                    if (is_unused_position(row, adjusted_col)) continue;
#endif
                    // Disable unused rows
                    disable_unused_row(row);
                    // Read the raw switch value and accumulate to noise floor
                    runtime_hybrid_config.runtime_key_state[row][adjusted_col].noise_floor += hybrid_readkey_raw(amux, row, col);
                }
            }
        }
        // Small delay between samples
        wait_ms(5);
    }

    // Average the noise floor and rescale thresholds for all keys
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            // Get pointer to key state in runtime and EEPROM
            // Makes code more readable than having the expanded version multiple times
            runtime_key_state_t *key_runtime = &runtime_hybrid_config.runtime_key_state[row][col];
            eeprom_key_state_t  *key_eeprom  = &eeprom_hybrid_config.eeprom_key_state[row][col];

            // Average the noise floor
            key_runtime->noise_floor /= DEFAULT_NOISE_FLOOR_SAMPLING_COUNT;
            // Rescale all key thresholds based on the new noise floor
            bulk_rescale_key_thresholds(key_runtime, key_eeprom, RESCALE_MODE_ALL);
        }
    }
}

// Scan the Hybrid switch matrix
bool hybrid_matrix_scan(matrix_row_t current_matrix[]) {
    // Variable to track if any key state has changed
    bool updated = false;
    // Coordinate struct for ghost key detection
    KeyCoord keypresses[4];
    // Track count of keypresses in current scan cycle
    uint8_t mx_keypress_count = 0;

    // Column offsets for each AMUX
    uint8_t col_offsets[AMUX_COUNT];
    col_offsets[0] = 0;
    // Calculate column offsets
    for (uint8_t i = 1; i < AMUX_COUNT; i++) {
        col_offsets[i] = col_offsets[i - 1] + amux_n_col_sizes[i - 1];
    }

    // Iterate through all AMUXs and columns
    for (uint8_t amux = 0; amux < AMUX_COUNT; amux++) {
        // Disable unused AMUXs
        disable_unused_amux(amux);
        for (uint8_t col = 0; col < amux_n_col_sizes[amux]; col++) {
            // Adjusted column index in the full matrix
            uint8_t adjusted_col = col + col_offsets[amux];
            for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
                // Skip unused positions if specified
#ifdef UNUSED_POSITIONS_LIST
                if (is_unused_position(row, adjusted_col)) continue;
#endif
                // Disable unused rows
                disable_unused_row(row);
                // Read the raw switch value
                sw_value[row][adjusted_col] = hybrid_readkey_raw(amux, row, col);
                // Get pointer to key state in runtime
                runtime_key_state_t *key_runtime = &runtime_hybrid_config.runtime_key_state[row][adjusted_col];

                // Track keypresses for ghost key detection (MX mode)
                if (key_runtime->switch_type == SWITCH_TYPE_MX && (sw_value[row][adjusted_col] > 1000)) {
                    if (mx_keypress_count < 4) {
                        keypresses[mx_keypress_count++] = (KeyCoord){row, adjusted_col};
                    }
                }

                // Handle bottoming calibration or update key state
                // In bottoming calibration mode
                if (runtime_hybrid_config.bottoming_calibration) {
                    // Only track keys that are actually pressed (above noise floor + threshold)
                    if (sw_value[row][adjusted_col] > key_runtime->noise_floor + BOTTOMING_CALIBRATION_THRESHOLD) {
                        if (key_runtime->bottoming_calibration_starter) {
                            // First time seeing this key pressed - initialize with actual pressed value
                            key_runtime->bottoming_calibration_reading = sw_value[row][adjusted_col];
                            key_runtime->bottoming_calibration_starter = false;
                        } else if (sw_value[row][adjusted_col] > key_runtime->bottoming_calibration_reading) {
                            // Update bottoming reading if current reading is higher
                            key_runtime->bottoming_calibration_reading = sw_value[row][adjusted_col];
                        }
                    }
                } else { // Normal operation mode
                    // Update the key state and track if any change occurred
                    updated |= hybrid_update_key(&current_matrix[row], row, adjusted_col, sw_value[row][adjusted_col]);
                }
            }
        }
    }

    // Check if a square pattern exists and deactivate the last key in the pattern (ghost key
    if (mx_keypress_count == 4) {
        // If four keypresses detected, check for square formation (2x2 grid pattern)
        if (forms_square(keypresses[0], keypresses[1], keypresses[2], keypresses[3])) {
            // The last scanned key in the 2x2 square is the ghost key (determined by hardware scan order)
            KeyCoord ghost_key = keypresses[3];
            // Deactivate the ghost key from the matrix
            current_matrix[ghost_key.row] &= ~(1 << ghost_key.col);
        }
    }

    return runtime_hybrid_config.bottoming_calibration ? false : updated;
}

// Read the raw switch value from specified channel, row, and column
uint16_t hybrid_readkey_raw(uint8_t channel, uint8_t row, uint8_t col) {
    // Variable to store the switch value
    uint16_t sw_value = 0;

    // Select the AMUX channel and column
    select_amux_channel(channel, col);

    // Ensure the row pin is low before starting
    gpio_write_pin_low(row_pins[row]);

    // Atomic block to prevent interruptions during the critical timing section
    ATOMIC_BLOCK_FORCEON {
        // Charge the peak hold capacitor
        charge_capacitor(row);
        // Waiting for the capacitor to charge
        wait_us(CHARGE_TIME);
        // Read the ADC value
        sw_value = adc_read(adcMux);
    }
    // Discharge peak hold capacitor
    discharge_capacitor();
    // Waiting for the ghost capacitor to discharge fully
    wait_us(DISCHARGE_TIME);

    return sw_value;
}

// Update the key state based on the switch value
bool hybrid_update_key(matrix_row_t *current_row, uint8_t row, uint8_t col, uint16_t sw_value) {
    // Get pointer to key state in runtime and EEPROM
    runtime_key_state_t *key_runtime = &runtime_hybrid_config.runtime_key_state[row][col];
    eeprom_key_state_t  *key_eeprom  = &eeprom_hybrid_config.eeprom_key_state[row][col];

    // Current pressed state
    bool pressed = (*current_row >> col) & 1;

    // Update noise floor if current reading is lower than existing noise floor minus threshold
    if (sw_value + NOISE_FLOOR_THRESHOLD < key_runtime->noise_floor) {
        // Update noise floor
        key_runtime->noise_floor = sw_value;
        // Rescale all key thresholds based on new noise floor
        bulk_rescale_key_thresholds(key_runtime, key_eeprom, RESCALE_MODE_ALL);
    }

    if (runtime_hybrid_config.board_mode == BOARD_MODE_EC) {
        // Full EC mode - treat all keys as EC switches with APC mode
        // EC switch handling
        if (runtime_hybrid_config.board_actuation_mode == ACTUATION_MODE_APC) {
            // Normal board-wide APC (mode 0)
            return hybrid_update_key_apc(current_row, col, sw_value, key_runtime, pressed);
        } else if (runtime_hybrid_config.board_actuation_mode == ACTUATION_MODE_RT) {
            // Rapid Trigger (RT) mode
            return hybrid_update_key_rt(current_row, col, sw_value, key_runtime, pressed);
        }
    } else if (runtime_hybrid_config.board_mode == BOARD_MODE_MX) {
        // Full MX mode - treat all keys as MX switches
        return hybrid_update_key_mx(current_row, col, sw_value, pressed);
    } else if (runtime_hybrid_config.board_mode == BOARD_MODE_HYBRID) {
        // Hybrid mode - treat keys based on their individual switch type
        if (key_runtime->switch_type == SWITCH_TYPE_EC) {
            // EC switch handling
            if (key_runtime->actuation_mode == ACTUATION_MODE_APC) {
                // Normal board-wide APC (mode 0)
                return hybrid_update_key_apc(current_row, col, sw_value, key_runtime, pressed);
            } else if (key_runtime->actuation_mode == ACTUATION_MODE_RT) {
                // Rapid Trigger (RT) mode
                return hybrid_update_key_rt(current_row, col, sw_value, key_runtime, pressed);
            }
        } else if (key_runtime->switch_type == SWITCH_TYPE_MX) {
            // MX switch handling
            return hybrid_update_key_mx(current_row, col, sw_value, pressed);
        }
    }

    return false;
}

// Update the key state in APC mode
bool hybrid_update_key_apc(matrix_row_t *current_row, uint8_t col, uint16_t sw_value, runtime_key_state_t *key_runtime, bool pressed) {
    // Check for release condition
    if (pressed && sw_value < key_runtime->rescaled_apc_release_threshold) {
        // Key released
        *current_row &= ~(1 << col);
        return true;
    }
    // Check for actuation condition
    else if (!pressed && sw_value > key_runtime->rescaled_apc_actuation_threshold) {
        *current_row |= (1 << col);
        return true;
    }

    return false;
}

// Update the key state in RT mode
bool hybrid_update_key_rt(matrix_row_t *current_row, uint8_t col, uint16_t sw_value, runtime_key_state_t *key_runtime, bool pressed) {
    // Key in active zone
    if (sw_value > key_runtime->rescaled_rt_initial_deadzone_offset) {
        if (pressed) {
            // Track downward movement
            if (sw_value > key_runtime->extremum) {
                key_runtime->extremum = sw_value;
            }
            // Check for release threshold
            else if (sw_value < key_runtime->extremum - key_runtime->rescaled_rt_release_offset) {
                key_runtime->extremum = sw_value;
                *current_row &= ~(1 << col);
                return true;
            }
        } else {
            // Track upward movement
            if (sw_value < key_runtime->extremum) {
                key_runtime->extremum = sw_value;
            }
            // Check for actuation threshold
            else if (sw_value > key_runtime->extremum + key_runtime->rescaled_rt_actuation_offset) {
                key_runtime->extremum = sw_value;
                *current_row |= (1 << col);
                return true;
            }
        }
    }
    // Key outside active zone - force release if extremum dropped
    else if (sw_value < key_runtime->extremum) {
        key_runtime->extremum = sw_value;
        if (pressed) {
            *current_row &= ~(1 << col);
            return true;
        }
    }

    return false;
}

// Update the key state in MX mode
bool hybrid_update_key_mx(matrix_row_t *current_row, uint8_t col, uint16_t sw_value, bool pressed) {
    // Check for release condition
    if (pressed && sw_value < 950) {
        // Key released
        *current_row &= ~(1 << col);
        return true;
    }
    // Check for actuation condition
    else if (!pressed && sw_value > 1000) {
        *current_row |= (1 << col);
        return true;
    }

    return false;
}

// Rescale all key thresholds based on noise floor and bottoming calibration reading
void bulk_rescale_key_thresholds(runtime_key_state_t *key_runtime, eeprom_key_state_t *key_eeprom, rescale_mode_t mode) {
    // Rescale thresholds based on mode
    switch (mode) {
        case RESCALE_MODE_APC: // APC
            key_runtime->rescaled_apc_actuation_threshold = rescale(key_runtime->apc_actuation_threshold, key_runtime->noise_floor, key_eeprom->bottoming_calibration_reading);
            key_runtime->rescaled_apc_release_threshold   = rescale(key_runtime->apc_release_threshold, key_runtime->noise_floor, key_eeprom->bottoming_calibration_reading);
            break;
        case RESCALE_MODE_RT: // RT
            key_runtime->rescaled_rt_initial_deadzone_offset = rescale(key_runtime->rt_initial_deadzone_offset, key_runtime->noise_floor, key_eeprom->bottoming_calibration_reading);
            key_runtime->rescaled_rt_actuation_offset        = rescale(key_runtime->rt_actuation_offset, 0, key_eeprom->bottoming_calibration_reading);
            key_runtime->rescaled_rt_release_offset          = rescale(key_runtime->rt_release_offset, 0, key_eeprom->bottoming_calibration_reading);
            break;
        case RESCALE_MODE_ALL: // All thresholds
            key_runtime->rescaled_apc_actuation_threshold    = rescale(key_runtime->apc_actuation_threshold, key_runtime->noise_floor, key_eeprom->bottoming_calibration_reading);
            key_runtime->rescaled_apc_release_threshold      = rescale(key_runtime->apc_release_threshold, key_runtime->noise_floor, key_eeprom->bottoming_calibration_reading);
            key_runtime->rescaled_rt_initial_deadzone_offset = rescale(key_runtime->rt_initial_deadzone_offset, key_runtime->noise_floor, key_eeprom->bottoming_calibration_reading);
            key_runtime->rescaled_rt_actuation_offset        = rescale(key_runtime->rt_actuation_offset, 0, key_eeprom->bottoming_calibration_reading);
            key_runtime->rescaled_rt_release_offset          = rescale(key_runtime->rt_release_offset, 0, key_eeprom->bottoming_calibration_reading);
            break;
        default:
            bulk_rescale_key_thresholds(key_runtime, key_eeprom, RESCALE_MODE_ALL);
            break;
    }
}

// Unified helper function to update a field across all keys (runtime-only)
void update_keys_field(update_mode_t mode, size_t runtime_offset, size_t eeprom_offset, const void *value, size_t field_size) {
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            // If in Full EC mode or in Hybrid mode and the key is not a special position
            if ((runtime_hybrid_config.board_mode == BOARD_MODE_EC) || ((runtime_hybrid_config.board_mode == BOARD_MODE_HYBRID) && !is_special_position(row, col))) {
                // Update runtime
                uint8_t *runtime_field = (uint8_t *)&runtime_hybrid_config.runtime_key_state[row][col] + runtime_offset;
                memcpy(runtime_field, value, field_size);

                if (mode != HYBRID_UPDATE_RUNTIME_ONLY) {
                    // Determine EEPROM offset: shared or dual
                    size_t effective_eeprom_offset = (mode == HYBRID_UPDATE_SHARED_OFFSET) ? runtime_offset : eeprom_offset;

                    // Update EEPROM in-memory
                    uint8_t *eeprom_field = (uint8_t *)&eeprom_hybrid_config.eeprom_key_state[row][col] + effective_eeprom_offset;
                    memcpy(eeprom_field, value, field_size);
                }
            }
        }
    }
}

// Unified helper function to update a field of a specific key (runtime-only)
void update_single_key_field(update_mode_t mode, size_t runtime_offset, size_t eeprom_offset, const void *value, size_t field_size, uint8_t row, uint8_t col) {
    // Update runtime
    uint8_t *runtime_field = (uint8_t *)&runtime_hybrid_config.runtime_key_state[row][col] + runtime_offset;
    memcpy(runtime_field, value, field_size);

    if (mode != HYBRID_UPDATE_RUNTIME_ONLY) {
        // Determine EEPROM offset: shared or dual
        size_t effective_eeprom_offset = (mode == HYBRID_UPDATE_SHARED_OFFSET) ? runtime_offset : eeprom_offset;

        // Update EEPROM in-memory
        uint8_t *eeprom_field = (uint8_t *)&eeprom_hybrid_config.eeprom_key_state[row][col] + effective_eeprom_offset;
        memcpy(eeprom_field, value, field_size);
    }
}

// Check if four coordinates form a square
bool forms_square(KeyCoord key1, KeyCoord key2, KeyCoord key3, KeyCoord key4) {
    KeyCoord keys[4] = {key1, key2, key3, key4};

    // Collect unique rows and columns
    uint8_t unique_rows = 0, unique_cols = 0;
    uint8_t rows[2] = {0xFF, 0xFF}; // Use 0xFF as sentinel for "not set"
    uint8_t cols[2] = {0xFF, 0xFF};

    for (uint8_t i = 0; i < 4; i++) {
        // Check and add unique row
        if (keys[i].row != rows[0] && keys[i].row != rows[1]) {
            if (unique_rows < 2) {
                rows[unique_rows++] = keys[i].row;
            } else {
                return false; // More than 2 unique rows
            }
        }

        // Check and add unique column
        if (keys[i].col != cols[0] && keys[i].col != cols[1]) {
            if (unique_cols < 2) {
                cols[unique_cols++] = keys[i].col;
            } else {
                return false; // More than 2 unique columns
            }
        }
    }

    // Valid square: exactly 2 unique rows AND exactly 2 unique columns
    return unique_rows == 2 && unique_cols == 2;
}

// Print the switch matrix values for debugging
void hybrid_print_matrix(void) {
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS - 1; col++) {
            uprintf("%4d,", sw_value[row][col]);
        }
        uprintf("%4d\n", sw_value[row][MATRIX_COLS - 1]);
    }
    print("\n");
}

// rescale a value from 0-1023 to out_min - out_max
uint16_t rescale(uint16_t x, uint16_t out_min, uint16_t out_max) {
    return x * (out_max - out_min) / 1023 + out_min;
}

// Check if a position is unused (if UNUSED_POSITIONS_LIST is defined)
#ifdef UNUSED_POSITIONS_LIST
bool is_unused_position(uint8_t row, uint8_t col) {
    // Check against the list of unused positions
    for (uint8_t i = 0; i < UNUSED_POSITIONS_COUNT; i++) {
        // Compare current position with each unused position
        if (UNUSED_POSITIONS[i][0] == row && UNUSED_POSITIONS[i][1] == col) {
            return true;
        }
    }
    return false;
}
#endif

// Check if a position is special (if SPECIAL_POSITIONS is defined)
#ifdef SPECIAL_POSITIONS_LIST
bool is_special_position(uint8_t row, uint8_t col) {
    // Check against the list of special positions
    for (uint8_t i = 0; i < SPECIAL_POSITIONS_COUNT; i++) {
        // Compare current position with each special position
        if (SPECIAL_POSITIONS[i][0] == row && SPECIAL_POSITIONS[i][1] == col) {
            return true;
        }
    }
    return false;
}
#endif
