#include QMK_KEYBOARD_H
#include "tapdance.h"

bool is_hold_tapdance_disabled = false;

bool touched_td = false;

//instanalize an instance of 'tap' for the 'x' tap dance.
static tap xtap_state = {
  .is_press_action = true,
  .state = 0
};

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

// allow call multiple tap dance simultaneously
// e.g: TD_DEL/TD_DEL_OSX
void tap_dance_tap_hold_finished_unprotected(tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    if (state->pressed) {
        if (state->count == 1
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

void tap_dance_tap_hold_finished_permissive_layout(tap_dance_state_t *state, void *user_data) {
    touched_td = false;

    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    is_hold_tapdance_disabled = true;

    if (state->pressed) {
        if (state->interrupted) {
            tap_code16(tap_hold->tap);
        } else {
            layer_on(tap_hold->hold);
        }
    }
}

void tap_dance_tap_hold_reset_layout(tap_dance_state_t *state, void *user_data) {
    is_hold_tapdance_disabled = false;
}
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

int cur_dance_permissive (tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || state->pressed) {
            return SINGLE_HOLD;
        } else {
            return SINGLE_TAP;
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

void td_raise_finished (tap_dance_state_t *state, void *user_data) {
  xtap_state.state = cur_dance_permissive(state);
  is_hold_tapdance_disabled = false;

  // quick fix to remove dupplicate RALT
  unregister_code(KC_LALT);
  unregister_code(KC_LGUI);

  switch (xtap_state.state) {
      case SINGLE_TAP:
          register_code(KC_BSPC);
          break;

      case SINGLE_HOLD:
          layer_on(_RAISE);
          break;

      case DOUBLE_SINGLE_TAP:
      case DOUBLE_HOLD:
          register_code(KC_RALT);
          break;
  }
}

void td_raise_reset (tap_dance_state_t *state, void *user_data) {
    is_hold_tapdance_disabled = false;

    switch (xtap_state.state) {
        case SINGLE_TAP:
          unregister_code(KC_BSPC);
          break;

        case SINGLE_HOLD:
            layer_off(_RAISE);
            break;

        case DOUBLE_SINGLE_TAP:
        case DOUBLE_HOLD:
            unregister_code(KC_RALT);
            break;
    }
    xtap_state.state = 0;
}

void td_lctl_finished (tap_dance_state_t *state, void *user_data) {
  xtap_state.state = cur_dance(state);
  is_hold_tapdance_disabled = false;

  switch (xtap_state.state) {
      case SINGLE_TAP:
      case SINGLE_HOLD:
          register_code(KC_LCTL);
          break;

      case DOUBLE_SINGLE_TAP:
      case DOUBLE_HOLD:
          register_code(KC_LALT);
          break;
  }
}

void td_lctl_reset (tap_dance_state_t *state, void *user_data) {
    is_hold_tapdance_disabled = false;

    switch (xtap_state.state) {
        case SINGLE_TAP:
        case SINGLE_HOLD:
            unregister_code(KC_LCTL);
            break;

        case DOUBLE_SINGLE_TAP:
        case DOUBLE_HOLD:
            unregister_code(KC_LALT);
            break;
    }
    xtap_state.state = 0;
}
