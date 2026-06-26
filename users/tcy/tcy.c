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

#if defined(RGB_MATRIX_ENABLE) || defined(RGBLIGHT_ENABLE)
uint8_t COLOR_RED[3]    = {255, 0, 0};
uint8_t COLOR_GREEN[3]  = {0, 255, 0};
uint8_t COLOR_BLUE[3]   = {0, 0, 255};
uint8_t COLOR_PURPLE[3] = {191, 255, 100};
uint8_t COLOR_YELLOW[3] = {255, 255, 0};
uint8_t COLOR_PINK[3] = {255, 80, 120};
uint8_t COLOR_ORANGE[3] = {50, 15, 0};
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
  OLED_BONGO_MINI,
  OLED_OFF,
};

// prevent the oled to comeback on after typing
bool keep_oled_off = false;

// oled_bongo.c always included: provides frame data, state machine, draw_bongo()
#include "oled_bongo.c"

#ifdef OLED_ENABLE_MINIMAL
// oled_bongo_mini.c: 2:1 vertically-compressed frames for 16px OLED (256 bytes/frame)
#include "oled_bongo_mini.c"
int8_t oled_mode = OLED_BONGO_MINI;
#else
int8_t oled_mode = OLED_BONGO;
#endif
#endif

// disble trackpoint
bool disable_tp = false;

// use to know if we are on OSX layout
bool is_osx = false;

#if defined(PS2_ENABLE) || defined(PS2_CUSTOM_ENABLE)
uint8_t ps2_acceleration_setting = PS2_DEFAULT_ACCELERATION_SETTING;
#endif

uint8_t current_layer = 0;

// Reach into matrix.c to get the button state
extern uint8_t mcp_click_state; 

static uint32_t key_timer = 0;

#ifdef RGB_MATRIX_ENABLE
static bool is_rgb_off = false;
bool keep_rgb_off = false;
#endif

bool lock_mode = false;

bool esc_drag_active = false;

#ifdef TCY_FULL_TD
// Combo to be able to use Ctrl + z and `fg` with vim
// set when KC_X is pressed while TD_A tap dance is pending (not yet resolved)
static bool td_a_pressed  = false;
static bool td_a_z_combo  = false;
#endif

uint16_t bootloader_timer = 0;

bool bootloader_active = false;

// Associate our tap dance key with its functionality
tap_dance_action_t tap_dance_actions[] = {
#ifdef TCY_FULL_TD
    [TD_A]    = ACTION_TAP_DANCE_TAP_HOLD(KC_A, KC_LCTL),
#else
    [TD_A]    = ACTION_TAP_DANCE_FN_ADVANCED(NULL, NULL, NULL),
#endif
    [TD_ESC] = ACTION_TAP_DANCE_TAP_HOLD_LAYOUT(KC_ESC, _ESC),
    [TD_TAB] = ACTION_TAP_DANCE_TAP_HOLD(KC_TAB, KC_TILD),
    [TD_O] = ACTION_TAP_DANCE_TAP_HOLD(KC_O, KC_LPRN),
    [TD_P] = ACTION_TAP_DANCE_TAP_HOLD(KC_P, KC_RPRN),
    [TD_L] = ACTION_TAP_DANCE_TAP_HOLD(KC_L, KC_LCBR),
    [TD_SCLN] = ACTION_TAP_DANCE_TAP_HOLD(KC_SCLN, KC_RCBR),
    [TD_ENT] = ACTION_TAP_DANCE_TAP_HOLD(KC_ENT, KC_LSFT),

    [TD_RAISE] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_raise_finished, td_raise_reset),
#ifdef TCY_FULL_TD
    [TD_LOWER] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_lower_finished, td_lower_reset),
#else
    [TD_LOWER] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, NULL, NULL),
#endif
    [TD_LCTL] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_lctl_finished, td_lctl_reset),
    [TD_LALT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_lalt_finished, td_lalt_reset),
#ifdef TCY_FULL_TD
    [TD_LGUI] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_lgui_finished, td_lgui_reset),
#else
    [TD_LGUI] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, NULL, NULL),
#endif
    [TD_BSPC]  = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_word_bspc_finished, td_word_bspc_reset),
    [TD_DEL]   = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_word_del_finished,  td_word_del_reset),
    [TD_LEFT]  = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_word_left_finished, td_word_left_reset),
    [TD_RIGHT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_word_right_finished, td_word_right_reset),
