/*
Copyright 2019 @foostan
Copyright 2020 Drashna Jaelre <@drashna>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H
#include "layout_42.h"
#include "tcy.h"
#include "ps2_acceleration.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_QWERTY]        = LAYOUT_qwerty,
    [_QWERTY_OSX]    = LAYOUT_qwerty_osx,
    [_QWERTY_GAMING] = LAYOUT_qwerty_gaming,
    [_LOWER]         = LAYOUT_lower,
    [_RAISE]         = LAYOUT_raise,
    [_ESC]           = LAYOUT_esc,
    [_ESC_OSX]       = LAYOUT_esc_osx,
    [_NUM_PADS]      = LAYOUT_num_pads,
    [_ACCENTS_RALT]  = LAYOUT_accents_ralt,
    [_ADJUST]        = LAYOUT_adjust,
};

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    return tcy_pointing_device_task(mouse_report);
}

//Lighting
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    if (keep_rgb_off) return false;

    for (uint8_t i = led_min; i < led_max; i++) {
      rgb_matrix_set_color(i, COLOR_PURPLE[0], COLOR_PURPLE[1], COLOR_PURPLE[2]);
    }

    return false;
}

