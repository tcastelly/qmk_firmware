/* Copyright 2023 Tye (@tyetye)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Current features of this keymap for the Santoku keyboard:
Trackpoint:
    - Speed, Acceleration, and Angle are adjustable on-the-fly from the Settings Screen.
    - Drag Scroll (aka Middle Click Scroll) speed is adjustable on-the-fly from the Settings Screen

*/

#include QMK_KEYBOARD_H
#include <stdbool.h>   // This is just to get rid of the linter warning
#include <stdint.h>   // This is just to get rid of the linter warning

#include "santoku.h"

// for EEPROM to save settings between resets
#include "quantum.h"
#include "eeprom.h"

#ifdef PS2_MOUSE_ENABLE
#include "ps2_mouse.h"
#include "ps2.h"
#endif

#include "tapdance.c"
#include "lib/lib8tion/lib8tion.h"

#define VANITY_TIMEOUT 5000
#define ___x___ XXXXXXX

#define USER_CONFIG_SIGNATURE 0xDA7A // Used to determine if EEPROM has previously saved Settings already
#define EEPROM_CUSTOM_START 32

#define NUM_DISPLAY_ROWS 4

// Scroll declarations
// Variables to store accumulated scroll values
static bool  is_scrolling = false;
float scroll_accumulated_h = 0;
float scroll_accumulated_v = 0;

static const char PROGMEM qmk_logo[] = {
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x94,
    0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4,
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0x00
};

typedef enum {
    ALTTAB_INACTIVE,
    ALTTAB_PRESSED,
    ALTTAB_WAITING,
} alttab_t;

typedef enum {
    SETTING_TP_ACCELERATION,
    SETTING_TP_SPEED,
    SETTING_TP_SCROLL_SPEED,
    SETTING_ROTATION_ANGLE,
    SETTING_PINKY_SHIFT,
    SETTING_ALT_TAB_TIMEOUT,
    SETTING_DEFAULT_LAYOUT,
    SETTING_OPERATING_SYSTEM,
    SETTING_EXP_SEND_MOUSE_UPDATE,
    SETTING_SCROLLWHEEL_DELAY,
    NUM_SETTINGS  // make sure this is always the final element in the enum
} setting_id_t;

typedef struct {
    setting_id_t id;
    const char* name;
    void (*display_func)(setting_id_t id, bool is_current);
} setting_t;

// The struct that will be saved to EEPROM to preserve user settings
typedef struct {
    uint16_t signature;  // Used to determine if data is currently saved in EEPROM
    uint16_t mouse_rotation_angle;
    uint16_t alt_tab_timeout;
    uint8_t  acceleration_setting;
    uint8_t  linear_reduction_setting;
    uint8_t  drag_scroll_speed_setting;
    uint8_t  scroll_wheel_test_setting;
    bool     is_pinky_shift_keys_active;
} user_config_t;

enum scroll_wheel_setting{
    DEFAULT,
    DEFAULT_FASTER,
    FANCY,
    FANCY2
};

// Combo stuff
enum combos {
    COMBO_MCOMMADOT_FORWARDHISTORY,
    COMBO_NMCOMM_BACKHISTORY,
    COMBO_HJK_CLOSETAB,
    COMBO_YUI_PREVTAB,
    COMBO_UIO_NEXTTAB,
    COMBO_GH_CAPSLOCK,
    COMBO_UI_ESCAPE,
    COMBO_FG_TAB,
    NUM_COMBOS    // make sure this is always the final element in the combos enum
};

// Forward declarations
void scroll_settings(int8_t direction);
void rotate_mouse_coordinates(uint16_t angle, report_mouse_t *mouse_report);
void cordic_rotate( uint16_t angle, report_mouse_t *mouse_report);
void update_settings_display(void);
void adjust_setting_uint16(uint16_t *setting, int8_t adjustment, uint16_t min, uint16_t max);
void adjust_setting_uint8(uint8_t *setting, int8_t adjustment, uint8_t min, uint8_t max);
void save_settings_to_eeprom(void);

void display_placeholder(setting_id_t id, bool is_current);
void display_int(setting_id_t id, bool is_current);
void display_progress_x6(setting_id_t id, bool is_current);
void display_bool(setting_id_t id, bool is_current);
void scale_mouse_vector(report_mouse_t *mouse_report);
setting_t settings[NUM_SETTINGS] = {
    {SETTING_TP_ACCELERATION,       "TP Accel    ",      display_progress_x6},
    {SETTING_TP_SPEED,              "TP Speed    ",      display_progress_x6},
    {SETTING_TP_SCROLL_SPEED,       "TP Scroll   ",      display_progress_x6},
    {SETTING_ROTATION_ANGLE,        "TP Rotate      ",   display_int},
    {SETTING_PINKY_SHIFT,           "Pinky Shift      ", display_bool},
    {SETTING_ALT_TAB_TIMEOUT,       "AltTab Time    ",   display_int},
    {SETTING_DEFAULT_LAYOUT,        "Def Layout  ",      display_placeholder},
    {SETTING_OPERATING_SYSTEM,      "Oper System ",      display_placeholder},
    {SETTING_EXP_SEND_MOUSE_UPDATE, "EX MouseUpd ",      display_placeholder},
    {SETTING_SCROLLWHEEL_DELAY,     "ScrlWhlDel     ",   display_int}
};

