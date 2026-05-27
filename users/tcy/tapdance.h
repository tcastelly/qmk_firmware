#pragma once

extern bool is_kc_caps;
extern bool touched_td;
extern bool scrolling_mode;
extern uint8_t hold_td_disable_count;

enum layer_names {
    _QWERTY,
    _QWERTY_GAMING,
    _LOWER,
    _RAISE,
    _ADJUST,
    _ESC,
    _NUM_PADS,
    _ACCENTS_RALT,
    _OLED_OFF_SIGNAL,
    _OSX_SIGNAL
};

enum custom_keycodes {
    QWERTY = SAFE_RANGE,
    QWERTY_OSX,
    QWERTY_GAMING,
    LOWER,
    RAISE,
    ADJUST,
    ACCENT_GRAVE,
    ACCENT_CIRCUM,
    ACCENT_TREMA,
    ACCENT_E_GRAVE,
    ACCENT_A_GRAVE,

    TOGGLE_BUZZ,

    TOGGLE_RGB,

    TOGGLE_OLED,

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
    TD_A,
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
    TD_BSPC,
    TD_DEL,
    TD_LEFT,
    TD_RIGHT,
    TD_LOWER,
    TD_RAISE
};

extern int cur_dance(tap_dance_state_t *state);
extern int cur_dance_permissive(tap_dance_state_t *state);

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
extern void td_lgui_reset(tap_dance_state_t *state, void *user_data);
extern void td_lgui_finished(tap_dance_state_t *state, void *user_data);
extern void td_lalt_reset(tap_dance_state_t *state, void *user_data);
extern void td_lalt_finished(tap_dance_state_t *state, void *user_data);
extern void td_lctl_reset(tap_dance_state_t *state, void *user_data);
extern void td_lctl_finished(tap_dance_state_t *state, void *user_data);
extern void td_lower_reset(tap_dance_state_t *state, void *user_data);
extern void td_lower_finished(tap_dance_state_t *state, void *user_data);
extern void td_raise_reset(tap_dance_state_t *state, void *user_data);
extern void td_raise_finished(tap_dance_state_t *state, void *user_data);
extern void tap_dance_tap_hold_reset(tap_dance_state_t *state, void *user_data);
extern void tap_dance_tap_hold_finished(tap_dance_state_t *state, void *user_data);
extern void tap_dance_tap_hold_reset_layout(tap_dance_state_t *state, void *user_data);
extern void tap_dance_tap_hold_finished_layout(tap_dance_state_t *state, void *user_data);
extern void td_word_bspc_finished(tap_dance_state_t *state, void *user_data);
extern void td_word_bspc_reset(tap_dance_state_t *state, void *user_data);
extern void td_word_del_finished(tap_dance_state_t *state, void *user_data);
extern void td_word_del_reset(tap_dance_state_t *state, void *user_data);
extern void td_word_left_finished(tap_dance_state_t *state, void *user_data);
extern void td_word_left_reset(tap_dance_state_t *state, void *user_data);
extern void td_word_right_finished(tap_dance_state_t *state, void *user_data);
extern void td_word_right_reset(tap_dance_state_t *state, void *user_data);

#define ACTION_TAP_DANCE_TAP_HOLD(tap, hold) \
    { .fn = {NULL, tap_dance_tap_hold_finished, tap_dance_tap_hold_reset}, .user_data = (void *)&((tap_dance_tap_hold_t){tap, hold, 0}), }


#define ACTION_TAP_DANCE_TAP_HOLD_LAYOUT(tap, hold) \
    { .fn = {NULL, tap_dance_tap_hold_finished_layout, tap_dance_tap_hold_reset_layout}, .user_data = (void *)&((tap_dance_tap_hold_t){tap, hold, 0}), }
