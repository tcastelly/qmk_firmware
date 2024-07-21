// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include <stdint.h>
#include "keycodes.h"
#include QMK_KEYBOARD_H
#include <stdio.h>
#include "quantum.h"

#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {[0] = LAYOUT(KC_MS_BTN1, KC_MS_BTN2, KC_MS_BTN3)};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] = {ENCODER_CCW_CW(KC_MS_WH_UP, KC_MS_WH_DOWN)},
};
#endif

#ifdef POINTING_DEVICE_ENABLE
#    define ANGLE_UNIT 15
#    define ANGLE_MAX (360 / ANGLE_UNIT)
uint8_t angle = -60;

report_mouse_t pointing_device_task_kb(report_mouse_t mouse_report) {
    double rad     = ANGLE_UNIT * angle * (M_PI / 180) * -1;
    int8_t x_rev   = +mouse_report.x * cos(rad) - mouse_report.y * sin(rad);
    int8_t y_rev   = +mouse_report.x * sin(rad) + mouse_report.y * cos(rad);
    mouse_report.x = x_rev;
    mouse_report.y = y_rev;
    return pointing_device_task_user(mouse_report);
}

enum my_keycodes { ROT_R15 = QK_KB_0, ROT_L15 };

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
#    ifdef CONSOLE_ENABLE
    uprintf("KL: kc: 0x%04X, col: %2u, row: %2u, pressed: %u, time: %5u, int: %u, count: %u\n", keycode, record->event.key.col, record->event.key.row, record->event.pressed, record->event.time, record->tap.interrupted, record->tap.count);
    uprintf("%04X  %04X\n", ROT_R15, ROT_L15);
#    endif

    switch (keycode) {
        case ROT_L15:
            angle = (angle + 1) % ANGLE_MAX;
            return false;
        case ROT_R15:
            angle = (angle + ANGLE_MAX - 1) % ANGLE_MAX;
            return false;
        default:
            return true;
    }
}

#endif