// One tap alt-tab controls. Improvement to the code example from: https://docs.qmk.fm/#/feature_macros?id=super-alt%e2%86%aftab
alttab_t is_alt_tab_pressed = ALTTAB_INACTIVE;
uint16_t alt_tab_timer      = 0;
uint16_t alt_tab_timeout    = 300;

// toggles the typical shift keys (in lower corners). Useful when learning to use homerow mod's shift keys but still need to be productive at day job.
bool is_pinky_shift_keys_active = false;

// Trackpoint/mouse pointer dynamic speed controls and GUI/OLED settings
uint8_t acceleration_setting        = 2;
float   acceleration_values[6]      = {0.6f, 0.8f, 1.0f, 1.2f, 1.4f, 1.6f};
uint8_t linear_reduction_setting    = 2;
float   linear_reduction_values[6]  = {80.0f, 2.2f, 2.0f, 1.8f, 1.6f, 1.4f};
uint8_t drag_scroll_speed_setting   = 2;
uint8_t drag_scroll_speed_values[6] = {8, 7, 6, 5, 4, 3};
char *  progress_bars_x6[6]         = {"[=     ]", "[==    ]", "[===   ]", "[====  ]", "[===== ]", "[=PLAID]"};
uint8_t scroll_wheel_test_setting   = 0;
uint16_t mouse_rotation_angle       = 350;

uint8_t current_setting_choice      = 0;
int8_t settings_scroll_position     = 0;
int8_t settings_selected_setting    = 0;

user_config_t user_config;

