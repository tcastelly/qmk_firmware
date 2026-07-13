// Copyright 2026 Thomas CASTELLY
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

/* EC matrix needs the ADC; the split link needs SERIAL (USART1). */
#define HAL_USE_ADC TRUE
#define HAL_USE_SERIAL TRUE

/* Enabled ahead of time for the reserved peripherals (I2C1 trackpad,
 * SPI1 trackball — see config.h). Harmless when unused. */
#define HAL_USE_I2C TRUE
#define HAL_USE_SPI TRUE

#include_next <halconf.h>
