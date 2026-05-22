#include "stdio.h"
#include "tcy.h"

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
  char buf[32];                // temporary buffer for formatted string

  const char *osx_sufix = "-OSX";
  switch (get_highest_layer(layer_state)) {
      case _QWERTY:
          snprintf(buf, sizeof(buf), "Q%s             %d",  is_osx ? osx_sufix : "", ps2_acceleration_setting);
          break;
      case _ESC:
          snprintf(buf, sizeof(buf), "ESC%s           %d", is_osx ? osx_sufix : "", ps2_acceleration_setting);
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

