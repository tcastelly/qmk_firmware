/*
This is the c configuration file for the keymap

Copyright 2012 Jun Wako <wakojun@gmail.com>
Copyright 2015 Jack Humbert

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "users/tcy/rp2040/config.h"

#define AZOTEQ_IQS5XX_TPS65
#define AZOTEQ_IQS5XX_ROTATION_270

#define OLED_ENABLE_MINIMAL

// Does not work
// #define OLED_DISPLAY_WIDTH 96
// #define OLED_DISPLAY_HEIGHT 16

// Use this instead
#define OLED_WIDTH 96
#define OLED_HEIGHT 16

