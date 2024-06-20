#include QMK_KEYBOARD_H
#include "tapdance.c"
#include "lib/lib8tion/lib8tion.h"

static bool scrolling_mode = false;

static bool lock_mode = false;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_QWERTY] = LAYOUT_split_3x6_3(
  //,---------------------------------------------------            ,-----------------------------------------------------.
      TD(TD_TAB),  KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,                         KC_Y,    KC_U,    KC_I,TD(TD_O),TD(TD_P), TD(TD_BSPC),
  //|--------+--------+-------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      TD(TD_ESC),  TD(TD_A),   KC_S,    KC_D,    KC_F,    KC_G,                         KC_H,    KC_J,    KC_K,TD(TD_L),TD(TD_SCLN), KC_QUOT,
  //|--------+---- ----+-------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LSFT,     KC_Z,   KC_X,    KC_C,    KC_V,    KC_B,                         KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH, TD(TD_ENT),
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                        KC_LCTL,TD(TD_LALT),LOWER,    KC_SPC,   TD(TD_RAISE), TD(TD_RALT)
                                      //`--------------------------'  `--------------------------'
  ),

  [_QWERTY_OSX] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      TD(TD_TAB),  KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,                         KC_Y,    KC_U,    KC_I,TD(TD_O),TD(TD_P), TD(TD_BSPC_OSX),
  //|--------+--------+-------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      TD(TD_ESC_OSX), TD(TD_A_OSX), KC_S,KC_D, KC_F,  KC_G,                          KC_H,    KC_J,    KC_K,TD(TD_L),TD(TD_SCLN), KC_QUOT,
  //|--------+---- ----+-------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LSFT,     KC_Z,   KC_X,    KC_C,    KC_V,    KC_B,                         KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH, TD(TD_ENT),
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                        TD(TD_LCTL),TD(TD_LGUI),LOWER,    KC_SPC,   TD(TD_RAISE), TD(TD_RALT_OSX)
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
                                          _______, _______, _______,    KC_MS_BTN1, _______, _______
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
                                          _______, KC_LALT, _______,    _______, _______, _______
                                      //`--------------------------'  `--------------------------'
  ),

  [_MOD_OSX] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      ACCENT_GRAVE, ACCENT_GRAVE, _______, ACCENT_E_GRAVE, JET_RNM, _______,    ACCENT_CIRCUM, KC_WH_D, KC_WH_U, JET_OPTI, JET_FORMAT_OSX, TD(TD_DEL_OSX),
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      _______, ACCENT_A_GRAVE, _______, _______, JET_FIND, _______,              TD(TD_LEFT_OSX), KC_DOWN, KC_UP,  TD(TD_RIGHT_OSX), _______, ACCENT_TREMA,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      _______, _______, _______, _______, _______, _______,                     KC_MS_LEFT,KC_MS_DOWN,KC_MS_UP, KC_MS_RIGHT, _______,  _______,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          _______, KC_LCTL,KC_MS_BTN2, KC_MS_BTN1, TD(TD_RALT), _______
                                      //`--------------------------'  `--------------------------'
  ),

  [_MOD] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      ACCENT_GRAVE, ACCENT_GRAVE, _______, ACCENT_E_GRAVE, JET_RNM, _______,   ACCENT_CIRCUM, KC_WH_D, KC_WH_U, JET_OPTI, JET_FORMAT, TD(TD_DEL),
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      _______, ACCENT_A_GRAVE, _______, _______, JET_FIND, _______,              TD(TD_LEFT), KC_DOWN, KC_UP,  TD(TD_RIGHT), _______, ACCENT_TREMA,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      _______, _______, _______, _______, _______, _______,                     KC_MS_LEFT,KC_MS_DOWN,KC_MS_UP, KC_MS_RIGHT, _______,  _______,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          _______, KC_LCTL,KC_MS_BTN2, KC_MS_BTN1, TD(TD_RALT_OSX), _______
                                      //`--------------------------'  `--------------------------'
  ),

  [_ACCENTS_RALT] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      _______, _______, _______ , _______, _______, ACCENT_EURO,                 _______, ACCENT_U_AIGU_RALT, ACCENT_I_CIRC_RALT, ACCENT_O_CIRC_RALT, _______, _______,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      _______, ACCENT_A_GRAVE_RALT, _______, _______, _______, _______,         _______,  _______, _______,  _______, _______, _______,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      _______, _______, _______, ACCENT_C_RALT, _______, _______,               _______, _______, _______, _______, _______, _______,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          _______, KC_LALT, _______,    _______, _______, _______
                                      //`--------------------------'  `--------------------------'
  ),

  [_ADJUST] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      _______, QWERTY , QWERTY_OSX  , QWERTY_GAMING, _______, _______,          _______, _______, _______, _______, _______, QK_BOOT,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
     _______, _______, _______, _______, _______, _______,                      _______,  _______, _______,  _______, _______, _______,
  //|--------+--------+-     -------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      _______, _______, _______, _______, _______, _______,                     _______, _______, _______, _______, _______, _______,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          _______, _______, _______,    _______, _______, _______
                                      //`--------------------------'  `--------------------------'
  )
};

