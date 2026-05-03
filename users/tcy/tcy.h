#pragma once

#include "tapdance.h"

uint8_t mcp_click_state;

uint8_t ps2_acceleration_setting;

uint8_t current_layer;

void play_audio(void);

report_mouse_t tcy_pointing_device_task(report_mouse_t mouse_report);

uint8_t COLOR_RED[3];
uint8_t COLOR_GREEN[3];
uint8_t COLOR_BLUE[3];
uint8_t COLOR_PURPLE[3];
uint8_t COLOR_YELLOW[3];
uint8_t COLOR_PINK[3];
uint8_t COLOR_ORANGE[3];

#ifdef PS2_ENABLE_STM32
uint8_t ps2_buttons_state;
#endif

