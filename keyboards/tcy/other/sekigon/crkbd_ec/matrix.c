// Copyright 2022 sekigon-gonnoc
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Modern QMK CUSTOM_MATRIX=lite + split_common handles the split transport,
// debounce and matrix_scan_quantum itself; this board only provides the EC
// init + per-hand scan.

#include QMK_KEYBOARD_H
#include "ec_switch_matrix.h"
#include "matrix.h"
#include "split_util.h"

// Measured rest ADC values per half (no key pressed, at boot).
// Used to compute per-key LOW/HIGH thresholds so each switch gets its own
// calibrated hysteresis band regardless of inter-switch variation.
// Remeasure after any hardware change by checking qmk console at boot.
static const uint16_t rest_left[3][7] = {
    { 67, 77, 63, 77, 57, 54, 52 },
    { 67, 86, 94, 88, 71, 57, 77 },
    { 77, 64, 83, 87, 80, 52, 78 },
};
static const uint16_t rest_right[3][7] = {
    {  49,  71,  79, 115,  99, 56, 62 },
    {  81,  90,  91,  94,  84, 91, 83 },
    {  78,  96, 109, 112,  79, 78, 74 },
};

// Margins above the rest value for each threshold.
// Pressed values are ~360; rest values range 49-115.
// LOW_MARGIN: value must drop THIS far above rest to release (ensures the
//   value crosses LOW on the way back down from a press).
// HIGH_MARGIN: value must rise THIS far above rest to register a press.
#define REST_LOW_MARGIN  50
#define REST_HIGH_MARGIN 150

static uint16_t key_low[3 * 7];
static uint16_t key_high[3 * 7];

void matrix_init_custom(void) {
    const uint16_t (*rest)[7] = is_keyboard_left() ? rest_left : rest_right;

    for (uint8_t r = 0; r < 3; r++) {
        for (uint8_t c = 0; c < 7; c++) {
            key_low[r * 7 + c]  = rest[r][c] + REST_LOW_MARGIN;
            key_high[r * 7 + c] = rest[r][c] + REST_HIGH_MARGIN;
        }
    }

    ecsm_config_t ecsm_config = {
        .low_threshold  = LOW_THRESHOLD,
        .high_threshold = HIGH_THRESHOLD,
        .low_keys       = key_low,
        .high_keys      = key_high,
    };
    ecsm_init(&ecsm_config);
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    return ecsm_matrix_scan(current_matrix);
}
