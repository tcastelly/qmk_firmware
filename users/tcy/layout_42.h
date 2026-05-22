#pragma once

#include "quantum.h"
#include "tapdance.h"

#define KC_WH_D MS_WHLD
#define KC_WH_U MS_WHLU
#define KC_MS_BTN1 MS_BTN1
#define KC_MS_BTN2 MS_BTN2
#define KC_MS_UP MS_UP
#define KC_MS_DOWN MS_DOWN
#define KC_MS_LEFT MS_LEFT
#define KC_MS_RIGHT MS_RGHT

#define RGB_HUI UG_HUEU
#define RGB_SAI UG_SATU
#define RGB_VAI UG_VALU
#define RGB_MOD UG_NEXT
#define RGB_HUD RM_HUED
#define RGB_SAD RM_SATD
#define RGB_VAD RM_VALD

// clang-format off
#define LAYOUT_qwerty LAYOUT_split_3x6_3( \
    TD(TD_TAB),     KC_Q,         KC_W,    KC_E,    KC_R,    KC_T,                 KC_Y,       KC_U,           KC_I,    TD(TD_O),       TD(TD_P),    TD(TD_BSPC),     \
    TD(TD_ESC),     TD(TD_A),     KC_S,    KC_D,    KC_F,    KC_G,                 KC_H,       KC_J,           KC_K,    TD(TD_L),       TD(TD_SCLN), KC_QUOT,         \
    KC_LSFT,        KC_Z,         KC_X,    KC_C,    KC_V,    KC_B,                 KC_N,       KC_M,           KC_COMM, KC_DOT,         KC_SLSH,     TD(TD_ENT),      \
                                           KC_LCTL, TD(TD_LALT), TD(TD_LOWER),     KC_SPC,     TD(TD_RAISE),   TD(TD_RALT)                                            \
)

#define LAYOUT_qwerty_gaming LAYOUT_split_3x6_3( \
    KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                             KC_Y,   KC_U,    KC_I,    KC_O,   KC_P,    KC_BSPC, \
    KC_ESC,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                             KC_H,   KC_J,    KC_K,    KC_L,   KC_SCLN, KC_QUOT, \
    KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,                             KC_N,   KC_M,    KC_COMM, KC_DOT, KC_SLSH, KC_ENT,  \
                               KC_LCTL, KC_SPC,  RAISE,                            KC_SPC, LOWER,   KC_RALT                            \
)

#define LAYOUT_lower LAYOUT_split_3x6_3( \
    KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,                         KC_CIRC,    KC_AMPR,    KC_ASTR, KC_LPRN, KC_RPRN, KC_DEL,  \
    KC_CAPS, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                           KC_F6,      KC_MINS,    KC_PLUS, KC_LCBR, KC_RCBR, KC_PIPE, \
    _______, KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,                          KC_F12,     S(KC_NUHS), KC_HOME, KC_END,  _______, _______, \
                               _______, _______, _______,                          KC_MS_BTN1, _______,    _______                             \
)

#define LAYOUT_raise LAYOUT_split_3x6_3( \
    KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                             KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC, \
    KC_DEL,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                           KC_F6,   KC_UNDS, KC_EQL,  KC_LBRC, KC_RBRC, KC_BSLS, \
    _______, KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,                          KC_F12,  KC_NUHS, KC_PGUP, KC_PGDN, _______, _______, \
                               _______, _______, _______,                          _______, _______, _______                             \
)

#define LAYOUT_esc LAYOUT_split_3x6_3( \
    ACCENT_GRAVE, ACCENT_GRAVE,   _______, ACCENT_E_GRAVE, JET_RNM,  _______,     ACCENT_CIRCUM, KC_WH_D,    KC_WH_U,  JET_OPTI,     JET_FORMAT,  TD(TD_DEL),   \
    _______,      ACCENT_A_GRAVE, _______, _______,        JET_FIND, _______,     TD(TD_LEFT),   KC_DOWN,    KC_UP,    TD(TD_RIGHT), _______,     ACCENT_TREMA, \
    _______,      _______,        _______, _______,        _______, _______,      KC_MS_LEFT,    KC_MS_DOWN, KC_MS_UP, KC_MS_RIGHT,  _______,     _______,      \
                                           _______, KC_LCTL, KC_MS_BTN2,          KC_MS_BTN1,    _______,    _______                                             \
)

#define LAYOUT_num_pads LAYOUT_split_3x6_3( \
    _______, _______, _______, _______, _______, _______,                          _______, _______, KC_7,    KC_8,   KC_9,   KC_BSPC, \
    _______, _______, _______, _______, _______, _______,                          _______, _______, KC_4,    KC_5,   KC_6,   KC_DOT,  \
    _______, _______, _______, _______, _______, _______,                          _______, _______, KC_1,    KC_2,   KC_3,   KC_0,    \
                               _______, _______, _______,                          _______, _______, KC_DOT                            \
)

#define LAYOUT_accents_ralt LAYOUT_split_3x6_3( \
    _______, _______,             _______, _______, _______, _______,              _______, ACCENT_U_AIGU_RALT, ACCENT_I_CIRC_RALT, ACCENT_O_CIRC_RALT, _______, _______, \
    _______, ACCENT_A_GRAVE_RALT, _______, _______, _______, _______,              _______, _______,            _______,            _______,            _______, _______, \
    _______, _______,             _______, ACCENT_C_RALT, _______, _______,        _______, _______,            _______,            _______,            _______, _______, \
                                           _______, _______, _______,              _______, _______,            _______                                                    \
)

#define LAYOUT_adjust LAYOUT_split_3x6_3( \
    TOGGLE_RGB,     QWERTY,  QWERTY_OSX, QWERTY_GAMING, _______, _______,            _______, _______, _______, _______, _______, QK_BOOT,     \
    TOGGLE_OLED, RGB_HUI, RGB_SAI,    RGB_VAI,        _______, _______,            _______, _______, _______, _______, _______, TOGGLE_BUZZ, \
    RGB_MOD,     RGB_HUD, RGB_SAD,    RGB_VAD,        _______, _______,            _______, _______, _______, _______, _______, _______,     \
                                      _______, _______, _______,                   _______, _______, _______                                 \
)

// clang-format on