#ifdef TCY_FULL_TD
    [TD_RALT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, td_ralt_finished, td_ralt_reset),
#else
    [TD_RALT] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, NULL, NULL),
#endif
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

#ifdef TCY_FULL_TD
bool pre_process_record_user(uint16_t keycode, keyrecord_t *record) {
    // Track TD_A here (not just in process_record_user) so td_a_pressed is
    // set before action_tapping_process potentially defers the TD_A event.
    if (keycode == TD(TD_A)) {
        td_a_pressed = record->event.pressed;
    }
    if (keycode == KC_X && record->event.pressed && td_a_pressed) {
        td_a_z_combo = true;
    }
    return true;
}
#endif

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  tap_dance_action_t *action;

  key_timer = timer_read32();  // resets timer
                               //
  bootloader_active = false;

#ifdef AUDIO_ENABLE
  if (!record->event.pressed && buzz_mode == BUZZ_ON) {
    PLAY_SONG(layer_sound_on);
  }
#endif
                               
  switch (keycode) {
    case QWERTY:
        if (record->event.pressed) {
            layer_move(_QWERTY);
#ifdef LAYER_STATE_8BIT
            is_osx = false;
#else
            layer_off(_OSX_SIGNAL);
#endif
        }
        return false;

    case QWERTY_OSX:
        if (record->event.pressed) {
            layer_move(_QWERTY);
#ifdef LAYER_STATE_8BIT
            is_osx = true;
#else
            layer_on(_OSX_SIGNAL);
#endif
        }
        return false;

    case QWERTY_GAMING:
        if (record->event.pressed) {
            layer_move(_QWERTY_GAMING);
        }
        return false;

    case LOWER:
      if (record->event.pressed) {
        bootloader_timer = timer_read();
        bootloader_active = true;

        hold_td_disable_count++;
        lock_mode = true;
        layer_on(_LOWER);
        update_tri_layer(_LOWER, _RAISE, _ADJUST);
      } else {
        bootloader_active = false;
        bootloader_timer = 0;

        lock_mode = false;
        layer_off(_LOWER);
        update_tri_layer(_LOWER, _RAISE, _ADJUST);
        if (hold_td_disable_count) hold_td_disable_count--;
      }
      return false;

    case RAISE:
      if (record->event.pressed) {
        bootloader_timer = timer_read();
        bootloader_active = true;

        hold_td_disable_count++;
        layer_on(_RAISE);
        update_tri_layer(_LOWER, _RAISE, _ADJUST);
      } else {
        bootloader_active = false;
        bootloader_timer = 0;

        layer_off(_RAISE);
        update_tri_layer(_LOWER, _RAISE, _ADJUST);
        if (hold_td_disable_count) hold_td_disable_count--;
      }
      return false;

#ifdef RGB_MATRIX_ENABLE
    case TOGGLE_RGB:
      if (record->event.pressed) {
        if (rgb_matrix_is_enabled()) {
          keep_rgb_off = true;
          rgb_matrix_disable_noeeprom();
          is_rgb_off = true;
        } else {
          rgb_matrix_enable_noeeprom();
          keep_rgb_off = false;
          is_rgb_off = false;
        }
      }
      return false;
#endif


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
#if defined(PS2_ENABLE) || defined(PS2_CUSTOM_ENABLE)
          ps2_acceleration_setting = PS2_MAX_ACCELERATION_SETTING;
#endif
#ifndef POINTING_DEVICE_COMBINED
#ifdef POINTING_DEVICE_ENABLE
#ifndef TCY_DISABLE_CPI_INIT
         pointing_device_set_cpi(POINTING_DEVICE_MAX_CPI);
#endif
#endif
#endif
          hold_td_disable_count++;
      } else {
#if defined(PS2_ENABLE) || defined(PS2_CUSTOM_ENABLE)
          ps2_acceleration_setting = PS2_DEFAULT_ACCELERATION_SETTING;
          if (is_osx) {
            ps2_acceleration_setting -= 1;
          }
#endif
#ifndef POINTING_DEVICE_COMBINED
#ifdef POINTING_DEVICE_ENABLE
#ifndef TCY_DISABLE_CPI_INIT
         pointing_device_set_cpi(POINTING_DEVICE_DEFAULT_CPI);
#endif
#endif
#endif
          if (hold_td_disable_count) hold_td_disable_count--;
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

#ifdef TCY_FULL_TD
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
#endif

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
              // Use tap_code16 with weak mods so we don't disturb any
              // "real" modifier the user may already be holding
              // (e.g. LALT via TD_LALT, LCTL via TD_LCTL).
              tap_code16(S(KC_F6));
          }
       return false;

      case JET_FIND:
          if (record->event.pressed) {
              tap_code16(LALT(KC_F1));
              tap_code(KC_1);
          }
          return false;

      case JET_OPTI:
          if (record->event.pressed) {
              tap_code16(LCTL(LALT(KC_O)));
          }
          return false;

      case JET_FORMAT:
          if (record->event.pressed) {
              tap_code16(is_osx ? LALT(LGUI(KC_L)) : LCTL(LALT(KC_L)));
          }
          return false;

