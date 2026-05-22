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

#ifdef AUDIO_ENABLE
#include "audio.h"
#endif
#include "layout_42.h"
#include "tcy.h"

#ifdef PMW3360_CUSTOM_ENABLE
#include "drivers/sensors/pmw3360.h"
#include "drivers/sensors/pmw33xx_common.h"
#endif

#ifndef constrain
#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
#endif

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_QWERTY]        = LAYOUT_qwerty,
    [_QWERTY_GAMING] = LAYOUT_qwerty_gaming,
    [_LOWER]         = LAYOUT_lower,
    [_RAISE]         = LAYOUT_raise,
    [_ESC]           = LAYOUT_esc,
    [_NUM_PADS]      = LAYOUT_num_pads,
    [_ACCENTS_RALT]  = LAYOUT_accents_ralt,
    [_ADJUST]        = LAYOUT_adjust,
};

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
#ifdef PMW3360_CUSTOM_ENABLE
    pmw33xx_report_t report = pmw33xx_read_burst(0);

    mouse_report.x += report.delta_x;
    mouse_report.y += report.delta_y;
#endif

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
      if (IS_LAYER_ON(_ESC)) {
        mouse_report.buttons &= ~MOUSE_BTN1;  // remove left
        mouse_report.buttons |=  MOUSE_BTN2;  // add right
      } else if (IS_LAYER_ON(_LOWER)) {
        mouse_report.buttons &= ~MOUSE_BTN1;  // remove left
        mouse_report.buttons |=  MOUSE_BTN3;  // add right
      }
    } else if (mouse_report.buttons & MOUSE_BTN2) {  // right click
      // used by MCP
      // MCP trigger right click by default
      if (IS_LAYER_ON(_ESC)) {
        mouse_report.buttons &= ~MOUSE_BTN2;  // remove left
        mouse_report.buttons |=  MOUSE_BTN1;  // add right
      } else if (IS_LAYER_ON(_RAISE)) {
        mouse_report.buttons &= ~MOUSE_BTN2;  // remove left
        mouse_report.buttons |=  MOUSE_BTN3;  // add right
      }
    }

    #ifdef CONSOLE_ENABLE
    static uint16_t log_timer = 0;
    if (timer_elapsed(log_timer) > 1000) {
        uprintf("DPI=%u | PS2_ACCEL=%u\n",
                 pointing_device_get_cpi(),
                 ps2_acceleration_setting);
        log_timer = timer_read();
    }
#endif
    return mouse_report;
}

