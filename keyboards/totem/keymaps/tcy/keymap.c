/*
                                             ▀▀▀▀▀     ▀▀▀▀▀          ▀▀█▀▀
                                             ▄▀▀▀▄  ▄  ▄▀▀▀▄  ▄  ▄▀▀▀▄  █  ▄▀▀▀▄
                                             █   █  █  █   █  █  █   █  █  █   █
                                              ▀▀▀   █   ▀▀▀   █   ▀▀▀   ▀   ▀▀▀
                                                    █      ▄▄▄█▄▄▄    █   █
                                                    ▀      █  █  █     █▄█
                                                  ▀▀▀▀▀    █  █  █      ▀
                                                           ▀  ▀  ▀  D E F A U L T

▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄
*/

#include QMK_KEYBOARD_H
#include <stdio.h>
#include "totem.h"
#include "tapdance.c"

// ┌────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
// │ K E Y M A P S                                                                                                          │
// └────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘
// ▝▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▘

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

 /*
   ┌─────────────────────────────────────────────────┐
   │ q w e r t y                                     │      ╭╮╭╮╭╮╭╮
   └─────────────────────────────────────────────────┘      │╰╯╰╯╰╯│
             ┌─────────┬─────────┬─────────┬─────────┬──────╨──┐┌──╨──────┬─────────┬─────────┬─────────┬─────────┐
     ╌┄┈┈───═╡    Q    │    W    │    E    │    R    │    T    ││    Y    │    U    │    I    │   O/(   │   P/)   │
             ├─────────┼─────────┼─────────┼─────────┼─────────┤├─────────┼─────────┼─────────┼─────────┼─────────┤
             │  A/MOD  │    S    │    D    │    F    │    G    ││    H    │    J    │    K    │   L/{   │  ;/}    │
   ┌─────────┼─────────┼─────────┼─────────┼─────────┼─────────┤├─────────┼─────────┼─────────┼─────────┼─────────┼─────────┐
   │ SFT/ESC │    Z    │    X    │    C    │    V    │    B    ││    N    │    M    │    ,    │    .    │    /    │ENTR/RSFT│
   └─────────┴─────────┴─────────┼─────────┼─────────┼─────────┤├─────────┼─────────┼─────────┼─────────┴─────────┴─────────┘
                                 │CTRL/~   │LALT/TAB │  LOWER  ││SPC/RAISE│  BSPC   │ RALT/'  │
                                 └─────────┴─────────┴─────────┘└─────────┴─────────┴─────────┘*/

   [_QWERTY] = LAYOUT(
 //╷         ╷         ╷         ╷              ╷             ╷         ╷╷             ╷             ╷         ╷         ╷         ╷         ╷
              KC_Q,     KC_W,     KC_E,          KC_R,         KC_T,      KC_Y,         KC_U,         KC_I,     TD(TD_O), TD(TD_P),
              TD(TD_A), KC_S,     KC_D,          KC_F,         KC_G,      KC_H,         KC_J,         KC_K,     TD(TD_L), TD(TD_SCLN),
TD(TD_LSFT),  KC_Z,     KC_X,     KC_C,          KC_V,         KC_B,      KC_N,         KC_M,         KC_COMM,  KC_DOT,   KC_SLSH,             TD(TD_ENT),
                                  TD(TD_LCTL),   TD(TD_LALT),  LOWER,     TD(TD_SPC),   TD(TD_BSPC),  TD(TD_RALT)
 ),
 /*
   ┌─────────────────────────────────────────────────┐
   │ q w e r t y OSX                                 │      ╭╮╭╮╭╮╭╮
   └─────────────────────────────────────────────────┘      │╰╯╰╯╰╯│
             ┌─────────┬─────────┬─────────┬─────────┬──────╨──┐┌──╨──────┬─────────┬─────────┬─────────┬─────────┐
     ╌┄┈┈───═╡    Q    │    W    │    E    │    R    │    T    ││    Y    │    U    │    I    │   O/(   │   P/)   │
             ├─────────┼─────────┼─────────┼─────────┼─────────┤├─────────┼─────────┼─────────┼─────────┼─────────┤
             │  A/MOD  │    S    │    D    │    F    │    G    ││    H    │    J    │    K    │   L/{   │  ;/}    │
   ┌─────────┼─────────┼─────────┼─────────┼─────────┼─────────┤├─────────┼─────────┼─────────┼─────────┼─────────┼─────────┐
   │ SFT/ESC │    Z    │    X    │    C    │    V    │    B    ││    N    │    M    │    ,    │    .    │    /    │ENTR/RSFT│
   └─────────┴─────────┴─────────┼─────────┼─────────┼─────────┤├─────────┼─────────┼─────────┼─────────┴─────────┴─────────┘
                                 │CTRL/~   │LALT/TAB │  LOWER  ││SPC/RAISE│  BSPC   │ RALT/'  │
                                 └─────────┴─────────┴─────────┘└─────────┴─────────┴─────────┘*/
   [_QWERTY_OSX] = LAYOUT(
 //╷         ╷              ╷         ╷            ╷             ╷         ╷╷            ╷                ╷         ╷        ╷         ╷         ╷
              KC_Q,          KC_W,     KC_E,        KC_R,         KC_T,      KC_Y,        KC_U,            KC_I,    TD(TD_O), TD(TD_P),
              TD(TD_A_OSX),  KC_S,     KC_D,        KC_F,         KC_G,      KC_H,        KC_J,            KC_K,    TD(TD_L), TD(TD_SCLN),
TD(TD_LSFT),  KC_Z,          KC_X,     KC_C,        KC_V,         KC_B,      KC_N,        KC_M,            KC_COMM, KC_DOT,   KC_SLSH,            TD(TD_ENT),
                                       TD(TD_LCTL), TD(TD_LGUI),  LOWER,     TD(TD_SPC),  TD(TD_BSPC_OSX), TD(TD_RALT)
 ),
