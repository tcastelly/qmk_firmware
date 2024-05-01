#pragma once

/* The way how "handedness" is decided (which half is which),
see https://docs.qmk.fm/#/feature_split_keyboard?id=setting-handedness
for more options.
*/

#define MASTER_RIGHT
#define TAPPING_TERM 170
#define TAPPING_TERM_PER_KEY

#undef ENCODER_RESOLUTION
#define ENCODER_RESOLUTIONS_RIGHT { 1, 2 }
