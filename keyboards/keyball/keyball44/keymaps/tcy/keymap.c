/*
Copyright 2022 @Yowkees
Copyright 2022 MURAOKA Taro (aka KoRoN, @kaoriya)

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

#include "quantum.h"

#include "tapdance.c"
#ifdef OLED_ENABLE
  #include "bongo.h"
#endif

#include "lib/lib8tion/lib8tion.h"

enum oled_modes {
  OLED_BONGO_LAYOUT,
  OLED_OFF,
};

int8_t oled_mode = OLED_BONGO_LAYOUT;

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_QWERTY] = LAYOUT_universal(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      TD(TD_TAB),  KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,                         KC_Y,    KC_U,    KC_I,TD(TD_O),TD(TD_P), TD(TD_BSPC),
  //|--------+--------+-------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      TD(TD_ESC),  KC_A,   KC_S,    KC_D,    KC_F,    KC_G,                         KC_H,    KC_J,    KC_K,TD(TD_L),TD(TD_SCLN), KC_QUOT,
  //|--------+---- ----+-------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LSFT,     KC_Z,   KC_X,    KC_C,    KC_V,    KC_B,                         KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH, TD(TD_ENT),
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
            _______ , _______  ,  KC_LCTL,TD(TD_LALT),LOWER ,                   KC_SPC,   RAISE , _______, _______  , TD(TD_RALT)
                                      //`--------------------------'  `--------------------------'
  ),

  [_QWERTY_OSX] = LAYOUT_universal(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      TD(TD_TAB),  KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,                         KC_Y,    KC_U,    KC_I,TD(TD_O),TD(TD_P), TD(TD_BSPC_OSX),
  //|--------+--------+-------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      TD(TD_ESC_OSX),  KC_A,   KC_S,    KC_D,  KC_F,     KC_G,                          KC_H,    KC_J,    KC_K,TD(TD_L),TD(TD_SCLN), KC_QUOT,
  //|--------+---- ----+-------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LSFT,     KC_Z,   KC_X,    KC_C,    KC_V,    KC_B,                         KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH, TD(TD_ENT),
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
             _______ , _______  , TD(TD_LCTL),TD(TD_LGUI),LOWER,                KC_SPC,   RAISE, _______, _______, TD(TD_RALT_OSX)
                                      //`--------------------------'  `--------------------------'
  ),

  [_LOWER] = LAYOUT_universal(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
    KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,                       KC_CIRC, KC_AMPR,    KC_ASTR,    KC_LPRN, KC_RPRN, KC_DEL,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
    KC_CAPS, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                         KC_F6,   KC_MINS,    KC_PLUS,    KC_LCBR, KC_RCBR, KC_PIPE,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
    _______, KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,                        KC_F12,  S(KC_NUHS), KC_HOME, KC_END, _______, _______,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                        _______, _______, _______, _______, _______,             KC_MS_BTN1, _______, _______, _______, _______
                                      //`--------------------------'  `--------------------------'
  ),

  [_RAISE] = LAYOUT_universal(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
    KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                          KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
    KC_DEL,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                         KC_F6,   KC_UNDS, KC_EQL,  KC_LBRC, KC_RBRC, KC_BSLS,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
    _______, KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,                        KC_F12,  KC_NUHS, KC_PGUP, KC_PGDN, _______,  _______,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                        _______, _______, _______, _______, _______,             _______, _______, _______, _______, _______
                                      //`--------------------------'  `--------------------------'
  ),

  [_ESC_OSX] = LAYOUT_universal(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      ACCENT_GRAVE, ACCENT_GRAVE, _______, ACCENT_E_GRAVE, JET_RNM, _______,    ACCENT_CIRCUM, KC_WH_D, KC_WH_U, _______, _______, TD(TD_DEL_OSX),
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      _______, ACCENT_A_GRAVE, _______, _______, JET_FIND, _______,              TD(TD_LEFT_OSX), KC_DOWN, KC_UP,  TD(TD_RIGHT_OSX), _______, ACCENT_TREMA,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      _______, _______, _______, _______, _______, _______,                     KC_MS_LEFT,KC_MS_DOWN,KC_MS_UP, KC_MS_RIGHT, _______,  _______,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                       _______, _______, _______, _______,KC_MS_BTN2,          KC_MS_BTN1, _______, _______, _______, _______
                                      //`--------------------------'  `--------------------------'
  ),

  [_ESC] = LAYOUT_universal(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      ACCENT_GRAVE, ACCENT_GRAVE, _______, ACCENT_E_GRAVE, JET_RNM, _______,   ACCENT_CIRCUM, KC_WH_D, KC_WH_U, _______, _______, TD(TD_DEL),
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      _______, ACCENT_A_GRAVE, _______, _______, JET_FIND, _______,              TD(TD_LEFT), KC_DOWN, KC_UP,  TD(TD_RIGHT), _______, ACCENT_TREMA,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      _______, _______, _______, _______, _______, _______,                     KC_MS_LEFT,KC_MS_DOWN,KC_MS_UP, KC_MS_RIGHT, _______,  _______,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                  _______, _______, _______, _______,KC_MS_BTN2 ,             KC_MS_BTN1, _______, _______, _______, _______
                                      //`--------------------------'  `--------------------------'
  ),

  [_ADJUST] = LAYOUT_universal(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      _______, QWERTY , QWERTY_OSX  , _______, _______, _______,                 _______, _______, _______, _______, _______, QK_BOOT,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      TOGGLE_OLED, _______,      _______, _______, _______, _______,                _______,  _______, _______,  _______, _______, _______,
  //|--------+--------+-     -------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      _______, _______,      _______, _______, _______, _______,                _______, _______, _______, _______, _______, _______,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                        _______, _______, _______, _______, _______,             _______, _______, _______, _______, _______
                                      //`--------------------------'  `--------------------------'
  ),
};
// clang-format on

#ifdef OLED_ENABLE
bool oled_task_user(void) {
    oled_clear();
    switch (oled_mode) {
        default:
        case OLED_BONGO_LAYOUT:
            draw_bongo();
            break;
        case OLED_OFF:
            oled_off();
            break;
    }
    return false;
}
#endif

#ifdef POINTING_DEVICE_ENABLE
static uint32_t       last_mouse_activity = 0;
static report_mouse_t last_mouse_report   = {0};
static bool           is_scrolling        = false;
report_mouse_t smooth_mouse_movement(report_mouse_t mouse_report) {
    // Linear interpolation and ease-in-out
    static fract8 fract = 0.5;
    int8_t        x     = 0;
    int8_t        y     = 0;
    int8_t        h     = 0;
    int8_t        v     = 0;

    if (!is_scrolling) {
        x = ease8InOutApprox(lerp8by8(last_mouse_report.x, mouse_report.x, fract));
        y = ease8InOutApprox(lerp8by8(last_mouse_report.y, mouse_report.y, fract));
    } else {
        h = ease8InOutApprox(lerp8by8(last_mouse_report.x, mouse_report.x, fract));
        v = ease8InOutApprox(lerp8by8(last_mouse_report.y, mouse_report.y, fract));
    }

    // update the new smoothed report
    mouse_report.x = x;
    mouse_report.y = y;
    mouse_report.h = h;
    mouse_report.v = v;

    return mouse_report;
}

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    if (has_mouse_report_changed(&last_mouse_report, &mouse_report)) {
        last_mouse_activity = timer_read32();
        memcpy(&last_mouse_report, &mouse_report, sizeof(mouse_report));
    }

    return smooth_mouse_movement(mouse_report);
}
#endif

// Associate our tap dance key with its functionality
tap_dance_action_t tap_dance_actions[] = {
    [TD_ESC] = ACTION_TAP_DANCE_TAP_HOLD_LAYOUT(KC_ESC, _ESC),
    [TD_ESC_OSX] = ACTION_TAP_DANCE_TAP_HOLD_LAYOUT(KC_ESC, _ESC_OSX),
    [TD_TAB] = ACTION_TAP_DANCE_TAP_HOLD(KC_TAB, KC_TILD),
    [TD_O] = ACTION_TAP_DANCE_TAP_HOLD(KC_O, KC_LPRN),
    [TD_P] = ACTION_TAP_DANCE_TAP_HOLD(KC_P, KC_RPRN),
    [TD_L] = ACTION_TAP_DANCE_TAP_HOLD(KC_L, KC_LCBR),
    [TD_SCLN] = ACTION_TAP_DANCE_TAP_HOLD(KC_SCLN, KC_RCBR),
    [TD_ENT] = ACTION_TAP_DANCE_TAP_HOLD(KC_ENT, KC_LSFT),

    // same tap-dance
    // enable it for osx and linux
    [TD_LCTL] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_lctl_finished, td_lctl_reset),
    [TD_LALT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_lalt_finished, td_lalt_reset),
    [TD_LGUI] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_lgui_finished, td_lgui_reset),

    [TD_BSPC] = ACTION_TAP_DANCE_TAP_HOLD(KC_BSPC, LCTL(KC_BSPC)),
    [TD_BSPC_OSX] = ACTION_TAP_DANCE_TAP_HOLD(KC_BSPC, LALT(KC_BSPC)),

    [TD_DEL] = ACTION_TAP_DANCE_TAP_HOLD_UNPROTECTED(KC_DEL, LCTL(KC_DEL)),
    [TD_DEL_OSX] = ACTION_TAP_DANCE_TAP_HOLD_UNPROTECTED(KC_DEL, LALT(KC_DEL)),

    [TD_LEFT] = ACTION_TAP_DANCE_TAP_HOLD_UNPROTECTED(KC_LEFT, LCTL(KC_LEFT)),
    [TD_LEFT_OSX] = ACTION_TAP_DANCE_TAP_HOLD_UNPROTECTED(KC_LEFT, LALT(KC_LEFT)),

    [TD_RIGHT] = ACTION_TAP_DANCE_TAP_HOLD_UNPROTECTED(KC_RIGHT, LALT(KC_RIGHT)),
    [TD_RIGHT_OSX] = ACTION_TAP_DANCE_TAP_HOLD_UNPROTECTED(KC_RIGHT, LCTL(KC_RIGHT)),

    [TD_RIGHT_OSX] = ACTION_TAP_DANCE_TAP_HOLD_UNPROTECTED(KC_RIGHT, LALT(KC_RIGHT)),
    [TD_RIGHT] = ACTION_TAP_DANCE_TAP_HOLD_UNPROTECTED(KC_RIGHT, LCTL(KC_RIGHT)),

    [TD_RALT_OSX] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_ralt_osx_finished, td_ralt_osx_reset),
    [TD_RALT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_ralt_finished, td_ralt_reset)
};

// Set a long-ish tapping term for tap-dance keys
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case QK_TAP_DANCE ... QK_TAP_DANCE_MAX:
            return 275;
        default:
            return TAPPING_TERM;
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  tap_dance_action_t *action;

  switch (keycode) {
    case QWERTY:
        if (record->event.pressed) {
            layer_move(_QWERTY);
        }
        return false;
        break;

    case QWERTY_OSX:
        if (record->event.pressed) {
            layer_move(_QWERTY_OSX);
        }
        return false;
        break;

    case QWERTY_GAMING:
        if (record->event.pressed) {
            layer_move(_QWERTY_GAMING);
        }
        return false;
        break;

    case LOWER:
      if (record->event.pressed) {
        is_hold_tapdance_disabled = true;
        layer_on(_LOWER);
        update_tri_layer(_LOWER, _RAISE, _ADJUST);

        // enable scroll
        #ifdef POINTING_DEVICE_ENABLE
          is_scrolling = true;
          pointing_device_set_cpi(TRACK_BALL_MIN_DPI);
        #endif
      } else {
        layer_off(_LOWER);
        update_tri_layer(_LOWER, _RAISE, _ADJUST);
        is_hold_tapdance_disabled = false;

        #ifdef POINTING_DEVICE_ENABLE
          // disable scroll
          if (is_scrolling) {  // check if we were scrolling before and set disable if so
              is_scrolling = false;
              pointing_device_set_cpi(TRACK_BALL_DEFAULT_DPI);
          }
        #endif
      }
      return false;
      break;

    case RAISE:
      if (record->event.pressed) {
        is_hold_tapdance_disabled = true;
        layer_on(_RAISE);
        update_tri_layer(_LOWER, _RAISE, _ADJUST);
      } else {
        layer_off(_RAISE);
        update_tri_layer(_LOWER, _RAISE, _ADJUST);
        is_hold_tapdance_disabled = false;
      }
      return false;
      break;

    case KC_LALT:
    case KC_LGUI:
    case KC_LSFT:
      #ifdef POINTING_DEVICE_ENABLE
        if (record->event.pressed) {
            pointing_device_set_cpi(TRACK_BALL_MAX_DPI);
            is_hold_tapdance_disabled = true;
        } else {
            pointing_device_set_cpi(TRACK_BALL_DEFAULT_DPI);
            is_hold_tapdance_disabled = false;
        }
      #endif
      return true;
      break;

    case ACCENT_CIRCUM:
      if (record->event.pressed) {
          register_code(KC_RALT);
          register_code(KC_6);
      } else {
          unregister_code(KC_6);
          unregister_code(KC_RALT);
      }
      break;

    case ACCENT_TREMA:
      if (record->event.pressed) {
          register_code(KC_RALT);
          register_code(KC_LSFT);
          register_code(KC_QUOT);
      } else {
          unregister_code(KC_QUOT);
          unregister_code(KC_LSFT);
          unregister_code(KC_RALT);
      }
      break;

    case ACCENT_GRAVE:
      if (record->event.pressed) {
          register_code(KC_RALT);
          register_code(KC_GRV);
      } else {
          unregister_code(KC_GRV);
          unregister_code(KC_RALT);
      }
      break;

    case ACCENT_E_GRAVE:
      if (record->event.pressed) {
          register_code(KC_RALT);
          register_code(KC_GRV);
      } else {
          unregister_code(KC_GRV);
          unregister_code(KC_RALT);
          register_code(KC_E);
          unregister_code(KC_E);
      }
      break;

      // to be used with RALT already pressed
     case ACCENT_A_GRAVE_RALT:
       if (record->event.pressed) {
           register_code(KC_GRV);
       } else {
           unregister_code(KC_GRV);
           unregister_code(KC_RALT);
           register_code(KC_A);
           unregister_code(KC_A);

           // will be unregister by `td_ralt_reset`
           register_code(KC_RALT);
       }
       break;

     case ACCENT_I_CIRC_RALT:
       if (record->event.pressed) {
           register_code(KC_6);
       } else {
           unregister_code(KC_6);
           unregister_code(KC_RALT);
           register_code(KC_I);
           unregister_code(KC_I);

           // will be unregister by `td_ralt_reset`
           register_code(KC_RALT);
       }
       break;

     case ACCENT_O_CIRC_RALT:
       if (record->event.pressed) {
           register_code(KC_6);
       } else {
           unregister_code(KC_6);
           unregister_code(KC_RALT);
           register_code(KC_O);
           unregister_code(KC_O);

           // will be unregister by `td_ralt_reset`
           register_code(KC_RALT);
       }
       break;

     case ACCENT_U_AIGU_RALT:
       if (record->event.pressed) {
           register_code(KC_GRV);
       } else {
           unregister_code(KC_GRV);
           unregister_code(KC_RALT);
           register_code(KC_U);
           unregister_code(KC_U);

           // will be unregister by `td_ralt_reset`
           register_code(KC_RALT);
       }
       break;

     case ACCENT_C_RALT:
       if (record->event.pressed) {
           register_code(KC_COMM);
       } else {
           unregister_code(KC_COMM);
           unregister_code(KC_RALT);

           // will be unregister by `td_ralt_reset`
           register_code(KC_RALT);
       }
       break;

     case ACCENT_A_GRAVE:
       if (record->event.pressed) {
           register_code(KC_RALT);
           register_code(KC_GRV);
       } else {
           unregister_code(KC_GRV);
           unregister_code(KC_RALT);
           register_code(KC_A);
           unregister_code(KC_A);
       }
       break;

     case JET_RNM:
       if (record->event.pressed) {
           register_code(KC_LSFT);
           register_code(KC_F6);

           unregister_code(KC_LSFT);
           unregister_code(KC_F6);
       }
       return false;
       break;

     case JET_FIND:
       if (record->event.pressed) {
           register_code(KC_LALT);
           register_code(KC_F1);

           unregister_code(KC_F1);
           unregister_code(KC_LALT);
           tap_code(KC_1);
       }
       return false;
       break;

     case TOGGLE_OLED:
       if (record->event.pressed) {
           if (oled_mode != OLED_OFF) {
               oled_mode = OLED_OFF;
           } else {
               oled_mode = OLED_BONGO_LAYOUT;
           }
       }
       return false;
       break;

    case TD(TD_O):  // list all tap dance keycodes with tap-hold configurations
    case TD(TD_ESC):
    case TD(TD_ESC_OSX):
    case TD(TD_TAB):
    case TD(TD_P):
    case TD(TD_L):
    case TD(TD_ENT):
    case TD(TD_SCLN):
    case TD(TD_BSPC):
    case TD(TD_BSPC_OSX):
    case TD(TD_DEL):
    case TD(TD_DEL_OSX):
    case TD(TD_LEFT):
    case TD(TD_LEFT_OSX):
    case TD(TD_RIGHT):
    case TD(TD_RIGHT_OSX):
      if ((keycode == TD(TD_ESC) || keycode == TD(TD_ESC_OSX)) && !record->event.pressed) {
          layer_off(_ESC);
          layer_off(_ESC_OSX);
          is_hold_tapdance_disabled = false;

          #ifdef POINTING_DEVICE_ENABLE
            pointing_device_set_cpi(TRACK_BALL_DEFAULT_DPI);
          #endif
      }

      action = &tap_dance_actions[TD_INDEX(keycode)];
      if (!record->event.pressed && action->state.count && !action->state.finished) {
          tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)action->user_data;
          tap_code16(tap_hold->tap);
      }
      break;
  }
  return true;
}

#ifdef OLED_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    if (!is_keyboard_master()) {
        return OLED_ROTATION_180;  // flips the display 180 degrees if offhand
    }

    return rotation;
}
#endif

uint16_t keycode_config(uint16_t keycode) {
    return keycode;
}
uint8_t mod_config(uint8_t mod) {
    return mod;
}
