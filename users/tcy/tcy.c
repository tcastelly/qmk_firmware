#include QMK_KEYBOARD_H
#include "i2c_master.h"
#include "spi_master.h"
#include "tapdance.h"
#include "print.h"

#ifdef PMW3360_CUSTOM_ENABLE
#include "drivers/sensors/pmw33xx_common.h"
#endif

#ifdef POINTING_DEVICE_ENABLE
#include "pointing_device.h"
#endif

#ifdef AUDIO_ENABLE
#include "audio.h"

enum buzz_modes {
  BUZZ_ON,
  BUZZ_OFF,
};

int8_t buzz_mode = BUZZ_OFF;

float layer_sound_on[][2] = SONG(STARTUP_SOUND);
#endif

#ifdef OLED_ENABLE
enum oled_modes {
  OLED_BONGO,
  OLED_MINIMAL,
  OLED_OFF,
};


// prevent the oled to comeback on after typing
bool keep_oled_off = false;
#include "oled_bongo.c"
#endif

#ifdef OLED_ENABLE_MINIMAL
int8_t oled_mode = OLED_MINIMAL;
#include "oled_minimal.c"
#endif

#ifndef OLED_ENABLE_MINIMAL
int8_t oled_mode = OLED_BONGO;
#endif


uint8_t ps2_acceleration_setting = PS2_DEFAULT_ACCELERATION_SETTING;

uint8_t current_layer = 0;

// Reach into matrix.c to get the button state
extern uint8_t mcp_click_state; 

static uint32_t key_timer = 0;

static bool is_rgb_off = false;

static bool keep_rgb_off = false;

static bool lock_mode = false;

// Associate our tap dance key with its functionality
tap_dance_action_t tap_dance_actions[] = {
    [TD_A] = ACTION_TAP_DANCE_TAP_HOLD(KC_A, KC_LCTL),
    [TD_A_OSX] = ACTION_TAP_DANCE_TAP_HOLD(KC_A, KC_LCTL),
    [TD_ESC] = ACTION_TAP_DANCE_TAP_HOLD_LAYOUT(KC_ESC, _ESC),
    [TD_ESC_OSX] = ACTION_TAP_DANCE_TAP_HOLD_LAYOUT(KC_ESC, _ESC_OSX),
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

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  tap_dance_action_t *action;

  key_timer = timer_read32();  // resets timer

#ifdef AUDIO_ENABLE
  if (!record->event.pressed && buzz_mode == BUZZ_ON) {
    PLAY_SONG(layer_sound_on);
  }
#endif
                               
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

    case KC_CAPS:
        if (record->event.pressed) {
          is_kc_caps = !is_kc_caps;
        }
        return true;
        break;

    case KC_LALT:
    case KC_LGUI:
    case KC_LSFT:
      if (record->event.pressed) {
          ps2_acceleration_setting = PS2_MAX_ACCELERATION_SETTING;
#ifdef POINTING_DEVICE_ENABLE
         pointing_device_set_cpi(POINTING_DEVICE_MAX_CPI);
#endif
          is_hold_tapdance_disabled = true;
      } else {
          ps2_acceleration_setting = PS2_DEFAULT_ACCELERATION_SETTING;
#ifdef POINTING_DEVICE_ENABLE
         pointing_device_set_cpi(POINTING_DEVICE_DEFAULT_CPI);
#endif
          is_hold_tapdance_disabled = false;

          // OSX needs less speed
          if (get_highest_layer(layer_state) == _QWERTY_OSX) {
            ps2_acceleration_setting -= 1;
          }
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
           tap_code(KC_GRV);
           
           unregister_code(KC_RALT);
           tap_code(KC_A);
           
           // will be unregister by `td_ralt_reset`
           register_code(KC_RALT);
       }
       touched_td = true;
       break;

     case ACCENT_I_CIRC_RALT:
       if (record->event.pressed) {
           tap_code(KC_6);
           
           unregister_code(KC_RALT);
           tap_code(KC_I);
           
           // will be unregister by `td_ralt_reset`
           register_code(KC_RALT);
       }
       touched_td = true;
       break;

     case ACCENT_O_CIRC_RALT:
       if (record->event.pressed) {
           tap_code(KC_6);
           
           unregister_code(KC_RALT);
           tap_code(KC_O);
           
           // will be unregister by `td_ralt_reset`
           register_code(KC_RALT);
       }
       touched_td = true;
       break;

     case ACCENT_U_AIGU_RALT:
       if (record->event.pressed) {
           tap_code(KC_GRV);
           
           unregister_code(KC_RALT);
           tap_code(KC_U);
           
           // will be unregister by `td_ralt_reset`
           register_code(KC_RALT);
       }
       touched_td = true;
       break;

     case ACCENT_C_RALT:
       if (record->event.pressed) {
           tap_code(KC_COMM);
           // (No RALT unregister/register needed here since tap_code handles the comma cleanly)
       }
       touched_td = true;
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
       touched_td = true;
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

#ifdef OLED_ENABLE
     case TOGGLE_OLED:
       if (record->event.pressed) {
           if (oled_mode != OLED_OFF) {
               oled_mode = OLED_OFF;
               keep_oled_off = true;
           } else {
#ifdef OLED_ENABLE_MINIMAL
               oled_mode = OLED_MINIMAL;
#endif

#ifndef OLED_ENABLE_MINIMAL
               oled_mode = OLED_BONGO;
#endif

               keep_oled_off = false;
           }
       }
       return false;
       break;
#endif

#ifdef AUDIO_ENABLE
     case TOGGLE_BUZZ:
       if (record->event.pressed) {
           if (buzz_mode != BUZZ_OFF) {
               buzz_mode = BUZZ_OFF;
           } else {
               buzz_mode = BUZZ_ON;
           }
       }
       return false;
       break;
#endif

    case TD(TD_O):  // list all tap dance keycodes with tap-hold configurations
    case TD(TD_A):
    case TD(TD_A_OSX):
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
         layer_off(_ESC);
         layer_off(_ESC_OSX);
         is_hold_tapdance_disabled = false;
      }
      if (keycode == TD(TD_ESC) || keycode == TD(TD_ESC_OSX)) {
          scrolling_mode = record->event.pressed;
      }

      action = &tap_dance_actions[TD_INDEX(keycode)];
      tap_dance_state_t *state = tap_dance_get_state(TD_INDEX(keycode));

      if (!record->event.pressed && state->count && !state->finished) {
          tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)action->user_data;
          tap_code16(tap_hold->tap);
      }

      if ((keycode == TD(TD_A) || keycode == TD(TD_A_OSX)) && !touched_td && !record->event.pressed && state->finished) {
          unregister_code(KC_LCTL);
          tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)action->user_data;
          tap_code16(tap_hold->tap);
      }
      touched_td = true;
      break;
  }
  touched_td = true;
  return true;
}

