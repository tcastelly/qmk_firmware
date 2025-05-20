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

//#define USE_MATRIX_I2C

/* Select hand configuration */

#define OLED_TIMEOUT 30000

// defines for making the Trackpoint more fluid
#define PS2_MOUSE_USE_REMOTE_MODE

#define PS2_MOUSE_SCROLL_BTN_SEND 500 /* Default is 300 */

#undef PS2_MOUSE_SCROLL_BTN_MASK
#define PS2_MOUSE_SCROLL_BTN_MASK (1<<PS2_MOUSE_BTN_MIDDLE) /* Default */
#define PS2_MOUSE_BTN_LEFT      0
#define PS2_MOUSE_BTN_RIGHT     1
#define PS2_MOUSE_BTN_MIDDLE    2

#define PS2_MOUSE_INIT_DELAY 1000 /* Default */



#ifdef PS2_USE_USART
/* synchronous, odd parity, 1-bit stop, 8-bit data, sample at falling edge */
/* set DDR of CLOCK as input to be slave */
#define PS2_USART_INIT() do {   \
	PS2_CLOCK_DDR &= ~(1<<PS2_CLOCK_BIT);   \
	PS2_DATA_DDR &= ~(1<<PS2_DATA_BIT);     \
	UCSR1C = ((1 << UMSEL10) |  \
			(3 << UPM10)   |  \
			(0 << USBS1)   |  \
			(3 << UCSZ10)  |  \
			(0 << UCPOL1));   \
	UCSR1A = 0;                 \
	UBRR1H = 0;                 \
	UBRR1L = 0;                 \
} while (0)
#define PS2_USART_RX_INT_ON() do {  \
	UCSR1B = ((1 << RXCIE1) |       \
			(1 << RXEN1));        \
} while (0)
#define PS2_USART_RX_POLL_ON() do { \
	UCSR1B = (1 << RXEN1);          \
} while (0)
#define PS2_USART_OFF() do {    \
	UCSR1C = 0;                 \
	UCSR1B &= ~((1 << RXEN1) |  \
			(1 << TXEN1));  \
} while (0)
#define PS2_USART_RX_READY      (UCSR1A & (1<<RXC1))
#define PS2_USART_RX_DATA       UDR1
#define PS2_USART_ERROR         (UCSR1A & ((1<<FE1) | (1<<DOR1) | (1<<UPE1)))
#define PS2_USART_RX_VECT       USART1_RX_vect
#endif
