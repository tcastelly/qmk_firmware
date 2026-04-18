#pragma once
#include <stdint.h>

// Variable name and length as you specified
extern const uint16_t firmware_length;
extern const uint8_t firmware_data[];

// These satisfy the 'weak' calls in pmw33xx_common.c
uint16_t pmw33xx_srom_get_length(void);
uint8_t pmw33xx_srom_get_byte(uint16_t position);
