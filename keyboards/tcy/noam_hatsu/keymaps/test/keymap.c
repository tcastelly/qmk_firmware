// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_split_3x8_2(
        KC_0 ,   KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,     KC_7,         KC_A,     KC_B,    KC_C,    KC_D,    KC_E, KC_F,   KC_G,    KC_H,
        KC_8,    KC_9,    KC_ENT,  KC_ESC,  KC_BSPC, KC_SPC,  KC_MINS,  KC_EQL,       KC_I,     KC_J,    KC_K,    KC_L,    KC_M, KC_N,   KC_O,    KC_P,
        KC_LBRC, KC_RBRC, KC_BSLS, KC_SCLN, KC_QUOT, KC_GRV,  KC_COMM,  KC_SLSH,      KC_Q,     KC_R,    KC_S,    KC_T,    KC_U, KC_V,   KC_W,    KC_X,
                                            KC_F1,   KC_F2,                                                                KC_Y, KC_Z
    )
};