#ifdef OLED_ENABLE
     case TOGGLE_OLED:
       if (record->event.pressed) {
           if (oled_mode != OLED_OFF) {
               oled_mode = OLED_OFF;
               keep_oled_off = true;
               layer_on(_OLED_OFF_SIGNAL);   // signal slave via synced layer state
           } else {
#ifdef OLED_ENABLE_MINIMAL
               oled_mode = OLED_BONGO_MINI;
#else
               oled_mode = OLED_BONGO;
#endif

               keep_oled_off = false;
               layer_off(_OLED_OFF_SIGNAL);  // signal slave: OLED back on
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

#ifdef TCY_FULL_TD
    case KC_X:
        if (td_a_z_combo) {
            // LCTL stays held (tap_hold->held untouched, reset by td_a release).
            // KC_Z tracks KC_X physically: held while X is down, released on X up.
            if (record->event.pressed) {
                touched_td = true;
                register_code(KC_Z);
            } else {
                unregister_code(KC_Z);
            }
            return false;
        }
        return true;
#endif

    case MS_BTN1:
        if (record->event.pressed) {
          esc_drag_active = true;
          return false;
        } else if (esc_drag_active) {
          esc_drag_active = false;
          return false;
        }
        return true;

    case TD(TD_O):  // list all tap dance keycodes with tap-hold configurations
    case TD(TD_A):
    case TD(TD_ESC):
    case TD(TD_TAB):
    case TD(TD_P):
    case TD(TD_L):
    case TD(TD_ENT):
    case TD(TD_SCLN):
    case TD(TD_BSPC):
    case TD(TD_DEL):
    case TD(TD_LEFT):
    case TD(TD_RIGHT):
        if (keycode == TD(TD_ESC) && !record->event.pressed) {
            /* Turn the layer off early so the button-remap checks
             * (IS_LAYER_ON(_ESC)) in keymap.c see the correct state on
             * this same release pass. The hold_td_disable_count-- is
             * handled solely in tap_dance_tap_hold_reset_layout to avoid
             * a double-decrement. layer_off here is idempotent with the
             * one in _reset_layout. */
            layer_off(_ESC);
            scrolling_mode = false;
        }
        if (keycode == TD(TD_ESC)) {
            scrolling_mode = record->event.pressed;
        }

      action = &tap_dance_actions[TD_INDEX(keycode)];
      tap_dance_state_t *state = tap_dance_get_state(TD_INDEX(keycode));

#ifdef TCY_FULL_TD
      if (keycode == TD(TD_A)) {
          td_a_pressed = record->event.pressed;
      }
#endif

      if (!record->event.pressed && state->count && !state->finished) {
          /* Clear QMK's captured weak-mods snapshot so it cannot be
           * re-applied on the upcoming finished/reset and stick on the
           * host after the physical modifier is released. */
          state->weak_mods = 0;
          if (action->user_data) {
              tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)action->user_data;
              tap_code16(tap_hold->tap);
          } else if (keycode == TD(TD_BSPC)) {
              tap_code16(KC_BSPC);
          } else if (keycode == TD(TD_DEL)) {
              tap_code16(KC_DEL);
          } else if (keycode == TD(TD_LEFT)) {
              tap_code16(KC_LEFT);
          } else if (keycode == TD(TD_RIGHT)) {
              tap_code16(KC_RIGHT);
          }
      }

#ifdef TCY_FULL_TD
      if (keycode == TD(TD_A) && !touched_td && !record->event.pressed && state->finished) {
          unregister_code(KC_LCTL);
          tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)action->user_data;
          tap_code16(tap_hold->tap);
      }

      // cleanup after TD_A + x → z combo: tap dance may have registered KC_A or
      // KC_LCTL after the combo fired (it never saw x); undo both.
      if (keycode == TD(TD_A) && !record->event.pressed && td_a_z_combo) {
          unregister_code(KC_LCTL);
          unregister_code(KC_A);
          unregister_code(KC_Z);  // in case KC_X still physically held
          td_a_z_combo = false;
      }
