#pragma once

// EC sensing needs the ChibiOS ADC driver.
#define HAL_USE_ADC TRUE

// Split serial (bitbang) needs PAL line-wait / pad-event callbacks.
#define PAL_USE_CALLBACKS TRUE
#define PAL_USE_WAIT TRUE

#include_next <halconf.h>
