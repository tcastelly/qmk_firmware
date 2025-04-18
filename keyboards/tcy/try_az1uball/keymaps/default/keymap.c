// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

// AZ1UBALL acceleration
// void pointing_device_init_kb(void) {
//     uint8_t addr=(0x0a << 1 );        // ic2 address with 1bit shift up
//     //uint8_t data[]={0x90, 0x00};    // AZ1UBALL normal speed mode
//     uint8_t data[]={0x91, 0x00};      // AZ1UBALL accellaration mode
//     uint16_t timeout=100;             // in milli-seconds
//     i2c_status_t status;
//     status  = i2c_transmit (addr, data, 1, timeout);
//     //if (status != I2C_STATUS_SCCESS) {
//     if (status != 0) {
//         return;
//     }
// }


// Modify these values to adjust the scrolling speed
#define SCROLL_DIVISOR_H 8.0
#define SCROLL_DIVISOR_V 8.0

// Variables to store accumulated scroll values
float scroll_accumulated_h = 0;
float scroll_accumulated_v = 0;

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    // Calculate and accumulate scroll values based on mouse movement and divisors
    scroll_accumulated_h += (float)mouse_report.x / SCROLL_DIVISOR_H;
    scroll_accumulated_v += (float)mouse_report.y / SCROLL_DIVISOR_V;

    // Assign integer parts of accumulated scroll values to the mouse report
    mouse_report.h = (int8_t)scroll_accumulated_h;
    mouse_report.v = (int8_t)scroll_accumulated_v;

    // Update accumulated scroll values by subtracting the integer parts
    scroll_accumulated_h -= (int8_t)scroll_accumulated_h;
    scroll_accumulated_v -= (int8_t)scroll_accumulated_v;

    // Clear the X and Y values of the mouse report
    mouse_report.x = 0;
    mouse_report.y = 0;

    return mouse_report;
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_1x1(
        KC_A
    )
};