void scan_i2c_bus(void) {
#ifdef CONSOLE_ENABLE
  uprintf("Starting I2C scan...\n");
  uint8_t dat = 0;
  for (uint8_t address = 1; address < 128; address++) {
    // We shift address left for the 7-bit + R/W format
    // Use i2c_receive if i2c_read continues to fail
    i2c_status_t status = i2c_receive(address << 1, &dat, 1, 100);
    if (status == I2C_STATUS_SUCCESS) {
      uprintf("Found device at address: 0x%02X\n", address);
    }
  }
  uprintf("Scan complete.\n");
#endif
}

void keyboard_post_init_user(void) {
#ifdef PMW3360_CUSTOM_ENABLE
    pmw33xx_init(0);
    pmw33xx_set_cpi(0, PMW33XX_CPI);
    wait_ms(500);
#endif

    wait_ms(500); // Let the trackpad boot

    scan_i2c_bus();

#ifdef OLED_ENABLE_MINIMAL
  oled_off();
  draw_minimal();
#endif
}

void play_audio(void) {
#ifdef AUDIO_ENABLE
  audio_play_melody(&layer_sound_on, 3, false);  // non-blocking
#endif
}

//
// trackpad
//

// Modify these values to adjust the scrolling speed
#define SCROLL_DIVISOR_H 8.0
#define SCROLL_DIVISOR_V 8.0

// Variables to store accumulated scroll values
float scroll_accumulated_h = 0;
float scroll_accumulated_v = 0;

report_mouse_t tcy_pointing_device_task(report_mouse_t mouse_report) {
    if (scrolling_mode) {
        // Calculate and accumulate scroll values based on mouse movement and divisors
        scroll_accumulated_h += (float)mouse_report.x / SCROLL_DIVISOR_H;
        scroll_accumulated_v += (float)mouse_report.y / SCROLL_DIVISOR_V;

        // Assign integer parts of accumulated scroll values to the mouse report
        mouse_report.h = (int8_t)scroll_accumulated_h;
        mouse_report.v = (int8_t)scroll_accumulated_v;

        // Update accumulated scroll values by subtracting the integer parts
        scroll_accumulated_h -= (int8_t)scroll_accumulated_h;
        scroll_accumulated_v -= (int8_t)scroll_accumulated_v;

        // Clear the X and Y values of the mouse report
        mouse_report.x = 0;
        mouse_report.y = 0;
    }

    if (lock_mode) {
        mouse_report.h = 0;
        mouse_report.v = 0;
        mouse_report.x = 0;
        mouse_report.y = 0;
    }

    bool has_moved = mouse_report.x > 0 || mouse_report.y > 0 || mouse_report.v > 0 || mouse_report.h > 0;

    if (is_rgb_off && !keep_rgb_off && has_moved) {
      key_timer = timer_read32();  // resets timer
#ifdef RGB_MATRIX_ENABLE
      rgb_matrix_enable_noeeprom();
#endif
      is_rgb_off = false;
    }

    return mouse_report;
}

#ifdef OLED_ENABLE
bool oled_task_user(void) {
    switch (oled_mode) {
        case OLED_BONGO:
            draw_bongo();
            break;
        case OLED_MINIMAL:
            draw_minimal();
            break;
        default:
        case OLED_OFF:
#ifdef OLED_ENABLE_MINIMAL
            _oled_off();
#endif

#ifndef OLED_ENABLE_MINIMAL
            oled_off();
#endif
            break;
    }
    return false;
}
#endif

layer_state_t layer_state_set_user(layer_state_t state) {
    current_layer = get_highest_layer(state);

    switch (current_layer) {
      case _QWERTY_OSX:
        // OSX needs less speed
        ps2_acceleration_setting = PS2_DEFAULT_ACCELERATION_SETTING;
        ps2_acceleration_setting -= 1;
        break;
      case _ESC:
      case _ESC_OSX:
        if (scrolling_mode) {
          ps2_acceleration_setting = PS2_MIN_ACCELERATION_SETTING;
        }
        break;
      default:
        ps2_acceleration_setting = PS2_DEFAULT_ACCELERATION_SETTING;
        break;
    }

    return state;
}

