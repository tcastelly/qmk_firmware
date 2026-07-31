#pragma once

/* Cirque Gen4 talks I2C — the RP2040 ChibiOS HAL needs the I2C subsystem on. */
#define HAL_USE_I2C TRUE

#include_next <halconf.h>
