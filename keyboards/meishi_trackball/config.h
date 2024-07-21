// Copyright 2024 esplo (@esplo)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

/*
 * Feature disable options
 *  These options are also useful to firmware size reduction.
 */

/* disable debug print */
// #define NO_DEBUG

/* disable print */
// #define NO_PRINT

/* disable action features */
// #define NO_ACTION_LAYER
// #define NO_ACTION_TAPPING
// #define NO_ACTION_ONESHOT

/* Rotary encoder */
#define ENCODERS_PAD_A \
    { D2 }
#define ENCODERS_PAD_B \
    { D3 }
#define ENCODER_RESOLUTION 4

#define ADNS5050_SCLK_PIN F7
#define ADNS5050_SDIO_PIN F6
#define ADNS5050_CS_PIN B1
