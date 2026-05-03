#include QMK_KEYBOARD_H

#include "layout_42.h"
#include "tcy.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_QWERTY]        = LAYOUT_qwerty,
    [_QWERTY_OSX]    = LAYOUT_qwerty_osx,
    [_QWERTY_GAMING] = LAYOUT_qwerty_gaming,
    [_LOWER]         = LAYOUT_lower,
    [_RAISE]         = LAYOUT_raise,
    [_ESC]           = LAYOUT_esc,
    [_ESC_OSX]       = LAYOUT_esc_osx,
    [_NUM_PADS]      = LAYOUT_num_pads,
    [_ACCENTS_RALT]  = LAYOUT_accents_ralt,
    [_ADJUST]        = LAYOUT_adjust,
};

