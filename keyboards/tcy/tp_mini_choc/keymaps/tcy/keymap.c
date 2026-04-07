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

#include "audio.h"
#include "layout_42.h"
#include "tcy.h"

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
    // for scrolling
    mouse_report = tcy_pointing_device_task(mouse_report);

    // Manually inject the MCP button into the report before processing logic
    if (mcp_click_state) {
      mouse_report.buttons |= MOUSE_BTN2;
    }

    // clicked from ps2
    // bool ps2_click = ps2_buttons_state & 0x01;   // left click from PS2

    // toggle buttons
    // used by PS/2 (PS/2 trigger left click by default) and trackpad
    if (mouse_report.buttons & MOUSE_BTN1) {  // left click
      if (IS_LAYER_ON(_ESC) || IS_LAYER_ON(_ESC_OSX)) {
        mouse_report.buttons &= ~MOUSE_BTN1;  // remove left
        mouse_report.buttons |=  MOUSE_BTN2;  // add right
      } else if (IS_LAYER_ON(_LOWER)) {
        mouse_report.buttons &= ~MOUSE_BTN1;  // remove left
        mouse_report.buttons |=  MOUSE_BTN3;  // add right
      }
    } else if (mouse_report.buttons & MOUSE_BTN2) {  // right click
      // used by MCP
      // MCP trigger right click by default
      if (IS_LAYER_ON(_ESC) || IS_LAYER_ON(_ESC_OSX)) {
        mouse_report.buttons &= ~MOUSE_BTN2;  // remove left
        mouse_report.buttons |=  MOUSE_BTN1;  // add right
      } else if (IS_LAYER_ON(_RAISE)) {
        mouse_report.buttons &= ~MOUSE_BTN2;  // remove left
        mouse_report.buttons |=  MOUSE_BTN3;  // add right
      }
    }

    uprintf(
        "I2C DPI=%u | PS2_DPI=%u\n",
        pointing_device_get_cpi(),
        ps2_acceleration_setting
        );

    return mouse_report;
}

