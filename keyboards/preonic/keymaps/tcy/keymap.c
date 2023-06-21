/* Copyright 2015-2021 Jack Humbert
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

#include QMK_KEYBOARD_H
#include "muse.h"

enum layer_names {
  _QWERTY,
  _QWERTY_LINUX,
  _LOWER,
  _RAISE,
  _ADJUST,
  _ESC,
  _ESC_LINUX,
  _NUM_PADS,
  _ACCENTS_RALT
};

enum custom_keycodes {
  QWERTY = SAFE_RANGE,
  QWERTY_LINUX,
  LOWER,
  RAISE,
  BACKLIT,
  ACCENT_GRAVE,
  ACCENT_CIRCUM,
  ACCENT_TREMA,
  ACCENT_E_GRAVE,
  ACCENT_A_GRAVE,
  NUM_PADS,

  ACCENTS_RALT,

  // to be used with RALT already pressed
  ACCENT_I_CIRC_RALT,
  ACCENT_O_CIRC_RALT,
  ACCENT_U_AIGU_RALT,
  ACCENT_C_RALT,
  ACCENT_A_GRAVE_RALT,

  // Jetbrains macro
  JET_FIND,
  JET_RNM,
};

typedef struct {
  bool is_press_action;
  int state;
} tap;

// default tap dance
enum {
  SINGLE_TAP = 1,
  SINGLE_HOLD = 2, // should use tap_dance_tap_hold_t instead
  DOUBLE_TAP = 3,
  DOUBLE_HOLD = 4,
  DOUBLE_SINGLE_TAP = 5, //send two single taps
  TRIPLE_TAP = 6,
  TRIPLE_HOLD = 7,
  TRIPLE_SINGLE_TAP = 8
};

// "tap-hold"
typedef struct {
    uint16_t tap;
    uint16_t hold;
    uint16_t held;
} tap_dance_tap_hold_t;

// custom tap dance
enum {
    TD_ESC,
    TD_ESC_LINUX,
    TD_TAB,
    TD_O,
    TD_P,
    TD_L,
    TD_ENT,
    TD_SCLN,
    TD_LGUI,
    TD_RALT,
    TD_LALT,
    TD_RALT_LIN,
    TD_BSPC,
    TD_BSPC_LIN,
    TD_DEL,
    TD_DEL_LIN,
    TD_LEFT,
    TD_LEFT_LIN,
    TD_RIGHT,
    TD_RIGHT_LIN,
    TD_LEFT_RALT,
};

bool is_hold_tapdance_disabled = false;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/* Qwerty
 * ,-----------------------------------------------------------------------------------.
 * |   `  |   1  |   2  |   3  |   4  |   5  |   6  |   7  |   8  |   9  |   0  | Del  |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Tab  |   Q  |   W  |   E  |   R  |   T  |   Y  |   U  |   I  |   O  |   P  | Bksp |
 * |------+------+------+------+------+-------------+------+------+------+------+------|
 * | Esc  |   A  |   S  |   D  |   F  |   G  |   H  |   J  |   K  |   L  |   ;  |  "   |
 * |------+------+------+------+------+------|------+------+------+------+------+------|
 * | Shift|   Z  |   X  |   C  |   V  |   B  |   N  |   M  |   ,  |   .  |   /  |Enter |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Ctrl |      | Alt  | GUI | Lower |    Space    |Raise | Left | Down |  Up  |Right |
 * `-----------------------------------------------------------------------------------'
 */
[_QWERTY] = LAYOUT_preonic_grid(
  KC_GRV,        KC_1,     KC_2,         KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,             KC_9,    KC_0,        TD(TD_BSPC),
  TD(TD_TAB),    KC_Q,     KC_W,         KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,             TD(TD_O),TD(TD_P),    TD(TD_BSPC),
  TD(TD_ESC),    KC_A,     KC_S,         KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,             TD(TD_L),TD(TD_SCLN), KC_QUOT,
  KC_LSFT,       KC_Z,     KC_X,         KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM,          KC_DOT,  KC_SLSH,     TD(TD_ENT),
  KC_LCTL,       NUM_PADS, ACCENTS_RALT, KC_LGUI, LOWER,   KC_SPC,  KC_SPC,  RAISE,   TD(TD_LEFT_RALT), KC_DOWN, KC_UP,       KC_RGHT
),

