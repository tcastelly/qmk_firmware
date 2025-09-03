#pragma once

#define USE_SERIAL          // enable hardware serial split

#define I2C1_SCL_PIN GP3
#define I2C1_SDA_PIN GP2

// Left (slave) half matrix
#define MATRIX_COL_PINS { GP29, GP28, GP27, GP26, GP22, GP20 }
#define MATRIX_ROW_PINS { GP4, GP5, GP6, GP7 }

// Right (master) half matrix
#define MATRIX_COL_PINS_RIGHT { GP20, GP22, GP26, GP27, GP28, GP29 }
#define MATRIX_ROW_PINS_RIGHT { GP4, GP5, GP6, GP7 }

// Auto-detect handedness using GP23
#define SPLIT_HAND_PIN GP23

#define OLED_TIMEOUT 30000

#define AZOTEQ_IQS5XX_TPS65
#define AZOTEQ_IQS5XX_ROTATION_270

#define RGB_MATRIX_TIMEOUT 30000  // milliseconds (e.g., 30000 = 30s)

#undef RGB_MATRIX_MAXIMUM_BRIGHTNESS
#define RGB_MATRIX_MAXIMUM_BRIGHTNESS 100

#define SPLIT_ACTIVITY_ENABLE
#define SPLIT_MODS_ENABLE
#define SPLIT_LAYER_STATE_ENABLE
#define SPLIT_POINTING_ENABLE

// #define POINTING_DEVICE_LEFT pimoroni_trackball
// #define POINTING_DEVICE_RIGHT azoteq_touch
#define POINTING_DEVICE_COMBINED

// accelaration mode
#define PIMORONI_TRACKBALL_SCALE 10    // default 5
