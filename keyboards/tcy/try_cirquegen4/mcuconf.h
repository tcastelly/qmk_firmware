#pragma once

#include_next <mcuconf.h>

/* GP0/GP1 = RP2040 I2C0 → enable the I2C0 peripheral. */
#undef  RP_I2C_USE_I2C0
#define RP_I2C_USE_I2C0 TRUE