[_QWERTY_LINUX] = LAYOUT_preonic_grid(
  KC_GRV,            KC_1,     KC_2,         KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,             KC_9,    KC_0,        TD(TD_BSPC_LIN),
  TD(TD_TAB),        KC_Q,     KC_W,         KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,             TD(TD_O),TD(TD_P),    TD(TD_BSPC_LIN),
  TD(TD_ESC_LINUX),  KC_A,     KC_S,         KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,             TD(TD_L),TD(TD_SCLN), KC_QUOT,
  KC_LSFT,           KC_Z,     KC_X,         KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM,          KC_DOT,  KC_SLSH,     TD(TD_ENT),
  KC_LCTL,           NUM_PADS, ACCENTS_RALT, KC_LALT, LOWER,   KC_SPC,  KC_SPC,  RAISE,   TD(TD_LEFT_RALT), KC_DOWN, KC_UP,       KC_RGHT
),

/* Lower
 * ,-----------------------------------------------------------------------------------.
 * |   ~  |   !  |   @  |   #  |   $  |   %  |   ^  |   &  |   *  |   (  |   )  | Bksp |
 * |------+------+------+------+------+-------------+------+------+------+------+------|
 * |   ~  |   !  |   @  |   #  |   $  |   %  |   ^  |   &  |   *  |   (  |   )  | Del  |
 * |------+------+------+------+------+-------------+------+------+------+------+------|
 * |CapLk |  F1  |  F2  |  F3  |  F4  |  F5  |  F6  |   -  |   +  |   {  |   }  |  |   |
 * |------+------+------+------+------+------|------+------+------+------+------+------|
 * |      |  F7  |  F8  |  F9  |  F10 |  F11 |  F12 |ISO ~ |ISO | | Home | End  |TG RGB|
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |< RGB |      |      |      |             |      | Next | Vol- | Vol+ | Play |
 * `-----------------------------------------------------------------------------------'
 */
[_LOWER] = LAYOUT_preonic_grid(
  KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_BSPC,
  KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_DEL,
  KC_CAPS, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_MINS, KC_PLUS, KC_LCBR, KC_RCBR, KC_PIPE,
  _______, KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,S(KC_NUHS),S(KC_NUBS),KC_HOME, KC_END, RGB_TOG,
  _______, RGB_MOD, _______, _______, _______, _______, _______, _______, KC_MNXT, KC_VOLD, KC_VOLU, KC_MPLY
),

/* Raise
 * ,-----------------------------------------------------------------------------------.
 * |   `  |   1  |   2  |   3  |   4  |   5  |   6  |   7  |   8  |   9  |   0  | Bksp |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |   `  |   1  |   2  |   3  |   4  |   5  |   6  |   7  |   8  |   9  |   9  | Del  |
 * |------+------+------+------+------+-------------+------+------+------+------+------|
 * | Del  |  F1  |  F2  |  F3  |  F4  |  F5  |  F6  |   _  |   =  |   [  |   ]  |  \   |
 * |------+------+------+------+------+------|------+------+------+------+------+------|
 * |      |  F7  |  F8  |  F9  |  F10 |  F11 |  F12 |ISO # |ISO / | Pg Up| Pg Dn|      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |> RGB |      |      |      |             |      | Next |ScrBr-|ScrBr+| Play |
 * `-----------------------------------------------------------------------------------'
 */
[_RAISE] = LAYOUT_preonic_grid(
  KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
  KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_DEL,
  KC_DEL,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_UNDS, KC_EQL,  KC_LBRC, KC_RBRC, KC_BSLS,
  _______, KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_NUHS, KC_NUBS, KC_PGUP, KC_PGDN, _______,
  _______, RGB_RMOD, _______, _______, _______, _______, _______, _______, KC_MNXT, KC_BRID, KC_BRIU, KC_MPLY
),

