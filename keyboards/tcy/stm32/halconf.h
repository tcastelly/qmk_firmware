#pragma once

#include_next <halconf.h>

// Force enable callbacks for the PS/2 Interrupt driver
#undef PAL_USE_CALLBACKS
#define PAL_USE_CALLBACKS TRUE

#undef PAL_USE_WAIT
#define PAL_USE_WAIT TRUE
