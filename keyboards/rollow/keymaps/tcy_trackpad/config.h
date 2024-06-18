#pragma once

/* The way how "handedness" is decided (which half is which),
see https://docs.qmk.fm/#/feature_split_keyboard?id=setting-handedness
for more options.
*/

// #define MASTER_RIGHT
#define EE_HANDS

#define TAPPING_TERM 170
#define TAPPING_TERM_PER_KEY

#undef ENCODER_RESOLUTION
#define ENCODER_RESOLUTIONS_RIGHT { 1, 2 }

#define POINTING_DEVICE_TASK_THROTTLE_MS 8
#define CIRQUE_PINNACLE_TAP_ENABLE
#define CIRQUE_PINNACLE_DIAMETER_MM	40
#define CIRQUE_PINNACLE_CURVED_OVERLAY
#define CIRQUE_PINNACLE_ATTENUATION EXTREG__TRACK_ADCCONFIG__ADC_ATTENUATE_2X
// #define POINTING_DEVICE_GESTURES_SCROLL_ENABLE
