#pragma once

#include "tapdance.h"

#define SIGNAL_LAYERS_MASK (((layer_state_t)1 << _OLED_OFF_SIGNAL) | ((layer_state_t)1 << _OSX_SIGNAL))

uint8_t mcp_click_state;

#if defined(PS2_ENABLE) || defined(PS2_CUSTOM_ENABLE)
uint8_t ps2_acceleration_setting;
bool disable_tp;
#endif

uint8_t current_layer;
bool is_osx;

void play_audio(void);

extern bool lock_mode;
extern uint16_t bootloader_timer;
extern bool bootloader_active;

report_mouse_t tcy_pointing_device_task(report_mouse_t mouse_report);

#if defined(RGB_MATRIX_ENABLE) || defined(RGBLIGHT_ENABLE)
bool keep_rgb_off;
uint8_t COLOR_RED[3];
uint8_t COLOR_GREEN[3];
uint8_t COLOR_BLUE[3];
uint8_t COLOR_PURPLE[3];
uint8_t COLOR_YELLOW[3];
uint8_t COLOR_PINK[3];
uint8_t COLOR_ORANGE[3];
#endif

#ifdef PS2_ENABLE_STM32
uint8_t ps2_buttons_state;
#endif

