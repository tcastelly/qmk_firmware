
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
  switch (current_layer) {
      case _QWERTY:
          oled_write_P(PSTR("Q              "), false);
          break;
      case _QWERTY_OSX:
          oled_write_P(PSTR("Q-OSX          "), false);
          break;
      case _ESC:
          oled_write_P(PSTR("ESC            "), false);
          break;
      case _ESC_OSX:
          oled_write_P(PSTR("ESC-OSX        "), false);
          break;
      case _LOWER:
          oled_write_P(PSTR("Lower          "), false);
          break;
      case _RAISE:
          oled_write_P(PSTR("Raise          "), false);
          break;
      case _ADJUST:
          oled_write_P(PSTR("Adjust         "), false);
          break;
      case _ACCENTS_RALT:
          oled_write_P(PSTR("Accents        "), false);
          break;
      default:
          oled_write_P(PSTR( "               "), false);
          break;
  }

  oled_set_cursor(6, 1);       // Set cursor to last available point
  if (is_kc_caps) {
    oled_write_P(PSTR( "             []"), false);
  } else {
    oled_write_P(PSTR( "               "), false);
  }
}

layer_state_t layer_state_set_user(layer_state_t state) {
    current_layer = get_highest_layer(state);

    return state;
}