/*

   ╺━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╸

   ┌─────────────────────────────────────────────────┐
   │ l o w e r                                       │      ╭╮╭╮╭╮╭╮
   └─────────────────────────────────────────────────┘      │╰╯╰╯╰╯│
             ┌─────────┬─────────┬─────────┬─────────┬──────╨──┐┌──╨──────┬─────────┬─────────┬─────────┬─────────┐
     ╌┄┈┈───═╡    !    │    @    │    #    │    $    │    %    ││    ^    │    &    │    *    │    (    │    )    │
             ├─────────┼─────────┼─────────┼─────────┼─────────┤├─────────┼─────────┼─────────┼─────────┼─────────┤
             │    F1   │    F2   │    F3   │    F4   │    F5   ││   F6    │    -    │    +    │    {    │    }    │
   ┌─────────┼─────────┼─────────┼─────────┼─────────┼─────────┤├─────────┼─────────┼─────────┼─────────┼─────────┼─────────┐
   │  LSFT   │    F7   │   F8    │   F9    │   F10   │   F11   ││   F1    │    ~    │ HOME    │   END   │         │    |    │
   └─────────┴─────────┴─────────┼─────────┼─────────┼─────────┤├─────────┼─────────┼─────────┼─────────┴─────────┴─────────┘
                                 │    ▼    │    ▼    │    ▼    ││  BTN1   │         │    0    │
                                 └─────────┴─────────┴─────────┘└─────────┴─────────┴─────────┘ */

   [_LOWER] = LAYOUT(
 //╷         ╷         ╷         ╷         ╷         ╷         ╷╷            ╷           ╷         ╷         ╷         ╷         ╷
              KC_EXLM,  KC_AT,    KC_HASH,  KC_DLR,   KC_PERC,   KC_CIRC,     KC_AMPR,    KC_ASTR,  KC_LPRN,  KC_PPLS,
              KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,     KC_F6,       KC_MINS,    KC_PLUS,  KC_LCBR,  KC_RCBR,
    KC_LSFT,  KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,    KC_F12,      S(KC_NUHS), KC_HOME,  KC_END,   _______,  KC_PIPE,
                                  _______,  _______,  _______,   KC_MS_BTN1,  _______,   _______
 ),
 /*
   ╺━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╸

   ┌─────────────────────────────────────────────────┐
   │ r a i s e                                       │      ╭╮╭╮╭╮╭╮
   └─────────────────────────────────────────────────┘      │╰╯╰╯╰╯│
             ┌─────────┬─────────┬─────────┬─────────┬──────╨──┐┌──╨──────┬─────────┬─────────┬─────────┬─────────┐
     ╌┄┈┈───═╡    1    │    2    │    3    │    4    │    5    ││    6    │    7    │    8    │    9    │    0    │
             ├─────────┼─────────┼─────────┼─────────┼─────────┤├─────────┼─────────┼─────────┼─────────┼─────────┤
             │         │         │         │         │         ││         │    _    │    =    │    [    │    ]    │
   ┌─────────┼─────────┼─────────┼─────────┼─────────┼─────────┤├─────────┼─────────┼─────────┼─────────┼─────────┼─────────┐
   │         │         │         │         │         │         ││         │         │         │         │         │    \    │
   └─────────┴─────────┴─────────┼─────────┼─────────┼─────────┤├─────────┼─────────┼─────────┼─────────┴─────────┴─────────┘
                                 │         │         │    ▼    ││    ▼    │    ▼    │    ▼    │
                                 └─────────┴─────────┴─────────┘└─────────┴─────────┴─────────┘ */
   [_RAISE] = LAYOUT(
 //╷         ╷                ╷         ╷         ╷         ╷         ╷╷          ╷         ╷         ╷         ╷         ╷         ╷
              KC_1,            KC_2,     KC_3,      KC_4,     KC_5,      KC_6,     KC_7,     KC_8,     KC_9,     KC_0,
              ACCENT_A_GRAVE,  _______,  _______,   _______,  _______,   _______,  KC_UNDS,  KC_EQL,  KC_LBRC,  KC_RBRC,
    _______,  _______,         _______,  ACCENT_C,  _______,  _______,   _______,  _______,  _______, _______,  _______,  KC_BSLS,
                                         _______,   _______,  _______,   _______,  _______,  _______
 ),
