#pragma once

#define HAL_USE_I2C TRUE

#undef HAL_USE_SERIAL
#define HAL_USE_SERIAL FALSE

#undef STM32_SERIAL_USE_USART3
#define STM32_SERIAL_USE_USART3 FALSE

#undef PAL_USE_CALLBACKS
#define PAL_USE_CALLBACKS TRUE

#undef PAL_USE_WAIT
#define PAL_USE_WAIT TRUE

#undef HAL_USE_PAL
#define HAL_USE_PAL TRUE

#include_next <halconf.h>

