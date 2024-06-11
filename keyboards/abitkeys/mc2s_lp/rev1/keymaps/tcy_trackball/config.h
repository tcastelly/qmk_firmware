#pragma once

/* place override params */
#define DYNAMIC_KEYMAP_LAYER_COUNT 10

/* AZ1UBALL settings */
// #define PIMORONI_TRACKBALL_SCALE 1    // default 5

// #undef RP_I2C_USE_I2C0
// #define RP_I2C_USE_I2C0 FALSE
// #undef RP_I2C_USE_I2C1
// #define RP_I2C_USE_I2C1 TRUE
// #define HAL_USE_I2C TRUE
//
// #undef I2C_DRIVER
// #define I2C_DRIVER I2CD1

#undef I2C0_SCL_PIN
#define I2C0_SCL_PIN PD0
#undef I2C0_SDA_PIN
#define I2C0_SDA_PIN PD1