/*
   ╺━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╸

   ┌─────────────────────────────────────────────────┐
   │ MOD                                             │      ╭╮╭╮╭╮╭╮
   └─────────────────────────────────────────────────┘      │╰╯╰╯╰╯│
             ┌─────────┬─────────┬─────────┬─────────┬──────╨──┐┌──╨──────┬─────────┬─────────┬─────────┬─────────┐
     ╌┄┈┈───═╡    `    │         │    è    │JET_RNM  │         ││    ^    │KC_WH_D  │KC_WH_D  │KC_WH_U  │         │
             ├─────────┼─────────┼─────────┼─────────┼─────────┤├─────────┼─────────┼─────────┼─────────┼─────────┤
             │         │         │         │JET_FND  │         ││  LEFT   │  DOWN   │  UP     │ RIGHT   │         │
   ┌─────────┼─────────┼─────────┼─────────┼─────────┼─────────┤├─────────┼─────────┼─────────┼─────────┼─────────┼─────────┐
   │         │         │         │         │         │         ││         │         │         │         │         │         │
   └─────────┴─────────┴─────────┼─────────┼─────────┼─────────┤├─────────┼─────────┼─────────┼─────────┴─────────┴─────────┘
                                 │         │         │ BTN2    ││  BTN1   │  DEL    │ TREMA   │
                                 └─────────┴─────────┴─────────┘└─────────┴─────────┴─────────┘ */

   [_MOD] = LAYOUT(
 //╷         ╷               ╷         ╷                 ╷            ╷            ╷╷              ╷              ╷         ╷              ╷         ╷        ╷
              ACCENT_GRAVE,   A_W,      ACCENT_E_GRAVE,   JET_RNM,     A_T,          ACCENT_CIRCUM,  KC_WH_D,      KC_WH_U,  A_O,           A_P,
              _______,        A_S,      A_D,              JET_FIND,    A_H,          TD(TD_LEFT),    KC_DOWN,      KC_UP,    TD(TD_RIGHT),  _______,
    _______,  _______,        A_X,      A_C,              A_V,         A_B,          A_N,            A_M,          A_K,      A_L,           KC_QUOT,  _______,
                                        _______,          _______,     KC_MS_BTN2,   KC_MS_BTN1,     TD(TD_DEL),   ACCENT_TREMA
 ),
