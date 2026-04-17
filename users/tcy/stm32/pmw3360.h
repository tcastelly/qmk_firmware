#pragma once

#include <stdint.h>
#include "spi_master.h"

// PMW3360 Register Addresses
#define PMW_PRODUCT_ID          0x00
#define PMW_REVISION_ID         0x01
#define PMW_MOTION              0x02
#define PMW_DELTA_X_L           0x03
#define PMW_DELTA_X_H           0x04
#define PMW_DELTA_Y_L           0x05
#define PMW_DELTA_Y_H           0x06
#define PMW_SQUAL               0x07
#define PMW_CONFIG1             0x3F
#define PMW_POWER_UP_RESET      0x3A
#define PMW_MOTION_BURST        0x50

// Function prototypes
void pmw3360_init(void);
void pmw3360_set_cpi(uint16_t cpi);
void pmw3360_read_burst(int16_t *dx, int16_t *dy);
