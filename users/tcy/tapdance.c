#include QMK_KEYBOARD_H

#include "tapdance.h"
#include "tcy.h"

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

// OSX-aware word navigation tap dances.
// is_osx selects Ctrl (Linux) vs Alt (macOS) for word jumps/deletions.
static uint16_t word_bspc_held  = 0;
static uint16_t word_del_held   = 0;
static uint16_t word_left_held  = 0;
static uint16_t word_right_held = 0;

void td_word_bspc_finished(tap_dance_state_t *state, void *user_data) {
    touched_td = false;
    if (state->pressed) {
        if (state->count == 1 && !is_hold_tapdance_disabled
#ifndef PERMISSIVE_HOLD
            && !state->interrupted
#endif
        ) {
            word_bspc_held = is_osx ? LALT(KC_BSPC) : LCTL(KC_BSPC);
        } else {
            word_bspc_held = KC_BSPC;
        }
        register_code16(word_bspc_held);
    }
}
void td_word_bspc_reset(tap_dance_state_t *state, void *user_data) {
    unregister_code16(word_bspc_held);
    word_bspc_held = 0;
}

void td_word_del_finished(tap_dance_state_t *state, void *user_data) {
    touched_td = false;
    if (state->pressed) {
        if (state->count == 1
#ifndef PERMISSIVE_HOLD
            && !state->interrupted
#endif
        ) {
            word_del_held = is_osx ? LALT(KC_DEL) : LCTL(KC_DEL);
        } else {
            word_del_held = KC_DEL;
        }
        register_code16(word_del_held);
    }
}
void td_word_del_reset(tap_dance_state_t *state, void *user_data) {
    unregister_code16(word_del_held);
    word_del_held = 0;
}

void td_word_left_finished(tap_dance_state_t *state, void *user_data) {
    touched_td = false;
    if (state->pressed) {
        if (state->count == 1
#ifndef PERMISSIVE_HOLD
            && !state->interrupted
#endif
        ) {
            word_left_held = is_osx ? LALT(KC_LEFT) : LCTL(KC_LEFT);
        } else {
            word_left_held = KC_LEFT;
        }
        register_code16(word_left_held);
    }
}
void td_word_left_reset(tap_dance_state_t *state, void *user_data) {
    unregister_code16(word_left_held);
    word_left_held = 0;
}

void td_word_right_finished(tap_dance_state_t *state, void *user_data) {
    touched_td = false;
    if (state->pressed) {
        if (state->count == 1
#ifndef PERMISSIVE_HOLD
            && !state->interrupted
#endif
        ) {
            word_right_held = is_osx ? LALT(KC_RIGHT) : LCTL(KC_RIGHT);
        } else {
            word_right_held = KC_RIGHT;
        }
        register_code16(word_right_held);
    }
}
void td_word_right_reset(tap_dance_state_t *state, void *user_data) {
    unregister_code16(word_right_held);
    word_right_held = 0;
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
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;
    layer_off(tap_hold->hold);
    is_hold_tapdance_disabled = false;
}
// END tap-hold layout


// START default tap-dance
// Like cur_dance but ignores interrupted for the single-tap/hold decision.
// Required for LOWER/RAISE: holding one while pressing the other sets
// interrupted=true, which would wrongly collapse SINGLE_HOLD → SINGLE_TAP.
int cur_dance_permissive (tap_dance_state_t *state) {
    if (state->count == 1) {
        if (!state->pressed) {
            return SINGLE_TAP;
        } else {
            return SINGLE_HOLD;
        }
    } else if (state->count == 2) {
        if (state->interrupted) {
            return DOUBLE_SINGLE_TAP;
        } else if (state->pressed) {
            return DOUBLE_HOLD;
        } else {
            return DOUBLE_TAP;
        }
    } else if (state->count == 3) {
        if (state->interrupted) {
            return TRIPLE_SINGLE_TAP;
        } else if (state->pressed) {
            return TRIPLE_HOLD;
        } else {
            return TRIPLE_TAP;
        }
    }
    return 8;
}

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
static tap lower_tap_state    = { .is_press_action = true, .state = 0 };
static tap raise_tap_state    = { .is_press_action = true, .state = 0 };