/*
   ╺━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╸

   ┌─────────────────────────────────────────────────┐
   │ MOD OSX                                         │      ╭╮╭╮╭╮╭╮
   └─────────────────────────────────────────────────┘      │╰╯╰╯╰╯│
             ┌─────────┬─────────┬─────────┬─────────┬──────╨──┐┌──╨──────┬─────────┬─────────┬─────────┬─────────┐
     ╌┄┈┈───═╡    `    │         │    è    │JET_RNM  │         ││    ^    │KC_WH_D  │KC_WH_D  │KC_WH_U  │         │
             ├─────────┼─────────┼─────────┼─────────┼─────────┤├─────────┼─────────┼─────────┼─────────┼─────────┤
             │         │         │         │JET_FND  │         ││  LEFT   │  DOWN   │  UP     │ RIGHT   │         │
   ┌─────────┼─────────┼─────────┼─────────┼─────────┼─────────┤├─────────┼─────────┼─────────┼─────────┼─────────┼─────────┐
   │         │         │         │         │         │         ││         │         │         │         │         │         │
   └─────────┴─────────┴─────────┼─────────┼─────────┼─────────┤├─────────┼─────────┼─────────┼─────────┴─────────┴─────────┘
                                 │         │         │ BTN2    ││  BTN1   │  DEL    │ TREMA   │
                                 └─────────┴─────────┴─────────┘└─────────┴─────────┴─────────┘ */

   [_MOD_OSX] = LAYOUT(
 //╷         ╷               ╷         ╷                  ╷            ╷         ╷╷                    ╷              ╷                ╷                  ╷         ╷         ╷
              ACCENT_GRAVE,   A_W,      ACCENT_E_GRAVE,   JET_RNM,     A_T,          ACCENT_CIRCUM,    KC_WH_D,        KC_WH_U,         A_O,               A_P,
              _______,        A_S,      A_D,              JET_FIND,    A_H,          TD(TD_LEFT_OSX),  KC_DOWN,        KC_UP,           TD(TD_RIGHT_OSX),  _______,
    _______,  _______,        A_X,      A_C,              A_V,         A_B,          A_N,              A_M,            A_K,             A_L,               KC_QUOT,  _______,
                                        _______,          _______,     KC_MS_BTN2,   KC_MS_BTN1,       TD(TD_DEL_OSX), ACCENT_TREMA
 ),
 /*
   ╺━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╸

   ┌─────────────────────────────────────────────────┐
   │ a d j u s t                                     │      ╭╮╭╮╭╮╭╮
   └─────────────────────────────────────────────────┘      │╰╯╰╯╰╯│
             ┌─────────┬─────────┬─────────┬─────────┬──────╨──┐┌──╨──────┬─────────┬─────────┬─────────┬─────────┐
     ╌┄┈┈───═╡ QWERTY  │QWER_OSX │         │         │         ││         │         │         │         │  RESET  │
             ├─────────┼─────────┼─────────┼─────────┼─────────┤├─────────┼─────────┼─────────┼─────────┼─────────┤
             │ DEBUG   │         │         │         │         ││         │         │         │         │         │
   ┌─────────┼─────────┼─────────┼─────────┼─────────┼─────────┤├─────────┼─────────┼─────────┼─────────┼─────────┼─────────┐
   │         │         │         │         │         │         ││         │         │         │         │         │         │
   └─────────┴─────────┴─────────┼─────────┼─────────┼─────────┤├─────────┼─────────┼─────────┼─────────┴─────────┴─────────┘
                                 │    ▼    │    ▼    │    ▼    ││    ▼    │    ▼    │    ▼    │
                                 └─────────┴─────────┴─────────┘└─────────┴─────────┴─────────┘ */

   [_ADJUST] = LAYOUT(
 //╷         ╷         ╷         ╷         ╷         ╷         ╷╷         ╷         ╷         ╷        ╷         ╷         ╷
              QWERTY,  QWERTY_OSX,_______,  _______,  _______,   _______,  _______,  _______, _______,  QK_BOOT,
              _______,  _______,  _______,  _______,  _______,   _______,  _______,  _______, _______,  _______,
    _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______,  _______, _______,  _______,  _______,
                                  _______,  _______,  _______,   _______,  _______,  _______
 )