#endif

      touched_td = true;
      break;
  }
  touched_td = true;
  return true;
}

#ifdef CONSOLE_ENABLE
void scan_i2c_bus(void) {
  uprintf("Starting I2C scan...\n");
  uint8_t dat = 0;
  for (uint8_t address = 1; address < 128; address++) {
    // We shift address left for the 7-bit + R/W format
    // Use i2c_receive if i2c_read continues to fail
    i2c_status_t status = i2c_receive(address << 1, &dat, 1, 10);
    if (status == I2C_STATUS_SUCCESS) {
      uprintf("Found device at address: 0x%02X\n", address);
    }
  }
  uprintf("Scan complete.\n");
}
#endif

void keyboard_post_init_user(void) {
#ifdef PMW3360_CUSTOM_ENABLE
    pmw33xx_init(0);
    pmw33xx_set_cpi(0, PMW33XX_CPI);
    wait_ms(500);
#endif

    wait_ms(500); // Let the trackpad boot

#ifdef CONSOLE_ENABLE
    scan_i2c_bus();
#endif

#ifdef POINTING_DEVICE_COMBINED
    pointing_device_set_cpi_on_side(true, POINTING_LEFT_DEVICE_DEFAULT_CPI);   // left  (scroll, low CPI)
    pointing_device_set_cpi_on_side(false, POINTING_RIGHT_DEVICE_DEFAULT_CPI); // right (cursor, higher CPI)
#endif

#ifndef POINTING_DEVICE_COMBINED
#ifndef TCY_DISABLE_CPI_INIT
    pointing_device_set_cpi(POINTING_DEVICE_DEFAULT_CPI);
#endif
#endif

#ifdef OLED_ENABLE
  oled_off();
#ifdef OLED_ENABLE_MINIMAL
  draw_minimal();
#endif
#endif

#ifdef RGB_MATRIX_ENABLE
  rgb_matrix_enable_noeeprom();
#endif
}

#ifdef POINTING_DEVICE_ENABLE
void pointing_device_init_kb(void) {
    pointing_device_init_user();
    /* Force SUCCESS so pointing_device_task() runs PS/2 processing even when
     * the Azoteq hotswap module is absent and its init returns false. */
    pointing_device_set_status(POINTING_DEVICE_STATUS_SUCCESS);
}
#endif

void play_audio(void) {
#ifdef AUDIO_ENABLE
  audio_play_melody(&layer_sound_on, 3, false);  // non-blocking
#endif
}

//
// trackpad
//

#define SCROLL_DIVISOR 8

// Fixed-point accumulators (int16 avoids the soft-float library on AVR)
static int16_t scroll_accumulated_h = 0;
static int16_t scroll_accumulated_v = 0;

report_mouse_t tcy_pointing_device_task(report_mouse_t mouse_report) {
    if (scrolling_mode) {
        scroll_accumulated_h += mouse_report.x;
        scroll_accumulated_v += mouse_report.y;

        int16_t h_out = scroll_accumulated_h / SCROLL_DIVISOR;
        int16_t v_out = scroll_accumulated_v / SCROLL_DIVISOR;

        scroll_accumulated_h -= h_out * SCROLL_DIVISOR;
        scroll_accumulated_v -= v_out * SCROLL_DIVISOR;

        mouse_report.h = (int8_t)h_out;
        mouse_report.v = (int8_t)v_out;
        mouse_report.x = 0;
        mouse_report.y = 0;
    }

    if (lock_mode) {
        mouse_report.h = 0;
        mouse_report.v = 0;
        mouse_report.x = 0;
        mouse_report.y = 0;
    }

    if (esc_drag_active) {
        mouse_report.buttons |= MOUSE_BTN1;
    }

    bool has_moved = mouse_report.x > 0 || mouse_report.y > 0 || mouse_report.v > 0 || mouse_report.h > 0;

    if (has_moved) {
      key_timer = timer_read32();
    }

#ifdef RGB_MATRIX_ENABLE
    if (is_rgb_off && !keep_rgb_off && has_moved) {
      rgb_matrix_enable_noeeprom();
      is_rgb_off = false;
    }
#endif

    return mouse_report;
}

