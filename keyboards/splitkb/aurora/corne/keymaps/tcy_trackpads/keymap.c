#include QMK_KEYBOARD_H

#include "../tcy/keymaps.c"
#include "tcy.h"

//Lighting
bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    if (!rgb_matrix_is_enabled()) return false;

    for (uint8_t i = led_min; i < led_max; i++) {
      rgb_matrix_set_color(i, COLOR_ORANGE[0], COLOR_ORANGE[1], COLOR_ORANGE[2]);
    }

    return false;
}

// switch off the power light of the liatris controller
void keyboard_pre_init_user(void) {
  // Set our LED pin as output
  gpio_set_pin_output(24);
  // Turn the LED off
  // (Due to technical reasons, high is off and low is on)
  gpio_write_pin_high(24);
}

report_mouse_t pointing_device_task_combined_user(report_mouse_t left_report, report_mouse_t right_report) {
    // Left trackpad → scroll only
    left_report.h = left_report.x;
    left_report.v = left_report.y;
    left_report.x = 0;
    left_report.y = 0;

    // Right trackpad → cursor only (no change needed)
    return pointing_device_combine_reports(left_report, tcy_pointing_device_task(right_report));
}
