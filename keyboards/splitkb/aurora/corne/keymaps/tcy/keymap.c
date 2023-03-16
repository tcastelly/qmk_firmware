#include QMK_KEYBOARD_H

enum layer_names {
    _QWERTY,
    _QWERTY_LINUX,
    _LOWER,
    _RAISE,
    _ADJUST,
    _ESC,
    _ESC_LINUX,
    _NUM_PADS,
    _ACCENTS_RALT
};

enum custom_keycodes {
  QWERTY = SAFE_RANGE,
  QWERTY_LINUX,
  LOWER,
  RAISE,
  ADJUST,
  ESC,
  ACCENT_GRAVE,
  ACCENT_CIRCUM,
  ACCENT_TREMA,
  ACCENT_E_GRAVE,
  ACCENT_A_GRAVE,

  // to be used with RALT already pressed
  ACCENT_I_CIRC_RALT,
  ACCENT_O_CIRC_RALT,
  ACCENT_U_AIGU_RALT,
  ACCENT_C_RALT,
  ACCENT_A_GRAVE_RALT
};

typedef struct {
  bool is_press_action;
  int state;
} tap;

// default tap dance
enum {
  SINGLE_TAP = 1,
  SINGLE_HOLD = 2, // should use tap_dance_tap_hold_t instead
  DOUBLE_TAP = 3,
  DOUBLE_HOLD = 4,
  DOUBLE_SINGLE_TAP = 5, //send two single taps
};

// "tap-hold"
typedef struct {
    uint16_t tap;
    uint16_t hold;
    uint16_t held;
} tap_dance_tap_hold_t;

// custom tap dance
enum {
    TD_ESC,
    TD_ESC_LINUX,
    TD_TAB,
    TD_O,
    TD_P,
    TD_L,
    TD_SCLN,
    TD_LGUI,
    TD_RALT,
    TD_LALT,
    TD_RALT_LIN
};

bool is_hold_tapdance_disabled = false;