/*
   ╺━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╸

   ┌─────────────────────────────────────────────────┐
   │ t e m p l a t e                                 │      ╭╮╭╮╭╮╭╮
   └─────────────────────────────────────────────────┘      │╰╯╰╯╰╯│
             ┌─────────┬─────────┬─────────┬─────────┬──────╨──┐┌──╨──────┬─────────┬─────────┬─────────┬─────────┐
     ╌┄┈┈───═╡         │         │         │         │         ││         │         │         │         │         │
             ├─────────┼─────────┼─────────┼─────────┼─────────┤├─────────┼─────────┼─────────┼─────────┼─────────┤
             │         │         │         │         │         ││         │         │         │         │         │
   ┌─────────┼─────────┼─────────┼─────────┼─────────┼─────────┤├─────────┼─────────┼─────────┼─────────┼─────────┼─────────┐
   │         │         │         │         │         │         ││         │         │         │         │         │         │
   └─────────┴─────────┴─────────┼─────────┼─────────┼─────────┤├─────────┼─────────┼─────────┼─────────┴─────────┴─────────┘
                                 │         │         │         ││         │         │         │
                                 └─────────┴─────────┴─────────┘└─────────┴─────────┴─────────┘

   [_TEMPLATE] = LAYOUT(
 //╷         ╷         ╷         ╷         ╷         ╷         ╷╷         ╷         ╷         ╷         ╷         ╷         ╷
              _______,  _______,  _______,  _______,   _______,  _______,  _______,  _______,  _______,   _______,
              _______,  _______,  _______,  _______,   _______,  _______,  _______,  _______,  _______,   _______,
    _______,  _______,  _______,  _______,  _______,   _______,  _______,  _______,  _______,  _______,   _______, _______,
                                  _______,  _______,   _______,  _______,  _______,  _______
 ),
*/
};

// Associate our tap dance key with its functionality
tap_dance_action_t tap_dance_actions[] = {
    [TD_A] = ACTION_TAP_DANCE_TAP_HOLD_PERMISIVE_LAYOUT(KC_A, _MOD),
    [TD_A_OSX] = ACTION_TAP_DANCE_TAP_HOLD_PERMISIVE_LAYOUT(KC_A, _MOD_OSX),
    [TD_SPC] = ACTION_TAP_DANCE_TAP_HOLD_PERMISIVE_LAYOUT(KC_SPC, _RAISE),
    [TD_TAB] = ACTION_TAP_DANCE_TAP_HOLD(KC_TAB, KC_TILD),
    [TD_O] = ACTION_TAP_DANCE_TAP_HOLD(KC_O, KC_LPRN),
    [TD_P] = ACTION_TAP_DANCE_TAP_HOLD(KC_P, KC_RPRN),
    [TD_L] = ACTION_TAP_DANCE_TAP_HOLD(KC_L, KC_LCBR),
    [TD_SCLN] = ACTION_TAP_DANCE_TAP_HOLD(KC_SCLN, KC_RCBR),
    [TD_ENT] = ACTION_TAP_DANCE_TAP_HOLD(KC_ENT, KC_LSFT),
    [TD_LSFT] = ACTION_TAP_DANCE_TAP_HOLD(KC_ESC, KC_LSFT),
    [TD_LCTL] = ACTION_TAP_DANCE_TAP_HOLD(KC_TILD, KC_LCTL),
    [TD_LALT] = ACTION_TAP_DANCE_TAP_HOLD(KC_TAB, KC_LALT),
    [TD_RALT] = ACTION_TAP_DANCE_TAP_HOLD(KC_QUOT, KC_RALT),
    [TD_LGUI] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_lgui_finished, td_lgui_reset),

    // same tap-dance
    // enable it for osx and linux
    [TD_BSPC] = ACTION_TAP_DANCE_TAP_HOLD(KC_BSPC, LCTL(KC_BSPC)),
    [TD_BSPC_OSX] = ACTION_TAP_DANCE_TAP_HOLD(KC_BSPC, LGUI(KC_BSPC)),

    [TD_DEL] = ACTION_TAP_DANCE_TAP_HOLD_UNPROTECTED(KC_DEL, LCTL(KC_DEL)),
    [TD_DEL_OSX] = ACTION_TAP_DANCE_TAP_HOLD_UNPROTECTED(KC_DEL, LALT(KC_DEL)),

    [TD_LEFT] = ACTION_TAP_DANCE_TAP_HOLD_UNPROTECTED(KC_LEFT, LCTL(KC_LEFT)),
    [TD_LEFT_OSX] = ACTION_TAP_DANCE_TAP_HOLD_UNPROTECTED(KC_LEFT, LALT(KC_LEFT)),

    [TD_RIGHT] = ACTION_TAP_DANCE_TAP_HOLD_UNPROTECTED(KC_RIGHT, LCTL(KC_RIGHT)),
    [TD_RIGHT_OSX] = ACTION_TAP_DANCE_TAP_HOLD_UNPROTECTED(KC_RIGHT, LALT(KC_RIGHT)),
};

