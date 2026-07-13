/* Copyright 2026 Cipulot
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "display.h"
#include "splash_cipulot.qgf.h"
#include "splash_kenban_de_go.qgf.h"
#include "densha.qgf.h"
#include <stdio.h>
#include <math.h>

painter_device_t              kenban_de_go_display;
static deferred_token         display_task_token;
static painter_image_handle_t splash_image;
// static painter_image_handle_t densha_animation;

uint32_t display_task_callback(uint32_t trigger_time, void *cb_arg) {
    display_task_kb();
    return 100;
}

void display_init_kb(void) {
    if (!display_init_user()) {
        return;
    }

    kenban_de_go_display = qp_sh1106_make_i2c_device(DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_ADDRESS);
    if (!qp_init(kenban_de_go_display, QP_ROTATION_0)) {
        kenban_de_go_display = NULL;
    }
    qp_clear(kenban_de_go_display);

    if (is_keyboard_left()) {
        splash_image = qp_load_image_mem(gfx_splash_kenban_de_go);
    } else {
        splash_image = qp_load_image_mem(gfx_splash_cipulot);
    }
    if (splash_image != NULL) {
        qp_drawimage(kenban_de_go_display, (DISPLAY_WIDTH - splash_image->width), (DISPLAY_HEIGHT - splash_image->height), splash_image);
        qp_flush(kenban_de_go_display);
        qp_close_image(splash_image);
        if (kenban_de_go_display != NULL) {
            display_task_token = defer_exec(2000, display_task_callback, NULL);
        }
    }
}

__attribute__((weak)) bool display_init_user(void) {
    return true;
}

void display_task_kb(void) {
    if (!display_task_user()) {
        return;
    }
    // static bool first_draw = true;
    //  static uint32_t last_wpm               = UINT32_MAX;
    //  static uint32_t last_scan_rate         = 0;
    //  static uint32_t last_key_pressed_count = UINT32_MAX;

    // if (first_draw) {
    //    qp_clear(kenban_de_go_display);
    //    densha_animation = qp_load_image_mem(gfx_densha);
    //   qp_animate(kenban_de_go_display, (128 - densha_animation->width), (64 - densha_animation->height), densha_animation);

    // first_draw = false;
    // }

    // char buffer[64] = {0};
    /*
    static bool     first_draw             = true;
    static uint32_t last_timer             = 0;
    static uint32_t last_wpm               = UINT32_MAX;
    static uint32_t last_scan_rate         = 0;
    static uint32_t last_key_pressed_count = UINT32_MAX;
    static uint8_t  last_hue               = 0;
    static uint8_t  last_sat               = 0;
    static uint8_t  last_val               = 0;
    bool            rgb_redraw             = false;

    if (first_draw) {
        qp_rect(reverb_display, 0, 0, 239, 239, 0, 0, 0, true);
        first_draw = false;
    }

    char buffer[64] = {0};

    if (last_hue != rgb_matrix_get_hue() || last_sat != rgb_matrix_get_sat() || last_val != rgb_matrix_get_val()) {
        last_hue = rgb_matrix_get_hue();
        last_sat = rgb_matrix_get_sat();
        last_val = rgb_matrix_get_val();
        qp_drawimage_recolor(reverb_surface, 60, 40, reverb_logo, rgb_matrix_get_hue(), rgb_matrix_get_sat(), MAX(rgb_matrix_get_val(), 128), 0, 0, 0);
        rgb_redraw = true;
    }

    if (rgb_redraw || last_scan_rate != get_matrix_scan_rate()) {
        snprintf(buffer, sizeof(buffer), "Scan Rate %ld", get_matrix_scan_rate());
        int16_t width = qp_textwidth(roboto_font, buffer);
        qp_line(reverb_surface, 17, 101, 220, 101, rgb_matrix_get_hue(), rgb_matrix_get_sat(), MAX(rgb_matrix_get_val(), 128));
        qp_rect(reverb_surface, 16, 125 - roboto_font->line_height, 221, 125, 0, 0, 0, true);
        qp_drawtext_recolor(reverb_surface, (120 - (width / 2)), (125 - roboto_font->line_height), roboto_font, buffer, rgb_matrix_get_hue(), rgb_matrix_get_sat(), MAX(rgb_matrix_get_val(), 128), 0, 0, 0);
        qp_line(reverb_surface, 16, 126, 221, 126, rgb_matrix_get_hue(), rgb_matrix_get_sat(), MAX(rgb_matrix_get_val(), 128));
        last_scan_rate = get_matrix_scan_rate();
    }
    if (rgb_redraw || last_key_pressed_count != key_pressed_count) {
        snprintf(buffer, sizeof(buffer), "Keys Pressed");
        int16_t width = qp_textwidth(roboto_font, buffer);
        qp_rect(reverb_surface, 30, 150 - roboto_font->line_height, 209, 150, 0, 0, 0, true);
        qp_drawtext_recolor(reverb_surface, (120 - (width / 2)), (150 - roboto_font->line_height), roboto_font, buffer, rgb_matrix_get_hue(), rgb_matrix_get_sat(), MAX(rgb_matrix_get_val(), 128), 0, 0, 0);
        snprintf(buffer, sizeof(buffer), "%ld", key_pressed_count);
        width = qp_textwidth(roboto_font, buffer);
        qp_rect(reverb_surface, 30, 172 - roboto_font->line_height, 173, 172, 0, 0, 0, true);
        qp_drawtext_recolor(reverb_surface, (120 - (width / 2)), (172 - roboto_font->line_height), roboto_font, buffer, rgb_matrix_get_hue(), rgb_matrix_get_sat(), MAX(rgb_matrix_get_val(), 128), 0, 0, 0);
        qp_line(reverb_surface, 30, 173, 209, 173, rgb_matrix_get_hue(), rgb_matrix_get_sat(), MAX(rgb_matrix_get_val(), 128));
        last_key_pressed_count = key_pressed_count;
    }
    if (rgb_redraw || last_wpm != get_current_wpm()) {
        snprintf(buffer, sizeof(buffer), "WPM %d", get_current_wpm());
        int16_t width = qp_textwidth(roboto_font, buffer);
        qp_rect(reverb_surface, 56, 200 - roboto_font->line_height, 184, 200, 0, 0, 0, true);
        qp_drawtext_recolor(reverb_surface, (120 - (width / 2)), (200 - roboto_font->line_height), roboto_font, buffer, rgb_matrix_get_hue(), rgb_matrix_get_sat(), MAX(rgb_matrix_get_val(), 128), 0, 0, 0);
        qp_line(reverb_surface, 56, 201, 184, 201, rgb_matrix_get_hue(), rgb_matrix_get_sat(), MAX(rgb_matrix_get_val(), 128));
        last_wpm = get_current_wpm();
    }

    if (rgb_redraw || timer_elapsed(last_timer) >= 1000) {
        static uint8_t seconds = 0;
        static uint8_t x       = 0;
        static uint8_t y       = 0;
        if (x && y) {
            qp_circle(reverb_surface, x, y, 4, 0, 0, 0, true);
        }
        double radians = 0.10471975511966 * seconds;
        x              = (110 * cos(radians)) + 119;
        y              = (110 * sin(radians)) + 119;
        qp_circle(reverb_surface, x, y, 4, rgb_matrix_get_hue(), rgb_matrix_get_sat(), MAX(rgb_matrix_get_val(), 128), true);
        qp_circle(reverb_surface, 119, 119, 105, rgb_matrix_get_hue(), rgb_matrix_get_sat(), MAX(rgb_matrix_get_val(), 128), false);
        if (seconds == 59) {
            seconds = 0;
        } else if (timer_elapsed(last_timer) >= 1000) {
            last_timer = timer_read32();
            seconds++;
        }
    }

    qp_surface_draw(reverb_surface, reverb_display, 0, 0, rgb_redraw);
    */
}

__attribute__((weak)) bool display_task_user(void) {
    return true;
}
