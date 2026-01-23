#pragma once

// Auto-detect handedness using GP3
// #define SPLIT_HAND_PIN GP3
// #define SPLIT_HAND_PIN_LOW_IS_LEFT
// #define SPLIT_USB_DETECT
//
// #define EE_HANDS
#define MASTER_RIGHT

// Left (slave) half matrix
#define MATRIX_COL_PINS { GP21, GP23, GP20, GP22, GP26, GP27, GP28, GP29 }
#define MATRIX_ROW_PINS { GP4, GP5, GP6, GP7 }

// Right (master) half matrix
#define MATRIX_COL_PINS_RIGHT { GP21, GP23, GP20, GP22, GP26, GP27, GP28, GP29 }
#define MATRIX_ROW_PINS_RIGHT { GP4, GP5, GP6, GP7 }
