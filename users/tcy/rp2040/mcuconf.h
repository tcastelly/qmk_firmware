#pragma once

#include_next <mcuconf.h>

// Audio
#undef RP_PWM_USE_PWM1
#define RP_PWM_USE_PWM1 TRUE

#undef RP_PWM_USE_TIM1
#define RP_PWM_USE_TIM1 TRUE

// pmw3360
#undef RP_SPI_USE_SPI0
#define RP_SPI_USE_SPI0 FALSE

#undef RP_SPI_USE_SPI1
#define RP_SPI_USE_SPI1 TRUE
