#include QMK_KEYBOARD_H

#include "../tcy/keymaps.c"
#include "tcy.h"

// --- Fire effect --------------------------------------------------------
#define FIRE_UPDATE_MS 50

static uint16_t fire_timer  = 0;
static bool     fire_inited = false;
static uint8_t  fire_heat[RGB_MATRIX_LED_COUNT];
static uint16_t fire_lfsr   = 0x1337;

static uint8_t fire_rand8(void) {
    fire_lfsr ^= fire_lfsr << 7;
    fire_lfsr ^= fire_lfsr >> 9;
    fire_lfsr ^= fire_lfsr << 8;
    return (uint8_t)(fire_lfsr ^ (fire_lfsr >> 8));
}

// heat 0=deep red, 128=orange, 255=bright yellow-orange
static void heat_to_rgb(uint8_t heat, uint8_t *r, uint8_t *g, uint8_t *b) {
    if (heat < 128) {
        *r = 128 + heat;
        *g = heat / 2;
        *b = 0;
    } else {
        *r = 255;
        *g = 64 + (heat - 128);
        *b = (heat - 128) / 8;
    }
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    if (keep_rgb_off) return false;

    if (!fire_inited) {
        for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
            fire_heat[i] = 80 + fire_rand8() % 170;
        }
        fire_inited = true;
        fire_timer  = timer_read();
    }

    if (timer_elapsed(fire_timer) >= FIRE_UPDATE_MS) {
        fire_timer = timer_read();
        for (uint8_t i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
            int16_t h = (int16_t)fire_heat[i] + (int16_t)(fire_rand8() % 41) - 20;
            if (h < 60)  h = 60  + (fire_rand8() & 0x0F);
            if (h > 250) h = 250 - (fire_rand8() & 0x0F);
            fire_heat[i] = (uint8_t)h;
        }
    }

    uint8_t val = rgb_matrix_get_val();
    for (uint8_t i = led_min; i < led_max; i++) {
        uint8_t r, g, b;
        heat_to_rgb(fire_heat[i], &r, &g, &b);
        r = (uint8_t)((uint16_t)r * val / 255);
        g = (uint8_t)((uint16_t)g * val / 255);
        b = (uint8_t)((uint16_t)b * val / 255);
        rgb_matrix_set_color(i, r, g, b);
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