#ifdef ENCODER_ENABLE

bool encoder_update_user(uint8_t index, bool clockwise) {
    switch (get_highest_layer(layer_state)) {
        // both QWERTY AND QWERTY_OSX scroll with the right encoder
        case _QWERTY_OSX:
            if (index == 0) {
                if (clockwise) {
                    register_code(KC_LCTL);
                    tap_code(KC_LEFT);
                    unregister_code(KC_LCTL);
                } else {
                    register_code(KC_LCTL);
                    tap_code(KC_RIGHT);
                    unregister_code(KC_LCTL);
                }
            }
            else {
                if (clockwise) {
                    tap_code(KC_MS_WH_UP);
                } else {
                    tap_code(KC_MS_WH_DOWN);
                }
            }
            return false;

        // on default QWERTY (Linux) left encoder will be used to change active "window"
        // simial to ALT/TAB
        case _QWERTY:
            if (index == 0) {
                if (clockwise) {
                    register_code(KC_LALT);
                    register_code(KC_LEFT);

                    unregister_code(KC_LALT);
                    unregister_code(KC_LEFT);
                } else {
                    register_code(KC_LALT);
                    register_code(KC_RIGHT);

                    unregister_code(KC_LALT);
                    unregister_code(KC_RIGHT);
                }
            }
            else {
                if (clockwise) {
                    tap_code(KC_MS_WH_DOWN);
                } else {
                    tap_code(KC_MS_WH_UP);
                }
            }
            return false;

        // on default QWERTY (Linux) and ESC left encoder will be used to resize active "window"
        case _MOD:
            if (index == 0) {
                if (clockwise) {
                    register_code(KC_LCTL);
                    register_code(KC_LSFT);
                    register_code(KC_LEFT);

                    unregister_code(KC_LEFT);
                    unregister_code(KC_LSFT);
                    unregister_code(KC_LCTL);
                } else {
                    register_code(KC_LCTL);
                    register_code(KC_LSFT);
                    register_code(KC_RIGHT);

                    unregister_code(KC_RIGHT);
                    unregister_code(KC_LSFT);
                    unregister_code(KC_LCTL);
                }
            } else {
                if (clockwise) {
                    register_code(KC_LCTL);
                    register_code(KC_LSFT);
                    register_code(KC_DOWN);

                    unregister_code(KC_DOWN);
                    unregister_code(KC_LSFT);
                    unregister_code(KC_LCTL);
                } else {
                    register_code(KC_LCTL);
                    register_code(KC_LSFT);
                    register_code(KC_UP);

                    unregister_code(KC_UP);
                    unregister_code(KC_LSFT);
                    unregister_code(KC_LCTL);
                }
            }
            return false;

        case _LOWER:
        case _RAISE:
        case _MOD_OSX:
            if (index == 0) {
                if (clockwise) {
                    tap_code(KC_PGDN);
                } else {
                    tap_code(KC_PGUP);
                }
            }
            else {
                if (clockwise) {
                    tap_code(KC_VOLD);
                } else {
                    tap_code(KC_VOLU);
                }
            }
            return false;
    }

    return true;
}

#endif

