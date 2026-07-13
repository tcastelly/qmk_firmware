// Copyright 2022 sekigon-gonnoc
// SPDX-License-Identifier: GPL-2.0-or-later

//
// Scan EC switch matrix using 74HC7051
// Define MUX_SEL_PINS, DISCHARGE_PIN, and ANALOG_PORT to compile
//

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "matrix.h"

typedef struct {
    uint16_t low_threshold;    // global fallback release threshold
    uint16_t high_threshold;   // global fallback press threshold
    const uint16_t *low_keys;  // per-key release thresholds [rows*cols], or NULL
    const uint16_t *high_keys; // per-key press thresholds [rows*cols], or NULL
} ecsm_config_t;

int      ecsm_init(ecsm_config_t const* const ecsm_config);
void     ecsm_get_config(ecsm_config_t* ecsm_config);
bool     ecsm_matrix_scan(matrix_row_t current_matrix[]);
void     ecsm_dprint_matrix(void);
uint16_t ecsm_get_sw_value(uint8_t row, uint8_t col);

#ifdef ECS_VELOCITY_ENABLED
int16_t ecsm_get_velocity(uint8_t row, uint8_t col);
void    ecsm_dprint_velocity(void);
#endif