/* Adjust (Lower + Raise)
 * ,-----------------------------------------------------------------------------------.
 * |      |      |      |      |      |             |      |      |      |      |Reset |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |Qwerty|QwerLi|      |      |      |      |      |      |      |      |      |
 * |------+------+------+------+------+-------------+------+------+------+------+------|
 * |      |      |      |      |      |      |      |      |      |      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      |      |      |      |      |      |      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      |      |             |      |      |      |      |      |
 * `-----------------------------------------------------------------------------------'
 */
[_ADJUST] = LAYOUT_preonic_grid(
  _______, _______,    _______,          _______, _______, _______, _______, _______, _______, _______, _______, QK_BOOT,
  _______, DF(QWERTY), DF(QWERTY_LINUX), _______, _______, _______, _______, _______, _______, _______, _______, _______,
  _______, _______,    _______,          _______, _______, _______, _______, _______, _______, _______, _______, _______,
  _______, _______,    _______,          _______, _______, _______, _______, _______, _______, _______, _______, _______,
  _______, _______,    _______,          _______, _______, _______, _______, _______, _______, _______, _______, _______
),


/* Esc
 * ,-----------------------------------------------------------------------------------.
 * |      |      |      |      |      |      |      |      |      |      |      |      |
 * |------+------+------+------+------+-------------+------+------+------+------+------|
 * |      |      |      |      |      |      | Left | Down |  Up  |Right |      |      |
 * |------+------+------+------+------+------|------+------+------+------+------+------|
 * |      |      |      |      |      |      |      |      |      |      |      |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      |      |             |      |      |      |      |      |
 * `-----------------------------------------------------------------------------------'
 */
[_ESC] = LAYOUT_preonic_grid(
  _______,      _______,        _______, _______,        _______,    _______,    _______,       _______,        _______,      _______,      _______, TD(TD_DEL),
  ACCENT_GRAVE, ACCENT_GRAVE,   _______, ACCENT_E_GRAVE, JET_RNM,    _______,    ACCENT_CIRCUM, KC_WH_D,        KC_WH_U,      _______,      _______, TD(TD_DEL),
  _______,      ACCENT_A_GRAVE, _______, _______,        JET_FIND,   _______,    TD(TD_LEFT),   KC_DOWN,        KC_UP,        TD(TD_RIGHT), _______, ACCENT_TREMA,
  _______,      _______,        _______, _______,        _______,    _______,    KC_MS_LEFT,    KC_MS_DOWN,     KC_MS_UP,     KC_MS_RIGHT,  _______, _______,
  _______,      _______,        _______, _______,        KC_MS_BTN2, KC_MS_BTN2, _______,       _______,        _______,      _______,      _______, _______
),

[_ESC_LINUX] = LAYOUT_preonic_grid(
  _______,      _______,        _______, _______,        _______,    _______,    _______,         _______,        _______,      _______,          _______, TD(TD_DEL_LIN),
  ACCENT_GRAVE, ACCENT_GRAVE,   _______, ACCENT_E_GRAVE, JET_RNM,    _______,    ACCENT_CIRCUM,   KC_WH_D,        KC_WH_U,      _______,          _______, TD(TD_DEL_LIN),
  _______,      ACCENT_A_GRAVE, _______, _______,        JET_FIND,   _______,    TD(TD_LEFT_LIN), KC_DOWN,        KC_UP,        TD(TD_RIGHT_LIN), _______, ACCENT_TREMA,
  _______,      _______,        _______, _______,        _______,    _______,    KC_MS_LEFT,      KC_MS_DOWN,     KC_MS_UP,     KC_MS_RIGHT,      _______, _______,
  _______,      _______,        _______, _______,        KC_MS_BTN2, KC_MS_BTN2, _______,         _______,        _______,      _______,          _______, _______
),

