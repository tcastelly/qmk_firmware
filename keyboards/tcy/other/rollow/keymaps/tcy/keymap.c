#include QMK_KEYBOARD_H

#include "layout_42.h"
#include "tcy.h"
#include "ps2_acceleration.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_QWERTY]        = LAYOUT_qwerty,
    [_QWERTY_GAMING] = LAYOUT_qwerty_gaming,
    [_LOWER]         = LAYOUT_lower,
    [_RAISE]         = LAYOUT_raise,
    [_ESC]           = LAYOUT_esc,
    [_NUM_PADS]      = LAYOUT_num_pads,
    [_ACCENTS_RALT]  = LAYOUT_accents_ralt,
    [_ADJUST]        = LAYOUT_adjust,
};

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    return tcy_pointing_device_task(mouse_report);
}

#ifdef ENCODER_ENABLE
bool encoder_update_user(uint8_t index, bool clockwise) {
    switch (get_highest_layer(layer_state)) {
        case _QWERTY:
            // both QWERTY AND QWERTY_OSX scroll with the right encoder
            if (is_osx) {
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
                        tap_code(KC_WH_U);
                    } else {
                        tap_code(KC_WH_D);
                    }
                }
            } else {
                // on default QWERTY (Linux) left encoder will be used to change active "window"
                // simial to ALT/TAB
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
                        tap_code(KC_WH_U);
                    } else {
                        tap_code(KC_WH_D);
                    }
                }
            }
            return false;

        case _LOWER:
        case _RAISE:

        case _ESC:
            if (is_osx) {
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
            } else {
                // on default QWERTY (Linux) and ESC left encoder will be used to resize active "window"
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
            }
            return false;
    }

    return true;
}
#endif
