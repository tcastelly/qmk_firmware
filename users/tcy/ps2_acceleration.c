#include "quantum.h"
#include "pointing_device.h"
#include <math.h>
#include "tcy.h"
#include <string.h>

/* ============================================================
 * Scroll divisors — override in config.h if needed
 *   #define PS2_SCROLL_DIVISOR_H 8
 *   #define PS2_SCROLL_DIVISOR_V 8
 * ============================================================ */
#ifndef PS2_SCROLL_DIVISOR_H
#define PS2_SCROLL_DIVISOR_H 8
#endif
#ifndef PS2_SCROLL_DIVISOR_V
#define PS2_SCROLL_DIVISOR_V 8
#endif

/* ============================================================
 * External state
 * scrolling_mode: set by keymap when _ESC layer is active
 * ============================================================ */
extern bool scrolling_mode;


/* ============================================================
 * Settings
 * All adjustable at runtime from keymap or menu.
 * ============================================================ */

#ifndef DEFAULT_ACCELERATION_SETTING
#define DEFAULT_ACCELERATION_SETTING 2
#endif

bool    disable_tp                      = false;

// default value 250
uint16_t mouse_rotation_angle           = 250;

uint8_t drag_scroll_speed_setting       = 2;
uint8_t drag_scroll_speed_values[6]     = {8, 7, 6, 5, 4, 3};

float   acceleration_values[7]          = {0.6f, 0.8f, 1.0f, 1.2f, 1.4f, 1.6f, 1.8f};

uint8_t linear_reduction_setting        = 3;
float   linear_reduction_values[7]      = {80.0f, 2.2f, 2.0f, 1.8f, 1.6f, 1.4f, 0.4f};

/* ============================================================
 * Fast math approximations
 * ============================================================ */

inline float fast_approximate_square_root(float input_number) {
    uint32_t bit_representation;
    float half_input = input_number * 0.5F;
    float approximate_result = input_number;
    const float constant_for_approximation = 1.5F;

    /* Safe float-to-int reinterpretation */
    memcpy(&bit_representation, &approximate_result, sizeof(bit_representation));
    bit_representation = 0x5f3759df - (bit_representation >> 1);
    /* Safe int-to-float reinterpretation */
    memcpy(&approximate_result, &bit_representation, sizeof(approximate_result));

    approximate_result = approximate_result * (constant_for_approximation - (half_input * approximate_result * approximate_result));
    return 1.0f / approximate_result;
}

inline float fast_approximate_power(float base_value, float exponent_value) {
    union {
        float float_value;
        int   int_value;
    } u = { base_value };
    u.int_value = (int)(exponent_value * (u.int_value - 1064866805) + 1064866805);
    return u.float_value;
}


/* ============================================================
 * Movement scaling
 * Converts X/Y into a vector, applies acceleration curve and
 * linear reduction, then decomposes back to X/Y.
 * ============================================================ */

void scale_mouse_vector(report_mouse_t *mouse_report) {
    float hypotenuse        = sqrt((mouse_report->x * mouse_report->x) + (mouse_report->y * mouse_report->y));
    float scaled_hypotenuse = pow(hypotenuse, acceleration_values[ps2_acceleration_setting]) / linear_reduction_values[linear_reduction_setting];
    float angle             = atan2(mouse_report->y, mouse_report->x);
    mouse_report->x        += (int8_t)(scaled_hypotenuse * cos(angle));
    mouse_report->y        += (int8_t)(scaled_hypotenuse * sin(angle));
}


/* ============================================================
 * Rotation
 * ============================================================ */

void rotate_mouse_coordinates_optimized(uint16_t angle, report_mouse_t *mouse_report) {
    if (angle == 0) return;

    static float last_angle          = 0;
    static float precomputed_cosine  = 1.0f;
    static float precomputed_sine    = 0.0f;

    if (last_angle != angle) {
        static const float deg2rad = 0.017453f;
        float radians              = angle * deg2rad;
        precomputed_cosine         = cos(radians);
        precomputed_sine           = sin(radians);
        last_angle                 = angle;
    }

    int cached_x    = mouse_report->x;
    int cached_y    = mouse_report->y;
    mouse_report->x = (int8_t)round(precomputed_cosine * cached_x - precomputed_sine * cached_y);
    mouse_report->y = (int8_t)round(precomputed_sine   * cached_x + precomputed_cosine * cached_y);
}

void rotate_mouse_coordinates(uint16_t angle, report_mouse_t *mouse_report) {
    if (angle == 0) return;

    static const float deg2rad = 0.017453f;
    float radians  = angle * deg2rad;
    int current_x  = mouse_report->x;
    int current_y  = mouse_report->y;
    mouse_report->x = (int8_t)round(cos(radians) * current_x - sin(radians) * current_y);
    mouse_report->y = (int8_t)round(sin(radians) * current_x + cos(radians) * current_y);
}


/* ============================================================
 * Acceleration task
 * Call explicitly from pointing_device_task_user() in keymap.c:
 *
 *   report_mouse_t pointing_device_task_user(report_mouse_t r) {
 *       r = ps2_acceleration_task(r);
 *       // button remap, custom logic here
 *       return r;
 *   }
 *
 * Compatible with both native QMK PS/2 driver and ps2_stm32.c.
 * ============================================================ */

report_mouse_t ps2_acceleration_task(report_mouse_t mouse_report) {
  /* Drag scroll rate limiting: skips reports to slow down scroll speed.
   * Counter wraps at 8! (40320) for clean modulo divisibility.        */
  static uint16_t drag_scroll_counter = 0;
  drag_scroll_counter = (drag_scroll_counter == 40320) ? 0 : drag_scroll_counter + 1;

  if ((mouse_report.v != 0 || mouse_report.h != 0) &&
      drag_scroll_counter % drag_scroll_speed_values[drag_scroll_speed_setting] != 0) {
    mouse_report.v = 0;
    mouse_report.h = 0;
  }

  scale_mouse_vector(&mouse_report);
  rotate_mouse_coordinates(mouse_rotation_angle, &mouse_report);

  return mouse_report;
}
