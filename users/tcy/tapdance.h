#pragma once

extern bool is_kc_caps;
extern bool touched_td;
extern bool scrolling_mode;
extern bool is_hold_tapdance_disabled;

enum layer_names {
    _QWERTY,        // 0
    _QWERTY_OSX,    // 1
    _QWERTY_GAMING, // 2
    _LOWER,         // 3
    _RAISE,         // 4
    _ADJUST,        // 5
    _ESC,           // 6
    _ESC_OSX,       // 7
    _NUM_PADS,      // 8
    _ACCENTS_RALT   // 9
};

enum custom_keycodes {
    QWERTY = SAFE_RANGE,
    QWERTY_OSX,
    QWERTY_GAMING,
    LOWER,
    RAISE,
    ADJUST,
    ESC,
    ACCENT_GRAVE,
    ACCENT_CIRCUM,
    ACCENT_TREMA,
    ACCENT_E_GRAVE,
    ACCENT_A_GRAVE,

    TOGGLE_BUZZ,

    TOGGLE_RGB,

    TOGGLE_OLED,
    TOGGLE_OLED_WPM,
    TOGGLE_OLED_LAYOUT,
    TOGGLE_OLED_DEFAULT,

    // to be used with RALT already pressed
    ACCENT_I_CIRC_RALT,
    ACCENT_O_CIRC_RALT,
    ACCENT_U_AIGU_RALT,
    ACCENT_C_RALT,
    ACCENT_A_GRAVE_RALT,

    // Jetbrains macros
    JET_FIND,
    JET_RNM,
    JET_OPTI,
    JET_FORMAT,
    JET_FORMAT_OSX,
};

// default tap dance states
enum {
    SINGLE_TAP        = 1,
    SINGLE_HOLD       = 2,
    DOUBLE_TAP        = 3,
    DOUBLE_HOLD       = 4,
    DOUBLE_SINGLE_TAP = 5,
    TRIPLE_TAP        = 6,
    TRIPLE_HOLD       = 7,
    TRIPLE_SINGLE_TAP = 8
};

// tap dance indices
enum {
    TD_ESC,
    TD_ESC_OSX,
    TD_A,
    TD_A_OSX,
    TD_TAB,
    TD_O,
    TD_P,
    TD_L,
    TD_ENT,
    TD_SCLN,
    TD_LCTL,
    TD_LGUI,
    TD_LALT,
    TD_RALT,
    TD_RALT_OSX,
    TD_BSPC,
    TD_BSPC_OSX,
    TD_DEL,
    TD_DEL_OSX,
    TD_LEFT,
    TD_LEFT_OSX,
    TD_RIGHT,
    TD_RIGHT_OSX
};

typedef struct {
    bool is_press_action;
    int  state;
} tap;

typedef struct {
    uint16_t tap;
    uint16_t hold;
    uint16_t held;
} tap_dance_tap_hold_t;

extern void td_ralt_reset(tap_dance_state_t *state, void *user_data);
extern void td_ralt_finished(tap_dance_state_t *state, void *user_data);
extern void td_ralt_osx_reset(tap_dance_state_t *state, void *user_data);
extern void td_ralt_osx_finished(tap_dance_state_t *state, void *user_data);
extern void td_lgui_reset(tap_dance_state_t *state, void *user_data);
extern void td_lgui_finished(tap_dance_state_t *state, void *user_data);
extern void td_lalt_reset(tap_dance_state_t *state, void *user_data);
extern void td_lalt_finished(tap_dance_state_t *state, void *user_data);
extern void td_lctl_reset(tap_dance_state_t *state, void *user_data);
extern void td_lctl_finished(tap_dance_state_t *state, void *user_data);
extern void tap_dance_tap_hold_reset(tap_dance_state_t *state, void *user_data);
extern void tap_dance_tap_hold_finished(tap_dance_state_t *state, void *user_data);
extern void tap_dance_tap_hold_finished_unprotected(tap_dance_state_t *state, void *user_data);
extern void tap_dance_tap_hold_reset_layout(tap_dance_state_t *state, void *user_data);
extern void tap_dance_tap_hold_finished_layout(tap_dance_state_t *state, void *user_data);

#define ACTION_TAP_DANCE_TAP_HOLD(tap, hold) \
    { .fn = {NULL, tap_dance_tap_hold_finished, tap_dance_tap_hold_reset}, .user_data = (void *)&((tap_dance_tap_hold_t){tap, hold, 0}), }

#define ACTION_TAP_DANCE_TAP_HOLD_UNPROTECTED(tap, hold) \
    { .fn = {NULL, tap_dance_tap_hold_finished_unprotected, tap_dance_tap_hold_reset}, .user_data = (void *)&((tap_dance_tap_hold_t){tap, hold, 0}), }

#define ACTION_TAP_DANCE_TAP_HOLD_LAYOUT(tap, hold) \
    { .fn = {NULL, tap_dance_tap_hold_finished_layout, tap_dance_tap_hold_reset_layout}, .user_data = (void *)&((tap_dance_tap_hold_t){tap, hold, 0}), }
