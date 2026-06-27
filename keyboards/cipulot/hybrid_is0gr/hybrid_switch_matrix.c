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

eeprom_hybrid_config_t  eeprom_hybrid_config;  // Definition of EEPROM shared instance
runtime_hybrid_config_t runtime_hybrid_config; // Definition of runtime shared instance

// Pin and port array
const pin_t row_pins[] = MATRIX_ROW_PINS;

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

    return 0;
}

// Initialize the noise floor and rescale per-key thresholds
void hybrid_noise_floor_calibration(void) {
    // Initialize all keys' noise floor to expected value
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            runtime_hybrid_config.runtime_key_state[row][col].noise_floor = EXPECTED_NOISE_FLOOR;
        }
    }

    // Sample multiple times to get an average noise floor
    for (uint8_t i = 0; i < DEFAULT_NOISE_FLOOR_SAMPLING_COUNT; i++) {
        for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
            for (uint8_t col = 0; col < MATRIX_COLS; col++) {
                // Read the raw switch value and accumulate to noise floor
                runtime_hybrid_config.runtime_key_state[row][col].noise_floor += hybrid_readkey_raw(row, col);
            }
        }
        // Small delay between samples
        wait_ms(5);
    }

    // Average the noise floor and rescale thresholds for all keys
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            // Get pointer to key state in runtime and EEPROM
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

    // Iterate through all keys
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            // Read the raw switch value
            sw_value[row][col] = hybrid_readkey_raw(row, col);
            // Get pointer to key state in runtime
            runtime_key_state_t *key_runtime = &runtime_hybrid_config.runtime_key_state[row][col];

            // Handle bottoming calibration or update key state
            if (runtime_hybrid_config.bottoming_calibration) {
                // Only track keys that are actually pressed (above noise floor + threshold)
                if (sw_value[row][col] > key_runtime->noise_floor + BOTTOMING_CALIBRATION_THRESHOLD) {
                    if (key_runtime->bottoming_calibration_starter) {
                        // First time seeing this key pressed - initialize with actual pressed value
                        key_runtime->bottoming_calibration_reading = sw_value[row][col];
                        key_runtime->bottoming_calibration_starter = false;
                    } else if (sw_value[row][col] > key_runtime->bottoming_calibration_reading) {
                        // Update bottoming reading if current reading is higher
                        key_runtime->bottoming_calibration_reading = sw_value[row][col];
                    }
                }
            } else {
                // Normal operation mode - update the key state
                updated |= hybrid_update_key(&current_matrix[row], row, col, sw_value[row][col]);
            }
        }
    }

    return runtime_hybrid_config.bottoming_calibration ? false : updated;
}

// Read the raw switch value from specified row and column
uint16_t hybrid_readkey_raw(uint8_t row, uint8_t col) {
    // Variable to store the switch value
    uint16_t sw_value = 0;

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

    // Handle switch type
    if (key_runtime->switch_type == 1) {
        // MX switch handling
        return hybrid_update_key_mx(current_row, col, sw_value, pressed);
    } else if (key_runtime->switch_type == 0) {
        // EC switch handling
        if (key_runtime->actuation_mode == 0) {
            // Normal board-wide APC (mode 0)
            return hybrid_update_key_apc(current_row, col, sw_value, key_runtime, pressed);
        } else if (key_runtime->actuation_mode == 1) {
            // Rapid Trigger (RT) mode
            return hybrid_update_key_rt(current_row, col, sw_value, key_runtime, pressed);
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