void matrix_scan_user(void) {
    // Boot into DFU if LOWER or RAISE held for 5 seconds
    if (bootloader_active && timer_elapsed(bootloader_timer) >= 5000) {
        bootloader_active = false;
        reset_keyboard();
    }

#if defined(PS2_ENABLE) || defined(PS2_CUSTOM_ENABLE)
    if (timer_elapsed32(key_timer) > 200) {
        disable_tp = false;
    } else {
        disable_tp = true;
    }
#endif

    // On the slave, key_timer is never updated (process_record_user only runs
    // on master). Use last_input_activity_elapsed() which is synced via
    // SPLIT_ACTIVITY_ENABLE so the slave always knows when activity occurred.
#if defined(RGB_MATRIX_ENABLE) || defined(RGBLIGHT_ENABLE) || defined(OLED_ENABLE)
    // 30 seconds
    int max_ms = 30000;

    uint32_t elapsed = is_keyboard_master()
        ? timer_elapsed32(key_timer)
        : last_input_activity_elapsed();
#endif

#ifdef OLED_ENABLE
    if (keep_oled_off) {
        layer_on(_OLED_OFF_SIGNAL);   // signal slave via synced layer state
        oled_off();
        return;
    } else {
        layer_off(_OLED_OFF_SIGNAL);   // signal slave via synced layer state
    }
        
    if (elapsed > max_ms) {
      oled_mode = OLED_OFF;
      layer_on(_OLED_OFF_SIGNAL);   // signal slave via synced layer state
    } else {
       layer_off(_OLED_OFF_SIGNAL);   // signal slave via synced layer state
#ifdef OLED_ENABLE_MINIMAL
      oled_mode = OLED_BONGO_MINI;
#else
      oled_mode = OLED_BONGO;
#endif
    }
#endif

#ifdef RGB_MATRIX_ENABLE
    if (is_keyboard_master()) {
      is_rgb_off = elapsed > max_ms;
      if (is_rgb_off) {
        rgb_matrix_disable_noeeprom();
      } else if (!keep_rgb_off) {
        rgb_matrix_enable_noeeprom();
      }
    }
#endif
}

#ifdef OLED_ENABLE
bool oled_task_user(void) {
    static bool is_screen_on = true;

    if (!is_keyboard_master() && IS_LAYER_ON(_OLED_OFF_SIGNAL)) {
#ifdef OLED_ENABLE_MINIMAL
      _oled_off();
#else
      oled_off(); 
#endif
      return false;
    }

    switch (oled_mode) {
        case OLED_BONGO:
            if (!is_screen_on) {
                oled_on();
                is_screen_on = true;
            }
            draw_bongo();
            break;

#ifdef OLED_ENABLE_MINIMAL
        case OLED_BONGO_MINI:
            if (!is_screen_on) {
                oled_on();
                is_screen_on = true;
            }
            draw_minimal();
            break;
#endif

        case OLED_OFF:
        default:
            if (is_screen_on) {
                oled_clear();

#ifdef OLED_ENABLE_MINIMAL
                _oled_off();
#else
                oled_off(); 
#endif
                is_screen_on = false;
            }

            // Return true to tell QMK to stop trying to render updates
            return true;
    }

    return false;
}
#endif

layer_state_t layer_state_set_user(layer_state_t state) {
#ifndef LAYER_STATE_8BIT
    is_osx = IS_LAYER_ON_STATE(state, _OSX_SIGNAL);
    current_layer = get_highest_layer(state & ~SIGNAL_LAYERS_MASK);
#else
    current_layer = get_highest_layer(state);
#endif

#if defined(PS2_ENABLE) || defined(PS2_CUSTOM_ENABLE)
    switch (current_layer) {
      case _QWERTY:
        ps2_acceleration_setting = PS2_DEFAULT_ACCELERATION_SETTING;
        if (is_osx) {
          ps2_acceleration_setting -= 1;
        }
        break;
      case _ESC:
        if (scrolling_mode) {
          ps2_acceleration_setting = PS2_MIN_ACCELERATION_SETTING;
        }
        break;
      default:
        ps2_acceleration_setting = PS2_DEFAULT_ACCELERATION_SETTING;
        break;
    }
#endif

    return state;
}
