#pragma once

#include "quantum.h"
#include "pointing_device.h"

/* ============================================================
 * Settings (Exposed for keymap/menu manipulation)
 * ============================================================ */

extern uint16_t mouse_rotation_angle;

extern uint8_t drag_scroll_speed_setting;
extern uint8_t drag_scroll_speed_values[6];

// Note: Ensure ps2_acceleration_setting is defined somewhere in your code!
extern uint8_t ps2_acceleration_setting; 
extern float acceleration_values[7];

extern uint8_t linear_reduction_setting;
extern float linear_reduction_values[7];

/* ============================================================
 * Fast Math Approximations
 * ============================================================ */

float fast_approximate_square_root(float input_number);
float fast_approximate_power(float base_value, float exponent_value);

/* ============================================================
 * Movement & Rotation Functions
 * ============================================================ */

void scale_mouse_vector(report_mouse_t *mouse_report);
void rotate_mouse_coordinates_optimized(uint16_t angle, report_mouse_t *mouse_report);
void rotate_mouse_coordinates(uint16_t angle, report_mouse_t *mouse_report);

/* ============================================================
 * Main Acceleration Task Hook
 * ============================================================ */

report_mouse_t ps2_acceleration_task(report_mouse_t mouse_report);
