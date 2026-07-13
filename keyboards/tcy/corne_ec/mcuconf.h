// Copyright 2026 Thomas CASTELLY
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include_next <mcuconf.h>

/* EC matrix ADC (ANALOG_PORT A3 = ADC1_IN3) */
#undef STM32_ADC_USE_ADC1
#define STM32_ADC_USE_ADC1 TRUE

/* Split transport (full duplex on PA9/PA10, AF1) */
#undef STM32_SERIAL_USE_USART1
#define STM32_SERIAL_USE_USART1 TRUE

/* Reserved peripherals — see config.h */
#undef STM32_I2C_USE_I2C1
#define STM32_I2C_USE_I2C1 TRUE

#undef STM32_SPI_USE_SPI1
#define STM32_SPI_USE_SPI1 TRUE