// Associate our tap dance key with its functionality
tap_dance_action_t tap_dance_actions[] = {
    [TD_A] = ACTION_TAP_DANCE_TAP_HOLD(KC_A, KC_LCTL),
    [TD_A_OSX] = ACTION_TAP_DANCE_TAP_HOLD(KC_A, KC_LCTL),
    [TD_ESC] = ACTION_TAP_DANCE_TAP_HOLD_LAYOUT(KC_ESC, _MOD),
    [TD_ESC_OSX] = ACTION_TAP_DANCE_TAP_HOLD_LAYOUT(KC_ESC, _MOD_OSX),
    [TD_TAB] = ACTION_TAP_DANCE_TAP_HOLD(KC_TAB, KC_TILD),
    [TD_O] = ACTION_TAP_DANCE_TAP_HOLD(KC_O, KC_LPRN),
    [TD_P] = ACTION_TAP_DANCE_TAP_HOLD(KC_P, KC_RPRN),
    [TD_L] = ACTION_TAP_DANCE_TAP_HOLD(KC_L, KC_LCBR),
    [TD_SCLN] = ACTION_TAP_DANCE_TAP_HOLD(KC_SCLN, KC_RCBR),
    [TD_ENT] = ACTION_TAP_DANCE_TAP_HOLD(KC_ENT, KC_LSFT),
    [TD_RAISE] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_raise_finished, td_raise_reset),

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
      touched_td = true;
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
      touched_td = true;
      break;

    case ACCENT_GRAVE:
      if (record->event.pressed) {
          register_code(KC_RALT);
          register_code(KC_GRV);
      } else {
          unregister_code(KC_GRV);
          unregister_code(KC_RALT);
      }
      touched_td = true;
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
      touched_td = true;
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
       touched_td = true;
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
       touched_td = true;
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
       touched_td = true;
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
       touched_td = true;
       break;

    case ACCENT_EURO:
       if (record->event.pressed) {
           register_code(KC_5);
       } else {
           unregister_code(KC_5);
           unregister_code(KC_RALT);

           // will be unregister by `td_ralt_reset`
           register_code(KC_RALT);
       }

       touched_td = true;
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
       touched_td = true;
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
       touched_td = true;
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

    case TD(TD_O):  // list all tap dance keycodes with tap-hold configurations
    case TD(TD_A):
    case TD(TD_A_OSX):
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
          layer_off(_MOD);
          layer_off(_MOD_OSX);
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
      if ((keycode == TD(TD_A) || keycode == TD(TD_A_OSX) || keycode == TD(TD_SPC)) && !touched_td && !record->event.pressed && action->state.finished) {
          unregister_code(KC_LCTL);
          tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)action->user_data;
          tap_code16(tap_hold->tap);
      }
      touched_td = true;
      break;
  }

  touched_td = true;
  return true;
}

uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case TD(TD_LGUI):
        case TD(TD_LALT):
            return TAPPING_TERM + 150;
        case TD(TD_A):
            return TAPPING_TERM + 25;
            break;
        default:
            return TAPPING_TERM;
    }
}

#ifdef OLED_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    return OLED_ROTATION_270;
    return rotation;
}

bool oled_task_user(){
    // This stores the image content
    static const char hello_world_img [] PROGMEM = {
        // your image 2 cpp content
        // 'hello_world_corne_final', 32x128px
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xe0, 0xe0, 0xe0, 0x00, 0x00,
        0xc0, 0xf0, 0xf8, 0xfc, 0xfc, 0xfe, 0xfe, 0xfe, 0xfe, 0xfc, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0xc0, 0xf0, 0xf8, 0xfc, 0xe8, 0x00, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0xc0, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x1f, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x0f, 0x1f, 0x3f, 0x7f, 0x7e, 0x7f, 0x3f, 0x3f, 0x7f, 0x7f, 0xff, 0xff, 0x7f, 0x7f,
        0x3f, 0x3f, 0x7f, 0x7f, 0xff, 0xff, 0xff, 0xf8, 0xf0, 0xf0, 0x70, 0x78, 0x1c, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xf0, 0x00, 0x00, 0xf0, 0xf0, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xf0, 0xf0, 0x00, 0x00, 0xf0, 0xf0, 0x00, 0x80, 0xf0, 0x70, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x06, 0x06, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, 0xff, 0x0f, 0x3f, 0xfc, 0xff, 0x1f, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x63, 0x63, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff, 0x03, 0x03, 0xfe, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x0f, 0x0c, 0x0c, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0f, 0x0c, 0x0c, 0x0f, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xe0, 0x60, 0x60, 0xc0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x0c, 0x0c, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x1f, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x1f, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xc0, 0xc0, 0xc0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x74, 0xff, 0xdf, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    // this displays the image
    oled_write_raw_P(hello_world_img, sizeof(hello_world_img));

    return false;
}
#endif


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

