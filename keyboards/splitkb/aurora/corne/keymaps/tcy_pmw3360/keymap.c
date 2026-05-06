#include QMK_KEYBOARD_H

#include "../tcy/keymaps.c"
#include "tcy.h"

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    return tcy_pointing_device_task(mouse_report);
}

//Lighting
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
  if (keep_rgb_off) return false;

  const uint8_t *color;

  switch (get_highest_layer(layer_state)) {
    case _ESC:
    case _ESC_OSX:
      color = COLOR_GREEN;
      break;
    case _QWERTY_GAMING:
      color = COLOR_YELLOW;
      break;
    case _ADJUST:
      color = COLOR_BLUE;
      break;
    case _QWERTY:
      color = COLOR_PURPLE;
      break;
    case _QWERTY_OSX:
      color = COLOR_PINK;
      break;
    default:
      color = COLOR_RED;
      break;
  }

  for (uint8_t i = led_min; i < led_max; i++) {
    rgb_matrix_set_color(i, color[0], color[1], color[2]);
  }
  
  return false;
}
