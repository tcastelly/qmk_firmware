// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include <stdint.h>
#include "keycodes.h"
#include QMK_KEYBOARD_H
#include <stdio.h>
#include "quantum.h"

#include QMK_KEYBOARD_H

static bool scrolling_mode = false;

// Invert vertical scroll direction
#ifndef COCOT_SCROLL_INV_DEFAULT
#    define COCOT_SCROLL_INV_DEFAULT 1
#endif

#ifndef COCOT_SCROLL_DIVIDERS
#    define COCOT_SCROLL_DIVIDERS { 1, 2, 3, 4, 5, 6 }
#    ifndef COCOT_SCROLL_DIV_DEFAULT
#       define COCOT_SCROLL_DIV_DEFAULT 4
#    endif
#endif
#ifndef COCOT_SCROLL_DIV_DEFAULT
#    define COCOT_SCROLL_DIV_DEFAULT 4
#endif

//Tap Dance Declarations
enum {
  TD_MS_BTN1 = 0
};

typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD,
    TD_DOUBLE_TAP,
    TD_DOUBLE_HOLD,
    TD_DOUBLE_SINGLE_TAP, // Send two single taps
    TD_TRIPLE_TAP,
    TD_TRIPLE_HOLD
} td_state_t;

typedef struct {
    bool is_press_action;
    td_state_t state;
} td_tap_t;

// For the x tap dance. Put it here so it can be used in any keymap
void x_finished(tap_dance_state_t *state, void *user_data);
void x_reset(tap_dance_state_t *state, void *user_data);

//Tap Dance Definitions
tap_dance_action_t tap_dance_actions[] = {
  [TD_MS_BTN1]  = ACTION_TAP_DANCE_FN_ADVANCED(NULL, x_finished, x_reset)
};

uint16_t scrl_div_array[] = COCOT_SCROLL_DIVIDERS;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {[0] = LAYOUT(TD(TD_MS_BTN1), KC_MS_BTN2, KC_MS_BTN3)};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] = {ENCODER_CCW_CW(KC_MS_WH_UP, KC_MS_WH_DOWN)},
};
#endif

#ifdef POINTING_DEVICE_ENABLE
#    define ANGLE_UNIT 15
#    define ANGLE_MAX (360 / ANGLE_UNIT)
uint8_t angle = -60;

// Scroll Accumulation
static int16_t h_acm       = 0;
static int16_t v_acm       = 0;

report_mouse_t pointing_device_task_kb(report_mouse_t mouse_report) {
    double rad     = ANGLE_UNIT * angle * (M_PI / 180) * -1;
    int8_t x_rev   = +mouse_report.x * cos(rad) - mouse_report.y * sin(rad);
    int8_t y_rev   = +mouse_report.x * sin(rad) + mouse_report.y * cos(rad);

    if (scrolling_mode) {
        // rock scroll direction
        if (abs(x_rev) > abs(y_rev)) {
            y_rev = 0;
        } else {
            x_rev = 0;
        }

        // accumulate scroll
        h_acm += x_rev * COCOT_SCROLL_INV_DEFAULT;
        v_acm += y_rev * COCOT_SCROLL_INV_DEFAULT * -1;

        int8_t h_rev = h_acm >> scrl_div_array[COCOT_SCROLL_DIV_DEFAULT];
        int8_t v_rev = v_acm >> scrl_div_array[COCOT_SCROLL_DIV_DEFAULT];

        // clear accumulated scroll on assignment

        if (h_rev != 0) {
            if (mouse_report.h + h_rev > 127) {
                h_rev = 127 - mouse_report.h;
            } else if (mouse_report.h + h_rev < -127) {
                h_rev = -127 - mouse_report.h;
            }
            mouse_report.h += h_rev;
            h_acm -= h_rev << scrl_div_array[COCOT_SCROLL_DIV_DEFAULT];
        }
        if (v_rev != 0) {
            if (mouse_report.v + v_rev > 127) {
                v_rev = 127 - mouse_report.v;
            } else if (mouse_report.v + v_rev < -127) {
                v_rev = -127 - mouse_report.v;
            }
            mouse_report.v += v_rev;
            v_acm -= v_rev << scrl_div_array[COCOT_SCROLL_DIV_DEFAULT];
        }

        mouse_report.x = 0;
        mouse_report.y = 0;
    } else {
        mouse_report.x = x_rev;
        mouse_report.y = y_rev;
    }

    return pointing_device_task_user(mouse_report);
}

