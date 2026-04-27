#include QMK_KEYBOARD_H

#include "tapdance.h"

bool is_kc_caps  = false;

bool touched_td = false;

bool scrolling_mode = false;

bool is_hold_tapdance_disabled = false;

void tap_dance_tap_hold_reset(tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    if (tap_hold->held) {
        unregister_code16(tap_hold->held);
        tap_hold->held = 0;
    }
}

void tap_dance_tap_hold_finished(tap_dance_state_t *state, void *user_data) {
    touched_td = false;

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
    touched_td = false;

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

// START tap-hold layout
// is_hold_tapdance_disabled is OWNED exclusively by this pair.
// No other tap dance should ever read or write it.
void tap_dance_tap_hold_finished_layout(tap_dance_state_t *state, void *user_data) {
    touched_td = false;

    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    is_hold_tapdance_disabled = true;

    if (state->pressed) {
        layer_on(tap_hold->hold);
    }
}

void tap_dance_tap_hold_reset_layout(tap_dance_state_t *state, void *user_data) {
    is_hold_tapdance_disabled = false;
}
// END tap-hold layout


// START default tap-dance
int cur_dance (tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed) {
            return SINGLE_TAP;
        }
        else {
            return SINGLE_HOLD;
        }
    }
    else if (state->count == 2) {
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
    else if (state->count == 3) {
        if (state->interrupted) {
            return TRIPLE_SINGLE_TAP;
        }
        else if (state->pressed) {
            return TRIPLE_HOLD;
        }
        else {
            return TRIPLE_TAP;
        }
    }

    return 8;
}

// One state variable per tap dance — avoids cross-contamination when
// multiple tap dances are active simultaneously (e.g. hold RALT + double-hold LALT)
static tap ralt_tap_state     = { .is_press_action = true, .state = 0 };
// dedicated state for OSX variant — was incorrectly sharing ralt_tap_state
static tap ralt_osx_tap_state = { .is_press_action = true, .state = 0 };
static tap lalt_tap_state     = { .is_press_action = true, .state = 0 };
static tap lgui_tap_state     = { .is_press_action = true, .state = 0 };
static tap lctl_tap_state     = { .is_press_action = true, .state = 0 };

void td_ralt_finished (tap_dance_state_t *state, void *user_data) {
    ralt_tap_state.state = cur_dance(state);
    // is_hold_tapdance_disabled is owned by the layout tap dance only
    // touching it here or in _reset would clear it while LOWER/RAISE is still held.

    // guard with state->pressed — quick tap must not spuriously
    // fire KC_RALT + layer_on to the OS
    if (state->pressed) {
        switch (ralt_tap_state.state) {
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
}

void td_ralt_reset (tap_dance_state_t *state, void *user_data) {
    // do NOT touch is_hold_tapdance_disabled here
    switch (ralt_tap_state.state) {
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
    ralt_tap_state.state = 0;
}

void td_ralt_osx_finished (tap_dance_state_t *state, void *user_data) {
    // use dedicated OSX state variable
    ralt_osx_tap_state.state = cur_dance(state);
    // do NOT touch is_hold_tapdance_disabled here

    // guard with state->pressed
    if (state->pressed) {
        switch (ralt_osx_tap_state.state) {
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
}

void td_ralt_osx_reset (tap_dance_state_t *state, void *user_data) {
    // use dedicated OSX state variable
    // do NOT touch is_hold_tapdance_disabled here
    switch (ralt_osx_tap_state.state) {
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
    ralt_osx_tap_state.state = 0;
}

void td_lalt_finished (tap_dance_state_t *state, void *user_data) {
    lalt_tap_state.state = cur_dance(state);
    // do NOT touch is_hold_tapdance_disabled here

    // guard with state->pressed
    if (state->pressed) {
        switch (lalt_tap_state.state) {
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
}

void td_lalt_reset (tap_dance_state_t *state, void *user_data) {
    // do NOT touch is_hold_tapdance_disabled here
    switch (lalt_tap_state.state) {
        case SINGLE_TAP:
        case SINGLE_HOLD:
            unregister_code(KC_LALT);
            break;

        case DOUBLE_SINGLE_TAP:
        case DOUBLE_HOLD:
            layer_off(_NUM_PADS);
            break;
    }
    lalt_tap_state.state = 0;
}

void td_lgui_finished (tap_dance_state_t *state, void *user_data) {
    lgui_tap_state.state = cur_dance(state);
    // do NOT touch is_hold_tapdance_disabled here

    // guard with state->pressed
    if (state->pressed) {
        switch (lgui_tap_state.state) {
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
}

void td_lgui_reset (tap_dance_state_t *state, void *user_data) {
    // do NOT touch is_hold_tapdance_disabled here
    switch (lgui_tap_state.state) {
        case SINGLE_TAP:
        case SINGLE_HOLD:
            unregister_code(KC_LGUI);
            break;

        case DOUBLE_SINGLE_TAP:
        case DOUBLE_HOLD:
            layer_off(_NUM_PADS);
            break;
    }
    lgui_tap_state.state = 0;
}

void td_lctl_finished (tap_dance_state_t *state, void *user_data) {
    lctl_tap_state.state = cur_dance(state);
    // do NOT touch is_hold_tapdance_disabled here

    // guard with state->pressed
    if (state->pressed) {
        switch (lctl_tap_state.state) {
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
}

void td_lctl_reset (tap_dance_state_t *state, void *user_data) {
    switch (lctl_tap_state.state) {
        case SINGLE_TAP:
        case SINGLE_HOLD:
            unregister_code(KC_LCTL);
            break;

        case DOUBLE_SINGLE_TAP:
        case DOUBLE_HOLD:
            unregister_code(KC_LALT);
            break;
    }
    lctl_tap_state.state = 0;
}
