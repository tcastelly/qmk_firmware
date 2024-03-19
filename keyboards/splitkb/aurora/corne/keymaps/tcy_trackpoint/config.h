/*
Copyright 2022 x123 <@x123>
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

#define TAPPING_TERM 175
#define TAPPING_TERM_PER_KEY

// use the power light of liatris controller
// to display the cap lock light
#define LED_CAPS_LOCK_PIN 24
#define SPLIT_ACTIVITY_ENABLE
#define SPLIT_TRANSPORT_MIRROR

#define SERIAL_USART_TX_PIN GP1

// The default streaming mode is flaky and doesn't always work. This setting doesn't seem to affect performance.
#define PS2_MOUSE_USE_REMOTE_MODE

// Serial uses PIO0, change PS2 to PIO1.
#define PS2_PIO_USE_PIO1

#define PS2_MOUSE_INVERT_X
#define PS2_MOUSE_INVERT_Y

#define PS2_DATA_PIN    GP2
#define PS2_CLOCK_PIN   GP3

// Start faster when keyboard resets.
#define PS2_MOUSE_INIT_DELAY 500