enum custom_keycodes {
  // _SCROLL = SAFE_RANGE,
  ROT_R15 = SAFE_RANGE,
  ROT_L15
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case ROT_L15:
            angle = (angle + 1) % ANGLE_MAX;
            return false;
        case ROT_R15:
            angle = (angle + ANGLE_MAX - 1) % ANGLE_MAX;
            return false;
            break;
        case QK_KB_0:
            scrolling_mode = record->event.pressed;
            return false;
        default:
            return true;
    }
}

#endif

td_state_t cur_dance(tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed) return TD_SINGLE_TAP;
        // Key has not been interrupted, but the key is still held. Means you want to send a 'HOLD'.
        else return TD_SINGLE_HOLD;
    } else if (state->count == 2) {
        // TD_DOUBLE_SINGLE_TAP is to distinguish between typing "pepper", and actually wanting a double tap
        // action when hitting 'pp'. Suggested use case for this return value is when you want to send two
        // keystrokes of the key, and not the 'double tap' action/macro.
        if (state->interrupted) return TD_DOUBLE_SINGLE_TAP;
        else if (state->pressed) return TD_DOUBLE_HOLD;
        else return TD_DOUBLE_TAP;
    }

    // Assumes no one is trying to type the same letter three times (at least not quickly).
    // If your tap dance key is 'KC_W', and you want to type "www." quickly - then you will need to add
    // an exception here to return a 'TD_TRIPLE_SINGLE_TAP', and define that enum just like 'TD_DOUBLE_SINGLE_TAP'
    if (state->count == 3) {
        if (state->interrupted || !state->pressed) return TD_TRIPLE_TAP;
        else return TD_TRIPLE_HOLD;
    } else return TD_UNKNOWN;
}

// Create an instance of 'td_tap_t' for the 'x' tap dance.
static td_tap_t xtap_state = {
    .is_press_action = true,
    .state = TD_NONE
};


void x_finished(tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP: register_code(KC_MS_BTN1); break;
        case TD_SINGLE_HOLD: register_code(KC_MS_BTN1); break;
        case TD_DOUBLE_TAP: register_code(KC_MS_BTN2); break;
        case TD_DOUBLE_HOLD:
            scrolling_mode = true;
          break;
        // Last case is for fast typing. Assuming your key is `f`:
        // For example, when typing the word `buffer`, and you want to make sure that you send `ff` and not `Esc`.
        // In order to type `ff` when typing fast, the next character will have to be hit within the `TAPPING_TERM`, which by default is 200ms.
        case TD_DOUBLE_SINGLE_TAP: tap_code(KC_MS_BTN2); register_code(KC_MS_BTN2); break;
        default: break;
    }
}

void x_reset(tap_dance_state_t *state, void *user_data) {
    switch (xtap_state.state) {
        case TD_SINGLE_TAP: unregister_code(KC_MS_BTN1); break;
        case TD_SINGLE_HOLD: unregister_code(KC_MS_BTN1); break;
        case TD_DOUBLE_TAP: unregister_code(KC_MS_BTN2); break;
        case TD_DOUBLE_HOLD:
          scrolling_mode = false;
          break;
        case TD_DOUBLE_SINGLE_TAP: unregister_code(KC_MS_BTN2); break;
        default: break;
    }
    xtap_state.state = TD_NONE;
}

 void keyboard_post_init_user(void) {
    pointing_device_set_cpi(500);
 }