// Santoku keymap layout
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_QWERTY] =
    {/*QWERTY*/
        {TD(TD_TAB),  KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,                         KC_Y,    KC_U,    KC_I,TD(TD_O),TD(TD_P), TD(TD_BSPC)},
        {TD(TD_ESC),  KC_A,   KC_S,    KC_D,    KC_F,    KC_G,                         KC_H,    KC_J,    KC_K,TD(TD_L),TD(TD_SCLN), KC_QUOT},
        {KC_LSFT,     KC_Z,   KC_X,    KC_C,    KC_V,    KC_B,                         KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH, TD(TD_ENT)},
        {___x___,  ___x___,      ___x___,      KC_LCTL,TD(TD_LALT),LOWER,    KC_SPC,   RAISE, TD(TD_RALT),       ___x___,      ___x___,         ___x___}},

    [_QWERTY_OSX] =
    {/*QWERTY OSX*/
        {TD(TD_TAB),  KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,                         KC_Y,    KC_U,    KC_I,TD(TD_O),TD(TD_P), TD(TD_BSPC_OSX)},
        {TD(TD_ESC_OSX),  KC_A,   KC_S,    KC_D,  KC_F,     KC_G,                          KC_H,    KC_J,    KC_K,TD(TD_L),TD(TD_SCLN), KC_QUOT},
        {KC_LSFT,     KC_Z,   KC_X,    KC_C,    KC_V,    KC_B,                         KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH, TD(TD_ENT)},
        {___x___,  ___x___,      ___x___,      TD(TD_LCTL),TD(TD_LGUI),LOWER,    KC_SPC,   RAISE, TD(TD_RALT_OSX),       ___x___,      ___x___,         ___x___}},

    [_QWERTY_GAMING] =
    {/*QWERTY Gaming*/
        {KC_TAB,  KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,                         KC_Y,    KC_U,    KC_I,KC_O,KC_P, KC_BSPC},
        {KC_ESC,  KC_A,   KC_S,    KC_D,    KC_F,    KC_G,                         KC_H,    KC_J,    KC_K,KC_L,KC_SCLN, KC_QUOT},
        {KC_LSFT,     KC_Z,   KC_X,    KC_C,    KC_V,    KC_B,                         KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH, KC_ENT},
        {___x___,  ___x___,      ___x___,      KC_LCTL,KC_SPC,RAISE,          KC_SPC,   LOWER, KC_RALT,       ___x___,      ___x___,         ___x___}},
    [_SETTINGS] =
    {/*SETTINGS*/
        {___x___, ___x___, ___x___, SETTINGS_UP, ___x___, ___x___, ___x___, ___x___, ___x___, ___x___, ___x___, ___x___},
        {SETTINGS_EXIT, ___x___, SETTINGS_LEFT, SETTINGS_DOWN, SETTINGS_RIGHT, ___x___, SETTINGS_LEFT, SETTINGS_DOWN, SETTINGS_UP, SETTINGS_RIGHT, ___x___, ___x___},
        {___x___, ___x___, ___x___, ___x___, ___x___, ___x___, ___x___, ___x___, ___x___, ___x___, QWERTY, ___x___},
        {___x___, ___x___, ___x___, ___x___, ___x___, ___x___, ___x___, ___x___, SETTINGS_EXIT, ___x___, ___x___, ___x___}
    },
    [_LOWER] =
    {/*QWERTY Lower*/
        {KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,                       KC_CIRC, KC_AMPR,    KC_ASTR,    KC_LPRN, KC_RPRN, KC_DEL},
        {KC_CAPS, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                         KC_F6,   KC_MINS,    KC_PLUS,    KC_LCBR, KC_RCBR, KC_PIPE},
        {_______, KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,                        KC_F12,  S(KC_NUHS), KC_HOME, KC_END, _______, TOGGLE_OLED},
        {___x___,  ___x___,      ___x___,      _______, _______, _______,    KC_MS_BTN1, _______, _______,       ___x___,      ___x___,        ___x___}},

    [_RAISE] =
    {/*QWERTY Raise*/
        {KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                          KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC},
        {KC_DEL,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                         KC_F6,   KC_UNDS, KC_EQL,  KC_LBRC, KC_RBRC, KC_BSLS},
        {_______, KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,                        KC_F12,  KC_NUHS, KC_PGUP, KC_PGDN, _______,  _______},
        {___x___,  ___x___,      ___x___,      _______, _______, _______,    _______, _______, _______,       ___x___,      ___x___,         ___x___}},

    [_ESC_OSX] =
    {/*ESC osx*/
        {ACCENT_GRAVE, ACCENT_GRAVE, _______, ACCENT_E_GRAVE, JET_RNM, _______,    ACCENT_CIRCUM, KC_WH_D, KC_WH_U, JET_OPTI, JET_FORMAT_OSX, TD(TD_DEL_OSX)},
        {_______, ACCENT_A_GRAVE, _______, _______, JET_FIND, _______,              TD(TD_LEFT_OSX), KC_DOWN, KC_UP,  TD(TD_RIGHT_OSX), _______, ACCENT_TREMA},
        {_______, _______, _______, _______, _______, _______,                     _______,KC_KB_MUTE,_______, _______, _______,  _______},
        {___x___,  ___x___,      ___x___,      _______, _______,KC_MS_BTN2, KC_MS_BTN1, _______,       ___x___,      ___x___,         ___x___}},

    [_ESC] =
    {/*ESC*/
        {ACCENT_GRAVE, ACCENT_GRAVE, _______, ACCENT_E_GRAVE, JET_RNM, _______,   ACCENT_CIRCUM, KC_WH_D, KC_WH_U, JET_OPTI, JET_FORMAT, TD(TD_DEL)},
        {_______, ACCENT_A_GRAVE, _______, _______, JET_FIND, _______,              TD(TD_LEFT), KC_DOWN, KC_UP,  TD(TD_RIGHT), _______, ACCENT_TREMA},
        {_______, _______, _______, _______, _______, _______,                     _______, KC_KB_MUTE, _______, _______, _______,  _______},
        {___x___,  ___x___,      ___x___,      _______, _______,KC_MS_BTN2, KC_MS_BTN1, _______, _______,       ___x___,      ___x___,         ___x___}},

    [_ACCENTS_RALT] =
    {/*ACCENTS Ralt*/
        {_______, _______, _______ , _______, _______, _______,                    _______, ACCENT_U_AIGU_RALT, ACCENT_I_CIRC_RALT, ACCENT_O_CIRC_RALT, _______, _______},
        {_______, ACCENT_A_GRAVE_RALT, _______, _______, _______, _______,         _______,  _______, _______,  _______, _______, _______},
        {_______, _______, _______, ACCENT_C_RALT, _______, _______,               _______, _______, _______, _______, _______, _______},
        {___x___,  ___x___,      ___x___,      _______, _______, _______,    _______, _______, _______,       ___x___,      ___x___,         ___x___}},
    [_ADJUST] =
    {/*Adjust*/
        {_______, QWERTY , QWERTY_OSX  , QWERTY_GAMING, _______, SETTINGS,                 _______, _______, _______, _______, _______, QK_BOOT},
        {_______, _______, _______, _______, _______, _______,              _______, _______, _______,  _______, _______, _______},
        {_______, _______, _______, _______, _______, _______,                     _______,_______,_______, _______, _______,  _______},
        {___x___,  ___x___,      ___x___,      _______, _______,_______, _______, _______, _______,       ___x___,      ___x___,         ___x___}},

    };

#ifdef OLED_ENABLE
bool is_kc_caps  = false;

enum oled_modes {
  OLED_DEFAULT,
  OLED_OFF,
};

int8_t oled_mode = OLED_DEFAULT;