/* NumPads
 * ,-----------------------------------------------------------------------------------.
 * |      |      |      |      |      |      |      |      |  7   |  8   |  9   | Bksp |
 * |------+------+------+------+------+-------------+------+------+------+------+------|
 * |      |      |      |      |      |      |      |      |  4   |  5   |  6   |  -   |
 * |------+------+------+------+------+------|------+------+------+------+------+------|
 * |      |      |      |      |      |      |      |      |  1   |  2   |  3   |  +   |
 * |------+------+------+------+------+------------------------------------------------|
 * |      |      |      |      |      |      |      |      |  0   |  .   |      |Enter |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      |      |             |      |      |      |      |      |
 * `-----------------------------------------------------------------------------------'
 */
[_NUM_PADS] = LAYOUT_preonic_grid(
  _______, _______, _______, _______, _______, _______, _______, _______,  _______,  _______,    _______, _______,
  _______, _______, _______, _______, _______, _______, _______, _______,  KC_7,    KC_8,        KC_9,    KC_BSPC,
  _______, _______, _______, _______, _______, _______, _______, _______,  KC_4,    KC_5,        KC_6,    KC_MINS,
  _______, _______, _______, _______, _______, _______, _______, _______,  KC_1,    KC_2,        KC_3,    KC_PLUS,
  _______, _______, _______, _______, _______, _______, _______, _______,  KC_0,    KC_DOT,      KC_DOT,  KC_ENT
),

