
#include "stdio.h"

bool is_kc_caps  = false;

static uint8_t current_layer = 0;

static void _oled_off(void)
{
  oled_clear();
  oled_set_cursor(6, 0);       // Set cursor to origin
  oled_write_P(PSTR("               "), false);
  oled_set_cursor(6, 1);       // Set cursor to origin
  oled_write_P(PSTR("               "), false);
  oled_off();
}

static void draw_minimal(void)
{
  oled_clear();
  oled_set_cursor(6, 0);       // Set cursor to origin
  oled_write_P(PSTR("[             ]"), false);
  oled_set_cursor(6, 1);       // Set cursor to origin
  oled_write_P(PSTR("[             ]"), false);

  oled_set_cursor(6, 0);       // Set cursor to origin
                               //
  char buf[32];  // temporary buffer for formatted string
  switch (current_layer) {
      case _QWERTY:
          snprintf(buf, sizeof(buf), "Q             %d", acceleration_setting);
          break;
      case _QWERTY_OSX:
          snprintf(buf, sizeof(buf), "Q-OSX         %d", acceleration_setting);
          break;
      case _ESC:
          snprintf(buf, sizeof(buf), "ESC           %d", acceleration_setting);
          break;
      case _ESC_OSX:
          snprintf(buf, sizeof(buf), "ESC-OSX       %d", acceleration_setting);
          break;
      case _LOWER:
          snprintf(buf, sizeof(buf), "Lower          ");
          break;
      case _RAISE:
          snprintf(buf, sizeof(buf), "Raise          ");
          break;
      case _ADJUST:
          snprintf(buf, sizeof(buf), "Adjust         ");
          break;
      case _ACCENTS_RALT:
          snprintf(buf, sizeof(buf), "Accents        ");
          break;
      default:
          snprintf(buf, sizeof(buf), "               ");
          break;

  }

  oled_write(buf, false);

  oled_set_cursor(6, 1);       // Set cursor to last available point
  if (is_kc_caps) {
    oled_write_P(PSTR( "             []"), false);
  } else {
    oled_write_P(PSTR( "               "), false);
  }
}

layer_state_t layer_state_set_user(layer_state_t state) {
    current_layer = get_highest_layer(state);

    switch (current_layer) {
      case _QWERTY_OSX:
        // OSX needs less speed
        acceleration_setting = DEFAULT_ACCELERATION_SETTING;
        acceleration_setting -= 1;
        break;
      case _ESC:
      case _ESC_OSX:
        if (scrolling_mode) {
          acceleration_setting = MIN_ACCELERATION_SETTING;
        }
        break;
      default:
        acceleration_setting = DEFAULT_ACCELERATION_SETTING;
        break;
    }

    return state;
}