void td_ralt_finished (tap_dance_state_t *state, void *user_data) {
    ralt_tap_state.state = cur_dance(state);
    // is_hold_tapdance_disabled is owned by the layout tap dance only
    // touching it here or in _reset would clear it while LOWER/RAISE is still held.

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

void td_ralt_reset (tap_dance_state_t *state, void *user_data) {
    layer_off(_ACCENTS_RALT);
    switch (ralt_tap_state.state) {
        case SINGLE_TAP:
        case SINGLE_HOLD:
            unregister_code(KC_RALT);
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

void td_ralt_osx_reset (tap_dance_state_t *state, void *user_data) {
    // use dedicated OSX state variable
    // do NOT touch is_hold_tapdance_disabled here
    layer_off(_ACCENTS_RALT);
    switch (ralt_osx_tap_state.state) {
        case SINGLE_TAP:
        case SINGLE_HOLD:
            unregister_code(KC_RALT);
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

void td_lalt_reset (tap_dance_state_t *state, void *user_data) {
    // do NOT touch is_hold_tapdance_disabled here
    // Both calls are no-ops when not active; calling unconditionally prevents
    // any state mismatch from leaving KC_LALT registered or _NUM_PADS stuck on.
    unregister_code(KC_LALT);
    layer_off(_NUM_PADS);
    lalt_tap_state.state = 0;
}

void td_lgui_finished (tap_dance_state_t *state, void *user_data) {
    lgui_tap_state.state = cur_dance(state);
    // do NOT touch is_hold_tapdance_disabled here

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

void td_lgui_reset (tap_dance_state_t *state, void *user_data) {
    // do NOT touch is_hold_tapdance_disabled here
    // Unconditional unregister/layer_off: no-op if not active, but
    // prevents a stuck modifier/layer if a macro (e.g. JET_*) or
    // another code path toggled the modifier bit underneath us.
    // See IA_FIX.md.
    unregister_code(KC_LGUI);
    layer_off(_NUM_PADS);
    lgui_tap_state.state = 0;
}

void td_lctl_finished (tap_dance_state_t *state, void *user_data) {
    lctl_tap_state.state = cur_dance(state);
    // do NOT touch is_hold_tapdance_disabled here

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

void td_lctl_reset (tap_dance_state_t *state, void *user_data) {
    // Both calls are no-ops when not active; calling unconditionally
    // prevents any state mismatch from leaving KC_LCTL or KC_LALT
    // registered if a macro toggled the modifier bits underneath us.
    // See IA_FIX.md.
    unregister_code(KC_LCTL);
    unregister_code(KC_LALT);
    lctl_tap_state.state = 0;
}

void td_lower_finished (tap_dance_state_t *state, void *user_data) {
    lower_tap_state.state = cur_dance_permissive(state);

    switch (lower_tap_state.state) {
        case SINGLE_HOLD:
            bootloader_timer = timer_read();
            bootloader_active = true;
            is_hold_tapdance_disabled = true;
            lock_mode = true;
            layer_on(_LOWER);
            update_tri_layer(_LOWER, _RAISE, _ADJUST);
            break;

        case SINGLE_TAP:
            break;

        case DOUBLE_SINGLE_TAP:
        case DOUBLE_HOLD:
            register_code(KC_LCTL);
            break;
    }
}

void td_lower_reset (tap_dance_state_t *state, void *user_data) {
    switch (lower_tap_state.state) {
        case SINGLE_HOLD:
            bootloader_active = false;
            bootloader_timer = 0;
            lock_mode = false;
            layer_off(_LOWER);
            update_tri_layer(_LOWER, _RAISE, _ADJUST);
            is_hold_tapdance_disabled = false;
            break;

        case SINGLE_TAP:
            break;

        case DOUBLE_SINGLE_TAP:
        case DOUBLE_HOLD:
            unregister_code(KC_LCTL);
            break;
    }
    lower_tap_state.state = 0;
}

void td_raise_finished (tap_dance_state_t *state, void *user_data) {
    raise_tap_state.state = cur_dance_permissive(state);

    switch (raise_tap_state.state) {
        case SINGLE_TAP:
            register_code(KC_BSPC);
            break;

        case SINGLE_HOLD:
            bootloader_timer = timer_read();
            bootloader_active = true;
            is_hold_tapdance_disabled = true;
            layer_on(_RAISE);
            update_tri_layer(_LOWER, _RAISE, _ADJUST);
            break;

        case DOUBLE_SINGLE_TAP:
        case DOUBLE_HOLD:
            register_code(KC_RALT);
            layer_on(_ACCENTS_RALT);
            break;
    }
}

void td_raise_reset (tap_dance_state_t *state, void *user_data) {
    switch (raise_tap_state.state) {
        case SINGLE_TAP:
            unregister_code(KC_BSPC);
            break;

        case SINGLE_HOLD:
            bootloader_active = false;
            bootloader_timer = 0;
            layer_off(_RAISE);
            update_tri_layer(_LOWER, _RAISE, _ADJUST);
            is_hold_tapdance_disabled = false;
            break;

        case DOUBLE_SINGLE_TAP:
        case DOUBLE_HOLD:
            unregister_code(KC_RALT);
            layer_off(_ACCENTS_RALT);
            break;
    }
    raise_tap_state.state = 0;
}
