#include QMK_KEYBOARD_H
#include "tapdance.c"
#include "lib/lib8tion/lib8tion.h"

static bool scrolling_mode = false;

static bool lock_mode = false;

// switch off the power light of the liatris controller
void keyboard_pre_init_user(void) {
  // Set our LED pin as output
  setPinOutput(24);
  // Turn the LED off
  // (Due to technical reasons, high is off and low is on)
  writePinHigh(24);
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_QWERTY] = LAYOUT_split_3x6_3(
  //,---------------------------------------------------            ,-----------------------------------------------------.
      TD(TD_TAB),  KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,                         KC_Y,    KC_U,    KC_I,TD(TD_O),TD(TD_P), TD(TD_BSPC),
  //|--------+--------+-------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      TD(TD_ESC),  KC_A,   KC_S,    KC_D,    KC_F,    KC_G,                         KC_H,    KC_J,    KC_K,TD(TD_L),TD(TD_SCLN), KC_QUOT,
  //|--------+---- ----+-------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LSFT,     KC_Z,   KC_X,    KC_C,    KC_V,    KC_B,                         KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH, TD(TD_ENT),
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                        KC_LCTL,TD(TD_LALT),LOWER,    KC_SPC,   RAISE, TD(TD_RALT)
                                      //`--------------------------'  `--------------------------'
  ),

  [_QWERTY_OSX] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      TD(TD_TAB),  KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,                         KC_Y,    KC_U,    KC_I,TD(TD_O),TD(TD_P), TD(TD_BSPC_OSX),
  //|--------+--------+-------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      TD(TD_ESC_OSX),  KC_A,   KC_S,    KC_D,  KC_F,     KC_G,                          KC_H,    KC_J,    KC_K,TD(TD_L),TD(TD_SCLN), KC_QUOT,
  //|--------+---- ----+-------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LSFT,     KC_Z,   KC_X,    KC_C,    KC_V,    KC_B,                         KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH, TD(TD_ENT),
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                        TD(TD_LCTL),TD(TD_LGUI),LOWER,    KC_SPC,   RAISE, TD(TD_RALT_OSX)
                                      //`--------------------------'  `--------------------------'
  ),

  [_QWERTY_GAMING] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      KC_TAB,  KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,                         KC_Y,    KC_U,    KC_I,KC_O,KC_P, KC_BSPC,
  //|--------+--------+-------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_ESC,  KC_A,   KC_S,    KC_D,    KC_F,    KC_G,                         KC_H,    KC_J,    KC_K,KC_L,KC_SCLN, KC_QUOT,
  //|--------+---- ----+-------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LSFT,     KC_Z,   KC_X,    KC_C,    KC_V,    KC_B,                         KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH, KC_ENT,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                        KC_LCTL,KC_SPC,RAISE,          KC_SPC,   LOWER, KC_RALT
                                      //`--------------------------'  `--------------------------'
  ),

  [_LOWER] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
    KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,                       KC_CIRC, KC_AMPR,    KC_ASTR,    KC_LPRN, KC_RPRN, KC_DEL,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
    KC_CAPS, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                         KC_F6,   KC_MINS,    KC_PLUS,    KC_LCBR, KC_RCBR, KC_PIPE,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
    _______, KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,                        KC_F12,  S(KC_NUHS), KC_HOME, KC_END, _______, _______,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          _______, QK_BOOT, _______,  KC_MS_BTN1, _______, _______
                                      //`--------------------------'  `--------------------------'
  ),

  [_RAISE] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
    KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                          KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
    KC_DEL,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                         KC_F6,   KC_UNDS, KC_EQL,  KC_LBRC, KC_RBRC, KC_BSLS,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
    _______, KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,                        KC_F12,  KC_NUHS, KC_PGUP, KC_PGDN, _______,  _______,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          _______, _______, _______,    _______, _______, QK_BOOT
                                      //`--------------------------'  `--------------------------'
  ),

  [_ESC_OSX] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      ACCENT_GRAVE, ACCENT_GRAVE, _______, ACCENT_E_GRAVE, JET_RNM, _______,    ACCENT_CIRCUM, KC_WH_D, KC_WH_U, JET_OPTI, JET_FORMAT_OSX, TD(TD_DEL_OSX),
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      _______, ACCENT_A_GRAVE, _______, _______, JET_FIND, _______,              TD(TD_LEFT_OSX), KC_DOWN, KC_UP,  TD(TD_RIGHT_OSX), _______, ACCENT_TREMA,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      _______, _______, _______, _______, _______, _______,                     KC_MS_LEFT,KC_MS_DOWN,KC_MS_UP, KC_MS_RIGHT, _______,  _______,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          _______, _______,KC_MS_BTN2, KC_MS_BTN1, _______, _______
                                      //`--------------------------'  `--------------------------'
  ),

  [_ESC] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      ACCENT_GRAVE, ACCENT_GRAVE, _______, ACCENT_E_GRAVE, JET_RNM, _______,   ACCENT_CIRCUM, KC_WH_D, KC_WH_U, JET_OPTI, JET_FORMAT, TD(TD_DEL),
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      _______, ACCENT_A_GRAVE, _______, _______, JET_FIND, _______,              TD(TD_LEFT), KC_DOWN, KC_UP,  TD(TD_RIGHT), _______, ACCENT_TREMA,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      _______, _______, _______, _______, _______, _______,                     KC_MS_LEFT,KC_MS_DOWN,KC_MS_UP, KC_MS_RIGHT, _______,  _______,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          _______, _______,KC_MS_BTN2, KC_MS_BTN1, _______, _______
                                      //`--------------------------'  `--------------------------'
  ),

  [_NUM_PADS] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
     _______, _______, _______, _______, _______, _______,                      _______, _______, KC_7,    KC_8,    KC_9, KC_BSPC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      _______, _______, _______, _______, _______, _______,                      _______, _______, KC_4,    KC_5,    KC_6, KC_DOT,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      _______, _______, _______, _______, _______, _______,                      _______, _______, KC_1,    KC_2,    KC_3, KC_0,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          _______, _______, _______,    _______, _______, KC_DOT
                                      //`--------------------------'  `--------------------------'
  ),

  [_ACCENTS_RALT] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      _______, _______, _______ , _______, _______, _______,                    _______, ACCENT_U_AIGU_RALT, ACCENT_I_CIRC_RALT, ACCENT_O_CIRC_RALT, _______, _______,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      _______, ACCENT_A_GRAVE_RALT, _______, _______, _______, _______,         _______,  _______, _______,  _______, _______, _______,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      _______, _______, _______, ACCENT_C_RALT, _______, _______,               _______, _______, _______, _______, _______, _______,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          _______, _______, _______,    _______, _______, _______
                                      //`--------------------------'  `--------------------------'
  ),

  [_ADJUST] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      _______, QWERTY , QWERTY_OSX  , QWERTY_GAMING, _______, _______,                 _______, _______, _______, _______, _______, QK_BOOT,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
     _RGB_TOG, _______, _______, _______, _______, _______,                _______,  _______, _______,  _______, _______, _______,
  //|--------+--------+-     -------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      _______, _______, _______, _______, _______, _______,                _______, _______, _______, _______, _______, _______,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          _______, _______, _______,    _______, _______, _______
                                      //`--------------------------'  `--------------------------'
  )
};

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
        lock_mode = true;
        layer_on(_LOWER);
        update_tri_layer(_LOWER, _RAISE, _ADJUST);
      } else {
        lock_mode = false;
        layer_off(_LOWER);
        update_tri_layer(_LOWER, _RAISE, _ADJUST);
        is_hold_tapdance_disabled = false;
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

    case _RGB_TOG:
      if (record->event.pressed) {
          if (rgb_matrix_is_enabled()) {
              rgb_matrix_disable_noeeprom();
          } else {
              rgb_matrix_enable_noeeprom();
          }
      }
      return false;
      break;

    case KC_LALT:
    case KC_LGUI:
    case KC_LSFT:
      if (record->event.pressed) {
          is_hold_tapdance_disabled = true;
      } else {
          is_hold_tapdance_disabled = false;
      }
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

     case JET_OPTI:
       if (record->event.pressed) {
           register_code(KC_LCTL);
           register_code(KC_LALT);

           tap_code(KC_O);
           unregister_code(KC_LALT);
           unregister_code(KC_LCTL);
       }
       return false;
       break;

     case  JET_FORMAT:
       if (record->event.pressed) {
           register_code(KC_LCTL);
           register_code(KC_LALT);

           tap_code(KC_L);
           unregister_code(KC_LALT);
           unregister_code(KC_LCTL);
       }
       return false;
       break;

     case  JET_FORMAT_OSX:
       if (record->event.pressed) {
           register_code(KC_LALT);
           register_code(KC_LGUI);

           tap_code(KC_L);
           unregister_code(KC_LALT);
           unregister_code(KC_LGUI);
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
      }

      if (keycode == TD(TD_ESC) || keycode == TD(TD_ESC_OSX)) {
          scrolling_mode = record->event.pressed;
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

void keyboard_post_init_user(void) {
    rgb_matrix_disable_noeeprom();

    // default values (true, 1000)
    pointing_device_set_cpi_on_side(true, 80); //Set cpi on left side to a low value for slower scrolling.

    // default values (false 8000)
    pointing_device_set_cpi_on_side(false, 500); //Set cpi on right side to a reasonable value for mousing.
}

//
// trackpad
//

// Modify these values to adjust the scrolling speed
#define SCROLL_DIVISOR_H 8.0
#define SCROLL_DIVISOR_V 8.0

// Variables to store accumulated scroll values
float scroll_accumulated_h = 0;
float scroll_accumulated_v = 0;

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    if (scrolling_mode) {
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
    }

    if (lock_mode) {
        mouse_report.h = 0;
        mouse_report.v = 0;
        mouse_report.x = 0;
        mouse_report.y = 0;
    }
    return mouse_report;
}

report_mouse_t pointing_device_task_combined_user(report_mouse_t left_report, report_mouse_t right_report) {
    left_report.h = left_report.x;
    left_report.v = left_report.y;
    left_report.x = 0;
    left_report.y = 0;

    right_report = pointing_device_task_user(right_report);
    return pointing_device_combine_reports(left_report, right_report);
}

//Lighting
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    for (uint8_t i = led_min; i < led_max; i++) {
      rgb_matrix_set_color(i, 50, 15, 0);
    }

    return false;
}