bool oled_task_user(void) {
    // led_t led_state = { .raw = host_keyboard_leds() };
    if (oled_mode == OLED_OFF) {
        oled_clear();
        oled_off();
        return false;
    }

    switch (get_highest_layer(layer_state)) {
        case _ADJUST:
            oled_write_ln_P(PSTR("       ADJUST       "), true);
            break;

        case _LOWER:
            oled_write_ln_P(PSTR("       LOWER        "), true);
            break;

        case _RAISE:
            oled_write_ln_P(PSTR("       RAISE        "), true);
            break;

        case _QWERTY:
            oled_write_ln_P(PSTR("       QWERTY       "), true);
            break;

        case _QWERTY_OSX:
            oled_write_ln_P(PSTR("     QWERTY OSX     "), true);
            break;

        case _QWERTY_GAMING:
            oled_write_ln_P(PSTR("    QWERTY Gaming   "), true);
            break;

        case _ESC:
            oled_write_ln_P(PSTR("        ESC         "), true);
            break;

        case _ESC_OSX:
            oled_write_ln_P(PSTR("      ESC OSX       "), true);
            break;

        case _ACCENTS_RALT:
            oled_write_ln_P(PSTR("    RALT Accents    "), true);
            break;

        case _SETTINGS:
            oled_write_ln_P(PSTR("      Options       "), true);
            oled_write_ln_P(PSTR(""), false);
            update_settings_display();
            oled_write_ln_P(PSTR(""), false);
            oled_write_ln_P(PSTR("HJKL Select,ESC Save"), false);
            break;
    }

    if (get_highest_layer(layer_state) != _SETTINGS) {
        oled_write_ln_P(PSTR(""), true);
        oled_write_ln_P(PSTR(""), true);

        oled_write_P(qmk_logo, false);

        oled_write_ln_P(PSTR("                    "), false);
        if (is_kc_caps) {
          oled_write_ln_P(PSTR("                  []"), false);
        } else {
          oled_write_ln_P(PSTR("                    "), false);
        }
    }

    return false;
}
#endif

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    tap_dance_action_t *action;
    switch (keycode) {
        case SHFT_KEY:
            if (is_pinky_shift_keys_active) {
                if (record->event.pressed) {
                    register_code(KC_LSFT);
                } else {
                    unregister_code(KC_LSFT);
                }
            }
            return true; // Let QMK send the press/release events

        case OVERVIEW:
            // Macro to handle overview mode. Enter overview, wait, then skip to window after current window
            if (record->event.pressed) {
                register_code(KC_LGUI);
                tap_code(KC_F5);
                unregister_code(KC_LGUI);
                tap_code(KC_RIGHT);
                tap_code(KC_RIGHT);
            }
            return true;

        case TAPALTTB: // Improved on but inspired by: https://github.com/qmk/qmk_firmware/blob/master/keyboards/dz60/keymaps/_bonfire/not-in-use/super-alt-tab.c
            if (record->event.pressed) {
                is_alt_tab_pressed = ALTTAB_PRESSED;
                register_code(KC_LALT);
                tap_code(KC_TAB);
            } else {
                is_alt_tab_pressed = ALTTAB_WAITING;
                alt_tab_timer      = timer_read();
            }
            return true;

        case SETTINGS_UP:
        case SETTINGS_DOWN:
            if (record->event.pressed) {
                scroll_settings(keycode == SETTINGS_UP ? -1 : 1);
            }
            return true;

        case SETTINGS_LEFT:
        case SETTINGS_RIGHT:
            if (record->event.pressed) {
                int8_t adjustment = (keycode == SETTINGS_LEFT) ? -1 : 1;
                if (settings_selected_setting == SETTING_ROTATION_ANGLE) {
                    adjust_setting_uint16(&mouse_rotation_angle, adjustment, 0, 359);
                }
                else if (settings_selected_setting == SETTING_ALT_TAB_TIMEOUT) {
                    adjust_setting_uint16(&alt_tab_timeout, adjustment * 25, 100, 1500);
                }
                else if (settings_selected_setting == SETTING_TP_ACCELERATION) {
                    adjust_setting_uint8(&acceleration_setting, adjustment, 0, 5);
                }
                else if (settings_selected_setting == SETTING_TP_SPEED) {
                    adjust_setting_uint8(&linear_reduction_setting, adjustment, 0, 5);
                }
                else if (settings_selected_setting == SETTING_TP_SCROLL_SPEED) {
                    adjust_setting_uint8(&drag_scroll_speed_setting, adjustment, 0, 5);
                }
                else if (settings_selected_setting == SETTING_PINKY_SHIFT) {
                    is_pinky_shift_keys_active = !is_pinky_shift_keys_active;
                }
            }
            return true;

        case SETTINGS_EXIT:
            layer_clear();
            save_settings_to_eeprom();
            return false;  // needs to be false so qmk doesn't send keystroke. We just want to manually exit the Settings screen.
                           //
        case SETTINGS:
            if (record->event.pressed) {
                layer_move(_SETTINGS);
            }
            return false;
            break;

        case QWERTY:
            if (record->event.pressed) {
                layer_move(_QWERTY);
            }
            return false;
            break;

        case QWERTY_OSX:
            if (record->event.pressed) {
                layer_move(_QWERTY_OSX);
            }
            return false;
            break;

        case QWERTY_GAMING:
            if (record->event.pressed) {
                layer_move(_QWERTY_GAMING);
            }
            return false;
            break;

        case LOWER:
            if (record->event.pressed) {
                is_hold_tapdance_disabled = true;
                layer_on(_LOWER);
                update_tri_layer(_LOWER, _RAISE, _ADJUST);

                // enable scroll
                is_scrolling = true;
            } else {
                layer_off(_LOWER);
                update_tri_layer(_LOWER, _RAISE, _ADJUST);
                is_hold_tapdance_disabled = false;

                // disable scroll
                if (is_scrolling) {  // check if we were scrolling before and set disable if so
                    is_scrolling = false;
                }
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

#ifdef OLED_ENABLE
        case KC_CAPS:
            if (record->event.pressed) {
                is_kc_caps = !is_kc_caps;
            }
            return true;
            break;
#endif

        case KC_LALT:
        case KC_LGUI:
        case KC_LSFT:
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

        case JET_RNM:
            if (record->event.pressed) {
                register_code(KC_LSFT);
                register_code(KC_F6);

                unregister_code(KC_LSFT);
                unregister_code(KC_F6);
            }
            return false;
            break;

        case JET_OPTI:
            if (record->event.pressed) {
                register_code(KC_LCTL);
                register_code(KC_LALT);

                tap_code(KC_O);
                unregister_code(KC_LALT);
                unregister_code(KC_LCTL);
            }
            return false;
            break;

        case  JET_FORMAT:
            if (record->event.pressed) {
                register_code(KC_LCTL);
                register_code(KC_LALT);

                tap_code(KC_L);
                unregister_code(KC_LALT);
                unregister_code(KC_LCTL);
            }
            return false;
            break;

        case  JET_FORMAT_OSX:
            if (record->event.pressed) {
                register_code(KC_LALT);
                register_code(KC_LGUI);

                tap_code(KC_L);
                unregister_code(KC_LALT);
                unregister_code(KC_LGUI);
            }
            return false;
            break;

        case JET_FIND:
            if (record->event.pressed) {
                register_code(KC_LALT);
                register_code(KC_F1);

                unregister_code(KC_F1);
                unregister_code(KC_LALT);
                tap_code(KC_1);
            }
            return false;
            break;

        case TOGGLE_OLED:
            if (record->event.pressed) {
                if (oled_mode != OLED_OFF) {
                    oled_mode = OLED_OFF;
                } else {
                    oled_mode = OLED_DEFAULT;
                }
            }
            return false;
            break;

        case TD(TD_O):  // list all tap dance keycodes with tap-hold configurations
        case TD(TD_SPC):
        case TD(TD_ESC):
        case TD(TD_ESC_OSX):
        case TD(TD_TAB):
        case TD(TD_P):
        case TD(TD_L):
        case TD(TD_ENT):
        case TD(TD_SCLN):
        case TD(TD_BSPC):
        case TD(TD_BSPC_OSX):
        case TD(TD_DEL):
        case TD(TD_DEL_OSX):
        case TD(TD_LEFT):
        case TD(TD_LEFT_OSX):
        case TD(TD_RIGHT):
        case TD(TD_RIGHT_OSX):
            if ((keycode == TD(TD_ESC) || keycode == TD(TD_ESC_OSX)) && !record->event.pressed) {
                adjust_setting_uint8(&acceleration_setting, -2, 0, 5);
                adjust_setting_uint8(&linear_reduction_setting, -2, 0, 5);

                layer_off(_ESC);
                layer_off(_ESC_OSX);
                is_hold_tapdance_disabled = false;
            } else if((keycode == TD(TD_ESC) || keycode == TD(TD_ESC_OSX)) && record->event.pressed) {
                adjust_setting_uint8(&acceleration_setting, 2, 0, 5);
                adjust_setting_uint8(&linear_reduction_setting, 2, 0, 5);
            } else if (keycode == TD(TD_SPC) && !record->event.pressed) {
                layer_off(_RAISE);
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

void scale_mouse_vector_optimized(report_mouse_t *mouse_report);
void rotate_mouse_coordinates_optimized(uint16_t angle, report_mouse_t *mouse_report);

// TODO: Move the speed and acceleration code into a separate function to make more modular
// TODO: Move the drag scroll counter code into a separate function to make more modular
// TODO: Move the mouse acceleration and speed code into a separate function, like rotate_mouse_coordinates()
// TODO: If possible, don't convert values back to int between functions. Keep as floats, round one time at the end.
void ps2_mouse_moved_user(report_mouse_t *mouse_report) {
    scale_mouse_vector(mouse_report);
    rotate_mouse_coordinates(mouse_rotation_angle, mouse_report);

    if (is_scrolling) {
        // Calculate and accumulate scroll values based on mouse movement and divisors
        scroll_accumulated_h += (float)mouse_report->x / PS2_MOUSE_SCROLL_DIVISOR_H;
        scroll_accumulated_v += (float)mouse_report->y / PS2_MOUSE_SCROLL_DIVISOR_V;

        // Assign integer parts of accumulated scroll values to the mouse report
        mouse_report->h = (int8_t)scroll_accumulated_h;
        mouse_report->v = (int8_t)scroll_accumulated_v;

        // Update accumulated scroll values by subtracting the integer parts
        scroll_accumulated_h -= (int8_t)scroll_accumulated_h;
        scroll_accumulated_v -= (int8_t)scroll_accumulated_v;

        // Clear the X and Y values of the mouse report
        mouse_report->x = 0;
        mouse_report->y = 0;

        // Invert scrolling direction
        mouse_report->h = -mouse_report->h;
        mouse_report->v = -mouse_report->v;

        // Send mouse report
        pointing_device_set_report(*mouse_report);
        pointing_device_send();
    } else{
        // Drag scrolling with the Trackpoint is reported so often that it makes the feature unusable without slowing it down.
        // The below code only reports when the counter is evenly divisible by the chosen integer speed.
        // Skipping reports is technically, probably, not ideal. I'd like to find a way to send a slower speed without skipping.
        // As is, however, it works well and is user configurable from the Options screen.
        // TODO: Break out into dedicated function
        static uint16_t drag_scroll_counter = 0;
        drag_scroll_counter == 40320 ? drag_scroll_counter = 0 : drag_scroll_counter++ ; // Because 8!==40320 (allows clean mod divisibility and avoids scrolling surge when resetting to 0)
        if ((mouse_report->v != 0 || mouse_report->h != 0) && drag_scroll_counter % drag_scroll_speed_values[drag_scroll_speed_setting] != 0) {
            mouse_report->v = 0;
            mouse_report->h = 0;
        }
    }
}

inline float fast_approximate_square_root(float input_number);
// Fast approximation for square root
inline float fast_approximate_square_root(float input_number) {
    long bit_representation;
    float half_input, approximate_result;
    const float constant_for_approximation = 1.5F;

    half_input = input_number * 0.5F;
    approximate_result = input_number;
    bit_representation = *(long*)&approximate_result;
    bit_representation = 0x5f3759df - (bit_representation >> 1);
    approximate_result = *(float*)&bit_representation;
    approximate_result = approximate_result * (constant_for_approximation - (half_input * approximate_result * approximate_result));

    return 1 / approximate_result;
}

inline float fast_approximate_power(float base_value, float exponent_value);
// Fast approximation for power function
inline float fast_approximate_power(float base_value, float exponent_value) {
    union {
        float float_value;
        int int_value;
    } union_representation = { base_value };

    union_representation.int_value = (int)(exponent_value * (union_representation.int_value - 1064866805) + 1064866805);

    return union_representation.float_value;
}

void scale_mouse_vector_optimized(report_mouse_t *mouse_report) {
    // Convert integers to float for calculations
    float x = (float)mouse_report->x;
    float y = (float)mouse_report->y;

    // Using fast square root and power approximations
    float hypotenuse = fast_approximate_square_root(x * x + y * y);
    float scaled_hypotenuse = fast_approximate_power(hypotenuse, acceleration_values[acceleration_setting]) / linear_reduction_values[linear_reduction_setting];

    // Eliminating redundant trigonometric calculations
    float cos_angle = x / hypotenuse;
    float sin_angle = y / hypotenuse;

    // Convert float back to integer with rounding
    mouse_report->x += (int)(scaled_hypotenuse * cos_angle + 0.5);
    mouse_report->y += (int)(scaled_hypotenuse * sin_angle + 0.5);
}


void rotate_mouse_coordinates_optimized(uint16_t angle, report_mouse_t *mouse_report) {
    if (angle == 0) {
        return;
    }

    // Pre-calculate conversion factor; this assumes angle rarely changes
    static float last_angle = 0;
    static float precomputed_cosine = 1.0;
    static float precomputed_sine = 0.0;
    if (last_angle != angle) {
        static const float degree_to_radian_conversion = 0.017453f;
        float radians = angle * degree_to_radian_conversion;
        precomputed_cosine = cos(radians);
        precomputed_sine = sin(radians);
        last_angle = angle;
    }

    // Cache current x and y coordinates to reduce memory access
    int cached_mouse_x = mouse_report->x;
    int cached_mouse_y = mouse_report->y;

    // Use precomputed trigonometric values
    mouse_report->x = round(precomputed_cosine * cached_mouse_x - precomputed_sine * cached_mouse_y);
    mouse_report->y = round(precomputed_sine * cached_mouse_x + precomputed_cosine * cached_mouse_y);
}

void scale_mouse_vector(report_mouse_t *mouse_report) {
    // The math below turns the Trackpoint x and y reports (movements) into a vector and scales the vector with some trigonometry.
    // This allows the user to dynamically adjust the mouse cursor sensitivity to their liking.
    // It also results in arguably smoother movement than just multiplying the x and y values by some fixed value.
    // (and yeah, there's some unnecessary/redundant math going here. I'm hoping to lay the foundation for things like software adjustable negative inertia.)
        float hypotenuse        = sqrt((mouse_report->x * mouse_report->x) + (mouse_report->y * mouse_report->y));
        float scaled_hypotenuse = pow(hypotenuse, acceleration_values[acceleration_setting]) / linear_reduction_values[linear_reduction_setting];
        float angle             = atan2(mouse_report->y, mouse_report->x);
        mouse_report->x += (scaled_hypotenuse * cos(angle));
        mouse_report->y += (scaled_hypotenuse * sin(angle));

}

void rotate_mouse_coordinates(uint16_t angle, report_mouse_t *mouse_report) {
    if (angle == 0) {
        return;
    }
    // because pi/180 = 0.017453
    static const float degree = 0.017453f;

    float radians = angle * degree;

    // Need to save these values because we rewrite mouse_report->x immediately but reuse the value to find the rotated y value
    int current_x = mouse_report->x;
    int current_y = mouse_report->y;

    // Calculate rotated x & y, convert back to an int
    mouse_report->x = round(cos(radians) * current_x - sin(radians) * current_y);
    mouse_report->y = round(sin(radians) * current_x + cos(radians) * current_y);
}

bool encoder_update_user(uint8_t index, bool clockwise) {
    //report_mouse_t currentReport     = pointing_device_get_report();
    //currentReport.v = 2 * (clockwise ? 1 : -1);
    switch (get_highest_layer(layer_state)) {
        case _ESC:
        case _ESC_OSX:
            clockwise ? tap_code(KC_KB_VOLUME_UP) : tap_code(KC_KB_VOLUME_DOWN);
            break;
        default:
            clockwise ? tap_code(KC_MS_WH_UP) : tap_code(KC_MS_WH_DOWN);
            break;
    }

    //pointing_device_set_report(currentReport);
    //pointing_device_send();
    return false;
}

void ps2_mouse_init_user(void) {
    uint8_t rcv;

#define TRACKPOINT_DEFAULT_CONFIG_PTSON   0
#define TRACKPOINT_DEFAULT_CONFIG_BUTTON2 2
#define TRACKPOINT_DEFAULT_CONFIG_FLIPX   3
#define TRACKPOINT_DEFAULT_CONFIG_FLIPY   4
#define TRACKPOINT_DEFAULT_CONFIG_FLIPZ   5
#define TRACKPOINT_DEFAULT_CONFIG_SWAPXY  6
#define TRACKPOINT_DEFAULT_CONFIG_FTRANS  7

    // Inquire pts status from Default Configuration register
    rcv = ps2_host_send(0xE2);
    rcv = ps2_host_send(0x2C);
    rcv = ps2_host_recv_response();
    if (rcv & (1 << TRACKPOINT_DEFAULT_CONFIG_PTSON)) {
        // If on, disable pts
        rcv = ps2_host_send(0xE2);
        rcv = ps2_host_send(0x47);
        rcv = ps2_host_send(0x2C);
        rcv = ps2_host_send(0x01);
    }
}

// Handles scrolling up and down through the choices on the Settings layer/screen
void scroll_settings(int8_t direction) {
    settings_scroll_position += direction;
    settings_selected_setting += direction;

    int8_t max_scroll_position = NUM_SETTINGS > NUM_DISPLAY_ROWS ? NUM_SETTINGS - NUM_DISPLAY_ROWS : 0;
    int8_t max_selected_setting = NUM_SETTINGS - 1;

    // handle edge cases
    if (settings_scroll_position < 0) {
        settings_scroll_position = 0;
    }
    else if (settings_scroll_position > max_scroll_position) {
        settings_scroll_position = max_scroll_position;
    }

    if (settings_selected_setting < 0) {
        settings_selected_setting = 0;
    }
    else if (settings_selected_setting > max_selected_setting) {
        settings_selected_setting = max_selected_setting;
    }
}

// TODO: Make better way to handle this for uint16_t AND ALSO uint8_t
void adjust_setting_uint16(uint16_t *setting, int8_t adjustment, uint16_t min, uint16_t max) {
    if (adjustment == -1 && *setting == min) {
        *setting = max;
    }
    else if (adjustment == 1 && *setting == max) {
        *setting = min;
    }
    else {
        //*setting += adjustment;
        uint16_t new_value = *setting + adjustment;
        if (new_value > max) {
            *setting = min;
        }
        else if (new_value < min) {
            *setting = max;
        }
        else {
            *setting = new_value;
        }
    }
}

void adjust_setting_uint8(uint8_t *setting, int8_t adjustment, uint8_t min, uint8_t max) {
    if (adjustment < 0 && *setting == min) {
        *setting = max;
    }
    else if (adjustment > 0 && *setting == max) {
        *setting = min;
    }
    else {
        //*setting += adjustment;
        uint16_t new_value = *setting + adjustment;
        if (new_value > max) {
            *setting = min;
        }
        else if (new_value < min) {
            *setting = max;
        }
        else {
            *setting = new_value;
        }
    }
}

// Loops through the visible Settings on the Settings screen to display them
void update_settings_display() {
    //static char * temp_progress_bars[9] = {"1", "2", "3", "4", "5", "6", "7", "8", "9"};
    for (uint8_t i = 0; i < NUM_DISPLAY_ROWS; i++) {
        uint8_t setting_index = settings_scroll_position + i;
        if (setting_index < NUM_SETTINGS) {
            oled_write(settings[setting_index].name, settings_selected_setting == setting_index);
            settings[setting_index].display_func(settings[setting_index].id, settings_selected_setting == setting_index);
        }
    }
}

// Used by update_display_settings to show Progress Bar settings
void display_progress_x6(setting_id_t id, bool is_current) {
    int value;
    switch (id) {
        case SETTING_TP_ACCELERATION:
            value = acceleration_setting;
            break;
        case SETTING_TP_SPEED:
            value = linear_reduction_setting;
            break;
        case SETTING_TP_SCROLL_SPEED:
            value = drag_scroll_speed_setting;
            break;
        default:
            value = 1;
    }

    oled_write_ln(progress_bars_x6[value], is_current);
}

// Used by update_display_settings to show boolean settings
void display_bool(setting_id_t id, bool is_current) {
    bool value;
    switch (id) {
        case SETTING_PINKY_SHIFT:
            value = is_pinky_shift_keys_active;
            break;
        default:
            value = false;
    }
    if (value) {
        oled_write_ln_P(PSTR("Yes"), is_current);
    } else {
        oled_write_ln_P(PSTR(" No"), is_current);
    }
}


// Used by update_display_settings to show integer settings
void display_int(setting_id_t id, bool is_current) {
    int value;
    switch (id) {
        case SETTING_ROTATION_ANGLE:
            value = mouse_rotation_angle;
            break;
        case SETTING_ALT_TAB_TIMEOUT:
            value = alt_tab_timeout;
            break;
        default:
            value = 7;
    }

    oled_write(get_u16_str(value, ' '), is_current);
    oled_write_ln_P(PSTR(""), false);
}

// Used by update_display_settings to provide some text for Settings that aren't finished yet.
void display_placeholder(setting_id_t id, bool is_current) {
    oled_write_ln_P(PSTR("   (WIP)"), is_current);
}



void save_settings_to_eeprom(void) {
        user_config.signature                  = USER_CONFIG_SIGNATURE;
        user_config.mouse_rotation_angle       = mouse_rotation_angle;
        user_config.alt_tab_timeout            = alt_tab_timeout;
        user_config.acceleration_setting       = acceleration_setting;
        user_config.linear_reduction_setting   = linear_reduction_setting;
        user_config.drag_scroll_speed_setting  = drag_scroll_speed_setting;
        user_config.scroll_wheel_test_setting  = scroll_wheel_test_setting;
        user_config.is_pinky_shift_keys_active = is_pinky_shift_keys_active;

        eeprom_update_block(&user_config, (void*)EEPROM_CUSTOM_START, sizeof(user_config_t));
}

void keyboard_post_init_user(void) {
    // Customise these values to desired behaviour
    debug_enable   = false;
    debug_matrix   = false;
    debug_keyboard = false;
    debug_mouse    = false;

    //user_config_t temp_config;
    eeprom_read_block(&user_config, (void*)EEPROM_CUSTOM_START, sizeof(user_config_t));
    if(user_config.signature != USER_CONFIG_SIGNATURE) {
        user_config.signature      = USER_CONFIG_SIGNATURE;
        save_settings_to_eeprom();
    }
    else {
        user_config.signature      = USER_CONFIG_SIGNATURE;
        mouse_rotation_angle       = user_config.mouse_rotation_angle;
        alt_tab_timeout            = user_config.alt_tab_timeout;
        acceleration_setting       = user_config.acceleration_setting;
        linear_reduction_setting   = user_config.linear_reduction_setting;
        drag_scroll_speed_setting  = user_config.drag_scroll_speed_setting;
        scroll_wheel_test_setting  = user_config.scroll_wheel_test_setting;
        is_pinky_shift_keys_active = user_config.is_pinky_shift_keys_active;
    }
}

// Associate our tap dance key with its functionality
tap_dance_action_t tap_dance_actions[] = {
    [TD_ESC] = ACTION_TAP_DANCE_TAP_HOLD_LAYOUT(KC_ESC, _ESC),
    [TD_ESC_OSX] = ACTION_TAP_DANCE_TAP_HOLD_LAYOUT(KC_ESC, _ESC_OSX),
    [TD_SPC] = ACTION_TAP_DANCE_TAP_HOLD_PERMISIVE_LAYOUT(KC_SPC, _RAISE),
    [TD_TAB] = ACTION_TAP_DANCE_TAP_HOLD(KC_TAB, KC_TILD),
    [TD_O] = ACTION_TAP_DANCE_TAP_HOLD(KC_O, KC_LPRN),
    [TD_P] = ACTION_TAP_DANCE_TAP_HOLD(KC_P, KC_RPRN),
    [TD_L] = ACTION_TAP_DANCE_TAP_HOLD(KC_L, KC_LCBR),
    [TD_SCLN] = ACTION_TAP_DANCE_TAP_HOLD(KC_SCLN, KC_RCBR),
    [TD_ENT] = ACTION_TAP_DANCE_TAP_HOLD(KC_ENT, KC_LSFT),

    // same tap-dance
    // enable it for osx and linux
    [TD_LCTL] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_lctl_finished, td_lctl_reset),
    [TD_LALT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_lalt_finished, td_lalt_reset),
    [TD_LGUI] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_lgui_finished, td_lgui_reset),

    [TD_BSPC] = ACTION_TAP_DANCE_TAP_HOLD(KC_BSPC, LCTL(KC_BSPC)),
    [TD_BSPC_OSX] = ACTION_TAP_DANCE_TAP_HOLD(KC_BSPC, LALT(KC_BSPC)),

    [TD_DEL] = ACTION_TAP_DANCE_TAP_HOLD_UNPROTECTED(KC_DEL, LCTL(KC_DEL)),
    [TD_DEL_OSX] = ACTION_TAP_DANCE_TAP_HOLD_UNPROTECTED(KC_DEL, LALT(KC_DEL)),

    [TD_LEFT] = ACTION_TAP_DANCE_TAP_HOLD_UNPROTECTED(KC_LEFT, LCTL(KC_LEFT)),
    [TD_LEFT_OSX] = ACTION_TAP_DANCE_TAP_HOLD_UNPROTECTED(KC_LEFT, LALT(KC_LEFT)),

    [TD_RIGHT] = ACTION_TAP_DANCE_TAP_HOLD_UNPROTECTED(KC_RIGHT, LALT(KC_RIGHT)),
    [TD_RIGHT_OSX] = ACTION_TAP_DANCE_TAP_HOLD_UNPROTECTED(KC_RIGHT, LCTL(KC_RIGHT)),

    [TD_RIGHT_OSX] = ACTION_TAP_DANCE_TAP_HOLD_UNPROTECTED(KC_RIGHT, LALT(KC_RIGHT)),
    [TD_RIGHT] = ACTION_TAP_DANCE_TAP_HOLD_UNPROTECTED(KC_RIGHT, LCTL(KC_RIGHT)),

    [TD_RALT_OSX] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_ralt_osx_finished, td_ralt_osx_reset),
    [TD_RALT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_ralt_finished, td_ralt_reset)
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
