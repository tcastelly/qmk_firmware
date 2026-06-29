// Copyright 2022 sekigon-gonnoc
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Modern QMK CUSTOM_MATRIX=lite + split_common handles the split transport,
// debounce and matrix_scan_quantum itself; this board only provides the EC
// init + per-hand scan.

#include QMK_KEYBOARD_H
#include "ec_switch_matrix.h"
#include "matrix.h"
#include "eeprom.h"

#ifndef LOW_THRESHOLD
#    define LOW_THRESHOLD 200
#endif

#ifndef HIGH_THRESHOLD
#    define HIGH_THRESHOLD 300
#endif

void matrix_init_custom(void) {
    ecsm_config_t ecsm_config;
#ifdef EEPROM_ECS_THRESHOLD_ADDR
    ecsm_config.low_threshold  = eeprom_read_word((uint16_t*)EEPROM_ECS_THRESHOLD_ADDR);
    ecsm_config.high_threshold = eeprom_read_word((uint16_t*)(EEPROM_ECS_THRESHOLD_ADDR + 2));
#else
    ecsm_config.low_threshold  = LOW_THRESHOLD;
    ecsm_config.high_threshold = HIGH_THRESHOLD;
#endif

    ecsm_init(&ecsm_config);
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    return ecsm_matrix_scan(current_matrix);
}