[_ACCENTS_RALT] = LAYOUT_preonic_grid(
  _______, _______,             _______, _______,       _______, _______, _______, _______,            _______,            _______,            _______, _______,
  _______, _______,             _______, _______,       _______, _______, _______, ACCENT_U_AIGU_RALT, ACCENT_I_CIRC_RALT, ACCENT_O_CIRC_RALT, _______, _______,
  _______, ACCENT_A_GRAVE_RALT, _______, _______,       _______, _______, _______, _______,            _______,            _______,            _______, _______,
  _______, _______,             _______, ACCENT_C_RALT, _______, _______, _______, _______,            _______,            _______,            _______, _______,
  _______, _______,             _______, _______,       _______, _______, _______, _______,            _______,            _______,            _______, _______
)
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  tap_dance_action_t *action;

  switch (keycode) {
        case QWERTY:
          if (record->event.pressed) {
            set_single_persistent_default_layer(_QWERTY);
          }
          return false;
          break;

        case LOWER:
          if (record->event.pressed) {
            is_hold_tapdance_disabled = true;
            layer_on(_LOWER);
            update_tri_layer(_LOWER, _RAISE, _ADJUST);
          } else {
            layer_off(_LOWER);
            update_tri_layer(_LOWER, _RAISE, _ADJUST);
            is_hold_tapdance_disabled = false;
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

        case NUM_PADS:
          if (record->event.pressed) {
              is_hold_tapdance_disabled = true;
              layer_on(_NUM_PADS);
          } else {
              layer_off(_NUM_PADS);
              is_hold_tapdance_disabled = false;
          }
          break;

        case ACCENTS_RALT:
          if (record->event.pressed) {
              is_hold_tapdance_disabled = true;
              register_code(KC_RALT);
              layer_on(_ACCENTS_RALT);
          } else {
              unregister_code(KC_RALT);
              layer_off(_ACCENTS_RALT);
              is_hold_tapdance_disabled = false;
          }
          break;

        case BACKLIT:
          if (record->event.pressed) {
            register_code(KC_RSFT);
            #ifdef BACKLIGHT_ENABLE
              backlight_step();
            #endif
            #ifdef RGBLIGHT_ENABLE
              rgblight_step();
            #endif
            #ifdef __AVR__
            writePinLow(E6);
            #endif
          } else {
            unregister_code(KC_RSFT);
            #ifdef __AVR__
            writePinHigh(E6);
            #endif
          }
          return false;
          break;

        case KC_LALT:
        case KC_LGUI:
        case KC_LSFT:
          if (record->event.pressed) {
              is_hold_tapdance_disabled = true;
          } else {
              is_hold_tapdance_disabled = false;
          }
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

              // will be unregister by `ACCENTS_RALT`
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

              // will be unregister by `ACCENTS_RALT`
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

              // will be unregister by `ACCENTS_RALT`
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

              // will be unregister by `ACCENTS_RALT`
              register_code(KC_RALT);
          }
          break;

        case ACCENT_C_RALT:
          if (record->event.pressed) {
              register_code(KC_COMM);
          } else {
              unregister_code(KC_COMM);
              unregister_code(KC_RALT);

              // will be unregister by `ACCENTS_RALT`
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

        case TD(TD_O):  // list all tap dance keycodes with tap-hold configurations
        case TD(TD_ESC):
        case TD(TD_ESC_LINUX):
        case TD(TD_TAB):
        case TD(TD_P):
        case TD(TD_L):
        case TD(TD_ENT):
        case TD(TD_SCLN):
        case TD(TD_BSPC):
        case TD(TD_BSPC_LIN):
        case TD(TD_DEL):
        case TD(TD_DEL_LIN):
        case TD(TD_LEFT):
        case TD(TD_LEFT_LIN):
        case TD(TD_RIGHT):
        case TD(TD_RIGHT_LIN):
        case TD(TD_LEFT_RALT):
          // specific behavior for tap-hold layout
          if ((keycode == TD(TD_ESC) || keycode == TD(TD_ESC_LINUX)) && !record->event.pressed) {
              layer_off(_ESC);
              is_hold_tapdance_disabled = false;
          }

          if (keycode == TD(TD_LEFT_RALT) && !record->event.pressed) {
              layer_off(_ACCENTS_RALT);
              unregister_code(KC_RALT);
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
};

bool muse_mode = false;
uint8_t last_muse_note = 0;
uint16_t muse_counter = 0;
uint8_t muse_offset = 70;
uint16_t muse_tempo = 50;

bool encoder_update_user(uint8_t index, bool clockwise) {
  if (muse_mode) {
    if (IS_LAYER_ON(_RAISE)) {
      if (clockwise) {
        muse_offset++;
      } else {
        muse_offset--;
      }
    } else {
      if (clockwise) {
        muse_tempo+=1;
      } else {
        muse_tempo-=1;
      }
    }
  } else {
    if (clockwise) {
      register_code(KC_PGDN);
      unregister_code(KC_PGDN);
    } else {
      register_code(KC_PGUP);
      unregister_code(KC_PGUP);
    }
  }
    return true;
}

bool dip_switch_update_user(uint8_t index, bool active) {
    switch (index) {
        case 0:
            if (active) {
                layer_on(_ADJUST);
            } else {
                layer_off(_ADJUST);
            }
            break;
        case 1:
            if (active) {
                muse_mode = true;
            } else {
                muse_mode = false;
            }
    }
    return true;
}


void matrix_scan_user(void) {
#ifdef AUDIO_ENABLE
    if (muse_mode) {
        if (muse_counter == 0) {
            uint8_t muse_note = muse_offset + SCALE[muse_clock_pulse()];
            if (muse_note != last_muse_note) {
                stop_note(compute_freq_for_midi_note(last_muse_note));
                play_note(compute_freq_for_midi_note(muse_note), 0xF);
                last_muse_note = muse_note;
            }
        }
        muse_counter = (muse_counter + 1) % muse_tempo;
    } else {
        if (muse_counter) {
            stop_all_notes();
            muse_counter = 0;
        }
    }
#endif
}

bool music_mask_user(uint16_t keycode) {
  switch (keycode) {
    case RAISE:
    case LOWER:
      return false;
    default:
      return true;
  }
}

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
// e.g: TD_DEL/TD_DEL_LIN
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

void tap_dance_tap_hold_reset_layout(tap_dance_state_t *state, void *user_data) {
    is_hold_tapdance_disabled = false;
}

void tap_dance_tap_hold_finished_layout_with_ralt(tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_finished_layout(state, user_data);

    if (state->pressed) {
        register_code(KC_RALT);
    }
}

#define ACTION_TAP_DANCE_TAP_HOLD(tap, hold) \
    { .fn = {NULL, tap_dance_tap_hold_finished, tap_dance_tap_hold_reset}, .user_data = (void *)&((tap_dance_tap_hold_t){tap, hold, 0}), }

// allow call multiple tap dance simultaneously
// e.g: TD_DEL/TD_DEL_LIN
#define ACTION_TAP_DANCE_TAP_HOLD_UNPROTECTED(tap, hold) \
    { .fn = {NULL, tap_dance_tap_hold_finished_unprotected, tap_dance_tap_hold_reset}, .user_data = (void *)&((tap_dance_tap_hold_t){tap, hold, 0}), }

#define ACTION_TAP_DANCE_TAP_HOLD_LAYOUT(tap, hold) \
    { .fn = {NULL, tap_dance_tap_hold_finished_layout, tap_dance_tap_hold_reset_layout}, .user_data = (void *)&((tap_dance_tap_hold_t){tap, hold, 0}), }

#define ACTION_TAP_DANCE_TAP_HOLD_LEFT_RALT(tap, hold) \
    { .fn = {NULL, tap_dance_tap_hold_finished_layout_with_ralt, tap_dance_tap_hold_reset_layout}, .user_data = (void *)&((tap_dance_tap_hold_t){tap, hold, 0}), }
// END tap-hold

// Associate our tap dance key with its functionality
tap_dance_action_t tap_dance_actions[] = {
    [TD_ESC] = ACTION_TAP_DANCE_TAP_HOLD_LAYOUT(KC_ESC, _ESC),
    [TD_ESC_LINUX] = ACTION_TAP_DANCE_TAP_HOLD_LAYOUT(KC_ESC, _ESC_LINUX),
    [TD_LEFT_RALT] = ACTION_TAP_DANCE_TAP_HOLD_LEFT_RALT(KC_LEFT, _ACCENTS_RALT),
    [TD_TAB] = ACTION_TAP_DANCE_TAP_HOLD(KC_TAB, KC_TILD),
    [TD_O] = ACTION_TAP_DANCE_TAP_HOLD(KC_O, KC_LPRN),
    [TD_P] = ACTION_TAP_DANCE_TAP_HOLD(KC_P, KC_RPRN),
    [TD_L] = ACTION_TAP_DANCE_TAP_HOLD(KC_L, KC_LCBR),
    [TD_SCLN] = ACTION_TAP_DANCE_TAP_HOLD(KC_SCLN, KC_RCBR),
    [TD_ENT] = ACTION_TAP_DANCE_TAP_HOLD(KC_ENT, KC_LSFT),

    // same tap-dance
    // enable it for osx and linux
    [TD_BSPC] = ACTION_TAP_DANCE_TAP_HOLD(KC_BSPC, LALT(KC_BSPC)),
    [TD_BSPC_LIN] = ACTION_TAP_DANCE_TAP_HOLD(KC_BSPC, LCTL(KC_BSPC)),

    [TD_DEL] = ACTION_TAP_DANCE_TAP_HOLD_UNPROTECTED(KC_DEL, LALT(KC_DEL)),
    [TD_DEL_LIN] = ACTION_TAP_DANCE_TAP_HOLD_UNPROTECTED(KC_DEL, LCTL(KC_DEL)),

    [TD_LEFT] = ACTION_TAP_DANCE_TAP_HOLD_UNPROTECTED(KC_LEFT, LALT(KC_LEFT)),
    [TD_LEFT_LIN] = ACTION_TAP_DANCE_TAP_HOLD_UNPROTECTED(KC_LEFT, LCTL(KC_LEFT)),

    [TD_RIGHT] = ACTION_TAP_DANCE_TAP_HOLD_UNPROTECTED(KC_RIGHT, LALT(KC_RIGHT)),
    [TD_RIGHT_LIN] = ACTION_TAP_DANCE_TAP_HOLD_UNPROTECTED(KC_RIGHT, LCTL(KC_RIGHT)),
};

