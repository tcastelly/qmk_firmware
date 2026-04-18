#pragma once

#include <stdint.h>
#include "hal.h"

// Define status types to match standard QMK I2C expectations
typedef enum {
    I2C_STATUS_SUCCESS = 0,
    I2C_STATUS_ERROR   = -1,
    I2C_STATUS_TIMEOUT = -2
} i2c_status_t;

// Function prototypes for the second hardware bus
void i2c2_init_custom(void);
i2c_status_t i2c2_write_reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t* data, uint16_t len, uint16_t timeout);
i2c_status_t i2c2_read_reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t* data, uint16_t len, uint16_t timeout);
