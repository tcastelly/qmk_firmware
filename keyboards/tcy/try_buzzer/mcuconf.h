#pragma once

#include_next <mcuconf.h>

#undef RP_PWM_USE_PWM1
#define RP_PWM_USE_PWM1 TRUE

#undef RP_PWM_USE_TIM1
#define RP_PWM_USE_TIM1 TRUE
