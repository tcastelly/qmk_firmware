/*
This is the c configuration file for the keymap

Copyright 2022 @Yowkees
Copyright 2022 MURAOKA Taro (aka KoRoN, @kaoriya)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#ifdef RGBLIGHT_ENABLE
#    define RGBLIGHT_EFFECT_BREATHING
#    define RGBLIGHT_EFFECT_RAINBOW_MOOD
#    define RGBLIGHT_EFFECT_RAINBOW_SWIRL
#    define RGBLIGHT_EFFECT_SNAKE
#    define RGBLIGHT_EFFECT_KNIGHT
#    define RGBLIGHT_EFFECT_CHRISTMAS
#    define RGBLIGHT_EFFECT_STATIC_GRADIENT
#    define RGBLIGHT_EFFECT_RGB_TEST
#    define RGBLIGHT_EFFECT_ALTERNATING
#    define RGBLIGHT_EFFECT_TWINKLE
#endif

#define TAP_CODE_DELAY 5

#define TAPPING_TERM 175
#define TAPPING_TERM_PER_KEY

#define SPLIT_WPM_ENABLE
#define SPLIT_OLED_ENABLE
#define SPLIT_ACTIVITY_ENABLE
#define SPLIT_TRANSPORT_MIRROR

#define POINTING_DEVICE_ROTATION_90
#define SPLIT_POINTING_ENABLE
#define POINTING_DEVICE_RIGHT
#define POINTING_DEVICE_TASK_THROTTLE_MS 8

// TRACK_BALL_MIN_DPI used for scroll only
#define TRACK_BALL_MIN_DPI 2000
#define TRACK_BALL_DEFAULT_DPI 16500
#define TRACK_BALL_MAX_DPI 40000

#undef LOCKING_SUPPORT_ENABLE
#undef LOCKING_RESYNC_ENABLE
#define NO_ACTION_ONESHOT
#define NO_ACTION_TAPPING
#define NO_MUSIC_MODE
#define LAYER_STATE_8BIT