// Set a long-ish tapping term for tap-dance keys
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case TD(TD_LSFT):
            return TAPPING_TERM - 20;
        case TD(TD_SPC):
            return TAPPING_TERM - 20;
        case TD(TD_A):
        case TD(TD_A_OSX):
            return TAPPING_TERM - 25;
        case TD(TD_LGUI):
            return TAPPING_TERM + 150;
        default:
            return TAPPING_TERM;
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  tap_dance_action_t *action;

  switch (keycode) {
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

    case A_W:
      if (record->event.pressed) {
          register_code(KC_A);
          register_code(KC_W);
          unregister_code(KC_A);
      } else {
          unregister_code(KC_W);
      }
      touched_td = true;
      break;

    case A_T:
      if (record->event.pressed) {
          register_code(KC_A);
          register_code(KC_T);
          unregister_code(KC_A);
      } else {
          unregister_code(KC_T);
      }
      touched_td = true;
      break;

    case A_O:
      if (record->event.pressed) {
          register_code(KC_A);
          register_code(KC_O);
          unregister_code(KC_A);
      } else {
          unregister_code(KC_O);
      }
      touched_td = true;
      break;

    case A_P:
      if (record->event.pressed) {
          register_code(KC_A);
          register_code(KC_P);
          unregister_code(KC_A);
      } else {
          unregister_code(KC_P);
      }
      touched_td = true;
      break;

    case A_S:
      if (record->event.pressed) {
          register_code(KC_A);
          register_code(KC_S);
          unregister_code(KC_A);
      } else {
          unregister_code(KC_S);
      }
      touched_td = true;
      break;

    case A_D:
      if (record->event.pressed) {
          register_code(KC_A);
          register_code(KC_D);
          unregister_code(KC_A);
      } else {
          unregister_code(KC_D);
      }
      touched_td = true;
      break;

    case A_H:
      if (record->event.pressed) {
          register_code(KC_A);
          register_code(KC_H);
          unregister_code(KC_A);
      } else {
          unregister_code(KC_H);
      }
      touched_td = true;
      break;

    case A_X:
      if (record->event.pressed) {
          register_code(KC_A);
          register_code(KC_X);
          unregister_code(KC_A);
      } else {
          unregister_code(KC_X);
      }
      touched_td = true;
      break;

    case A_C:
      if (record->event.pressed) {
          register_code(KC_A);
          register_code(KC_C);
          unregister_code(KC_A);
      } else {
          unregister_code(KC_C);
      }
      touched_td = true;
      break;

    case A_V:
      if (record->event.pressed) {
          register_code(KC_A);
          register_code(KC_V);
          unregister_code(KC_A);
      } else {
          unregister_code(KC_V);
      }
      touched_td = true;
      break;

    case A_B:
      if (record->event.pressed) {
          register_code(KC_A);
          register_code(KC_B);
          unregister_code(KC_A);
      } else {
          unregister_code(KC_B);
      }
      touched_td = true;
      break;

    case A_N:
      if (record->event.pressed) {
          register_code(KC_A);
          register_code(KC_N);
          unregister_code(KC_A);
      } else {
          unregister_code(KC_N);
      }
      touched_td = true;
      break;

    case A_M:
      if (record->event.pressed) {
          register_code(KC_A);
          register_code(KC_M);
          unregister_code(KC_A);
      } else {
          unregister_code(KC_M);
      }
      touched_td = true;
      break;

    case A_K:
      if (record->event.pressed) {
          register_code(KC_A);
          register_code(KC_K);
          unregister_code(KC_A);
      } else {
          unregister_code(KC_K);
      }
      touched_td = true;
      break;

    case A_L:
      if (record->event.pressed) {
          register_code(KC_A);
          register_code(KC_L);
          unregister_code(KC_A);
      } else {
          unregister_code(KC_L);
      }
      touched_td = true;
      break;

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
      touched_td = true;
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
      touched_td = true;
      break;

    case ACCENT_C:
      if (record->event.pressed) {
          register_code(KC_RALT);
          register_code(KC_COMM);
      } else {
          unregister_code(KC_COMM);
          unregister_code(KC_RALT);
      }
      touched_td = true;
      break;

    case ACCENT_GRAVE:
      if (record->event.pressed) {
          register_code(KC_RALT);
          register_code(KC_GRV);
      } else {
          unregister_code(KC_GRV);
          unregister_code(KC_RALT);
      }
      touched_td = true;
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
      touched_td = true;
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
       touched_td = true;
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
       touched_td = true;
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
       touched_td = true;
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
       touched_td = true;
       break;

     case ACCENT_C_RALT:
       if (record->event.pressed) {
           register_code(KC_RALT);
           register_code(KC_GRV);
       } else {
           unregister_code(KC_GRV);
           unregister_code(KC_RALT);
           register_code(KC_A);
           unregister_code(KC_A);
       }
       touched_td = true;
       break;

     case JET_RNM:
       if (record->event.pressed) {
           register_code(KC_LSFT);
           register_code(KC_F6);

           unregister_code(KC_LSFT);
           unregister_code(KC_F6);
       }
       touched_td = true;
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
       touched_td = true;
       return false;
       break;

    case TD(TD_O):  // list all tap dance keycodes with tap-hold configurations
    case TD(TD_A):
    case TD(TD_A_OSX):
    case TD(TD_SPC):
    case TD(TD_TAB):
    case TD(TD_LCTL):
    case TD(TD_LSFT):
    case TD(TD_LALT):
    case TD(TD_RALT):
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
      if ((keycode == TD(TD_A) || keycode == TD(TD_A_OSX)) && !record->event.pressed) {
          layer_off(_MOD);
          layer_off(_MOD_OSX);
          is_hold_tapdance_disabled = false;
      } else if (keycode == TD(TD_SPC) && !record->event.pressed) {
          layer_off(_RAISE);
          is_hold_tapdance_disabled = false;
      }

      action = &tap_dance_actions[TD_INDEX(keycode)];
      if (!record->event.pressed && action->state.count && !action->state.finished) {
          tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)action->user_data;
          tap_code16(tap_hold->tap);
      }

      if ((keycode == TD(TD_A) || keycode == TD(TD_A_OSX) || keycode == TD(TD_SPC)) && !touched_td && !record->event.pressed && action->state.finished) {
          tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)action->user_data;
          tap_code16(tap_hold->tap);
      }
      touched_td = true;
      break;
  }

  touched_td = true;
  return true;
}


/*
  ╺━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━╸


                                                          ██████
                                                            ██  ▄▄▄▄
                                                            ██ ██▀▀██
                                                            ▀▀ ██▄▄██
                                                        ██████  ▀▀▀▀
                                                          ██ ▄▄▄▄▄▄
                                                          ██ ██▀▀▀▀
                                                          ██ ██████
                                                             ██▄▄▄▄
                                                             ▀▀▀▀▀▀
                                                           ████████
                                                           ██ ██ ██
                                                           ██ ██ ██
                                                           ▀▀ ▀▀ ▀▀
                                                          ████████

*/