// switch off the power light of the liatris controller
void keyboard_pre_init_user(void) {
  // Set our LED pin as output
  setPinOutput(24);
  // Turn the LED off
  // (Due to technical reasons, high is off and low is on)
  writePinHigh(24);
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_QWERTY_LINUX] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      TD(TD_TAB),  KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,                         KC_Y,    KC_U,    KC_I,TD(TD_O),TD(TD_P), KC_BSPC,
  //|--------+--------+-------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      TD(TD_ESC_LINUX),KC_A,KC_S,KC_D,  KC_F,     KC_G,                          KC_H,    KC_J,    KC_K,TD(TD_L),TD(TD_SCLN), KC_QUOT,
  //|--------+---- ----+-------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LSFT,     KC_Z,   KC_X,    KC_C,    KC_V,    KC_B,                         KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH, KC_ENT,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                        KC_LCTL,TD(TD_LALT),LOWER,    KC_SPC,   RAISE, TD(TD_RALT_LIN)
                                      //`--------------------------'  `--------------------------'
  ),

  [_QWERTY] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      TD(TD_TAB),  KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,                         KC_Y,    KC_U,    KC_I,TD(TD_O),TD(TD_P), KC_BSPC,
  //|--------+--------+-------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      TD(TD_ESC),  KC_A,   KC_S,    KC_D,  KC_F,     KC_G,                          KC_H,    KC_J,    KC_K,TD(TD_L),TD(TD_SCLN), KC_QUOT,
  //|--------+---- ----+-------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LSFT,     KC_Z,   KC_X,    KC_C,    KC_V,    KC_B,                         KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH, KC_ENT,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                        KC_LCTL,TD(TD_LGUI),LOWER,    KC_SPC,   RAISE, TD(TD_RALT)
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
                                          _______, _______, _______,    _______, _______, _______
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
                                          _______, _______, _______,    _______, _______, _______
                                      //`--------------------------'  `--------------------------'
  ),

  [_ESC] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      ACCENT_GRAVE, ACCENT_GRAVE, _______, ACCENT_E_GRAVE, _______, _______,    ACCENT_CIRCUM, KC_WH_D, KC_WH_U, _______, _______, KC_DEL,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      _______, ACCENT_A_GRAVE, _______, _______, _______, _______,         KC_LEFT, KC_DOWN, KC_UP,  KC_RIGHT, _______, ACCENT_TREMA,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      _______, _______, _______, _______, _______, _______,                      KC_MS_LEFT,KC_MS_DOWN,KC_MS_UP, KC_MS_RIGHT, _______,  _______,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          _______, _______,KC_MS_BTN1, KC_MS_BTN2, _______, _______
                                      //`--------------------------'  `--------------------------'
  ),

  [_ESC_LINUX] = LAYOUT_split_3x6_3(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      ACCENT_GRAVE, ACCENT_GRAVE, _______, ACCENT_E_GRAVE, _______, _______,   ACCENT_CIRCUM, KC_WH_D, KC_WH_U, _______, _______, KC_DEL,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      _______, ACCENT_A_GRAVE, _______, _______, _______, _______,              KC_LEFT, KC_DOWN, KC_UP,  KC_RIGHT, _______, ACCENT_TREMA,
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
      _______, DF(QWERTY), DF(QWERTY_LINUX) , _______, _______, _______,                 _______, _______, _______, _______, _______, QK_BOOT,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      _______, _______,      _______, _______, _______, _______,                _______,  _______, _______,  _______, _______, _______,
  //|--------+--------+-     -------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      _______, _______,      _______, _______, _______, _______,                _______, _______, _______, _______, _______, _______,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          _______, _______, _______,    _______, _______, _______
                                      //`--------------------------'  `--------------------------'
  )
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  tap_dance_action_t *action;

  switch (keycode) {
    case LOWER:
      if (record->event.pressed) {
        is_hold_tapdance_disabled = true;
        layer_on(_LOWER);
        update_tri_layer(_LOWER, _RAISE, _ADJUST);
      } else {
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

    case TD(TD_O):  // list all tap dance keycodes with tap-hold configurations
    case TD(TD_ESC):
    case TD(TD_ESC_LINUX):
    case TD(TD_TAB):
    case TD(TD_P):
    case TD(TD_L):
    case TD(TD_SCLN):
      if ((keycode == TD(TD_ESC) || keycode == TD(TD_ESC_LINUX)) && !record->event.pressed) {
          layer_off(_ESC);
          layer_off(_ESC_LINUX);
          is_hold_tapdance_disabled = false;
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

void tap_dance_tap_hold_reset(tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    if (tap_hold->held) {
        unregister_code16(tap_hold->held);
        tap_hold->held = 0;
    }
}

void tap_dance_tap_hold_finished(tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    if (state->pressed) {
        if (state->count == 1
            && !is_hold_tapdance_disabled
#ifndef PERMISSIVE_HOLD
            && !state->interrupted
#endif
        ) {
            register_code16(tap_hold->hold);
            tap_hold->held = tap_hold->hold;
        } else {
            register_code16(tap_hold->tap);
            tap_hold->held = tap_hold->tap;
        }
    }
}

// START tap-hold
void tap_dance_tap_hold_finished_layout(tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    is_hold_tapdance_disabled = true;

    if (state->pressed) {
        layer_on(tap_hold->hold);
    }
}

void tap_dance_tap_hold_reset_layout(tap_dance_state_t *state, void *user_data) {
    is_hold_tapdance_disabled = false;
}

#define ACTION_TAP_DANCE_TAP_HOLD(tap, hold) \
    { .fn = {NULL, tap_dance_tap_hold_finished, tap_dance_tap_hold_reset}, .user_data = (void *)&((tap_dance_tap_hold_t){tap, hold, 0}), }

#define ACTION_TAP_DANCE_TAP_HOLD_LAYOUT(tap, hold) \
    { .fn = {NULL, tap_dance_tap_hold_finished_layout, tap_dance_tap_hold_reset_layout}, .user_data = (void *)&((tap_dance_tap_hold_t){tap, hold, 0}), }
// END tap-hold


// START default tap-dance
int cur_dance (tap_dance_state_t *state) {
  if (state->count == 1) {
    //key has not been interrupted, but they key is still held. Means you want to send a 'HOLD'.
    if (state->interrupted || !state->pressed) {
        return SINGLE_TAP;
    }
    //key has not been interrupted, but they key is still held. Means you want to send a 'HOLD'.
    else {
        return SINGLE_HOLD;
    }
  }
  else if (state->count == 2) {
    /*
     * DOUBLE_SINGLE_TAP is to distinguish between typing "pepper", and actually wanting a double tap
     * action when hitting 'pp'. Suggested use case for this return value is when you want to send two
     * keystrokes of the key, and not the 'double tap' action/macro.
    */
    if (state->interrupted) {
        return DOUBLE_SINGLE_TAP;
    }
    else if (state->pressed) {
        return DOUBLE_HOLD;
    }
    else {
        return DOUBLE_TAP;
    }
  }

  //magic number. At some point this method will expand to work for more presses
  return 8;
}

//instanalize an instance of 'tap' for the 'x' tap dance.
static tap xtap_state = {
  .is_press_action = true,
  .state = 0
};

void td_ralt_lin_finished (tap_dance_state_t *state, void *user_data) {
  xtap_state.state = cur_dance(state);
  is_hold_tapdance_disabled = false;

  switch (xtap_state.state) {
      case SINGLE_TAP:
      case SINGLE_HOLD:
          register_code(KC_RALT);
          layer_on(_ACCENTS_RALT);
          break;

      case DOUBLE_SINGLE_TAP:
      case DOUBLE_HOLD:
          register_code(KC_LCTL);
          break;
  }
}

void td_ralt_lin_reset (tap_dance_state_t *state, void *user_data) {
    is_hold_tapdance_disabled = false;

    switch (xtap_state.state) {
        case SINGLE_TAP:
        case SINGLE_HOLD:
            unregister_code(KC_RALT);
            layer_off(_ACCENTS_RALT);
            break;

        case DOUBLE_SINGLE_TAP:
        case DOUBLE_HOLD:
            unregister_code(KC_LCTL);
            break;
    }
    xtap_state.state = 0;
}

void td_ralt_finished (tap_dance_state_t *state, void *user_data) {
  xtap_state.state = cur_dance(state);
  is_hold_tapdance_disabled = false;

  switch (xtap_state.state) {
      case SINGLE_TAP:
      case SINGLE_HOLD:
          register_code(KC_RALT);
          layer_on(_ACCENTS_RALT);
          break;

      case DOUBLE_SINGLE_TAP:
      case DOUBLE_HOLD:
          register_code(KC_LGUI);
          break;
  }
}

void td_ralt_reset (tap_dance_state_t *state, void *user_data) {
    is_hold_tapdance_disabled = false;

    switch (xtap_state.state) {
        case SINGLE_TAP:
        case SINGLE_HOLD:
            unregister_code(KC_RALT);
            layer_off(_ACCENTS_RALT);
            break;

        case DOUBLE_SINGLE_TAP:
        case DOUBLE_HOLD:
            unregister_code(KC_LGUI);
            break;
    }
    xtap_state.state = 0;
}

void td_lalt_finished (tap_dance_state_t *state, void *user_data) {
  xtap_state.state = cur_dance(state);
  is_hold_tapdance_disabled = false;

  switch (xtap_state.state) {
      case SINGLE_TAP:
      case SINGLE_HOLD:
          register_code(KC_LALT);
          break;

      case DOUBLE_SINGLE_TAP:
      case DOUBLE_HOLD:
          layer_on(_NUM_PADS);
          break;
  }
}

void td_lalt_reset (tap_dance_state_t *state, void *user_data) {
    is_hold_tapdance_disabled = false;

    switch (xtap_state.state) {
        case SINGLE_TAP:
        case SINGLE_HOLD:
            unregister_code(KC_LALT);
            break;

        case DOUBLE_SINGLE_TAP:
        case DOUBLE_HOLD:
            layer_off(_NUM_PADS);
            break;
    }
    xtap_state.state = 0;
}

void td_lgui_finished (tap_dance_state_t *state, void *user_data) {
  xtap_state.state = cur_dance(state);
  is_hold_tapdance_disabled = false;

  switch (xtap_state.state) {
      case SINGLE_TAP:
      case SINGLE_HOLD:
          register_code(KC_LGUI);
          break;

      case DOUBLE_SINGLE_TAP:
      case DOUBLE_HOLD:
          layer_on(_NUM_PADS);
          break;
  }
}

void td_lgui_reset (tap_dance_state_t *state, void *user_data) {
    is_hold_tapdance_disabled = false;

    switch (xtap_state.state) {
        case SINGLE_TAP:
        case SINGLE_HOLD:
            unregister_code(KC_LGUI);
            break;

        case DOUBLE_SINGLE_TAP:
        case DOUBLE_HOLD:
            layer_off(_NUM_PADS);
            break;
    }
    xtap_state.state = 0;
}
// END default tap-dance

// Associate our tap dance key with its functionality
tap_dance_action_t tap_dance_actions[] = {
    [TD_ESC] = ACTION_TAP_DANCE_TAP_HOLD_LAYOUT(KC_ESC, _ESC),
    [TD_ESC_LINUX] = ACTION_TAP_DANCE_TAP_HOLD_LAYOUT(KC_ESC, _ESC_LINUX),
    [TD_TAB] = ACTION_TAP_DANCE_TAP_HOLD(KC_TAB, KC_TILD),
    [TD_O] = ACTION_TAP_DANCE_TAP_HOLD(KC_O, KC_LPRN),
    [TD_P] = ACTION_TAP_DANCE_TAP_HOLD(KC_P, KC_RPRN),
    [TD_L] = ACTION_TAP_DANCE_TAP_HOLD(KC_L, KC_LCBR),
    [TD_SCLN] = ACTION_TAP_DANCE_TAP_HOLD(KC_SCLN, KC_RCBR),

    // same tap-dance
    // enable it for osx and linux
    [TD_LALT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_lalt_finished, td_lalt_reset),
    [TD_LGUI] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_lgui_finished, td_lgui_reset),

    [TD_RALT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_ralt_finished, td_ralt_reset),
    [TD_RALT_LIN] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_ralt_lin_finished, td_ralt_lin_reset)
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
