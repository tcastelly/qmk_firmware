#pragma once

#define TAP_CODE_DELAY 10

/* communication between sides */
#define USE_SERIAL
#define SERIAL_USE_MULTI_TRANSACTION

/* track point */
#define PS1_MOUSE_SCROLL_BTN_MASK (1<<PS2_MOUSE_BTN_MIDDLE) /* Default */

#define PS2_MOUSE_INVERT_X
#define PS2_MOUSE_INVERT_Y

// The default streaming mode is flaky and doesn't always work. This setting doesn't seem to affect performance.
#define PS2_MOUSE_USE_REMOTE_MODE

// Serial uses PIO0, change PS2 to PIO1.
#define PS2_PIO_USE_PIO1

// Start faster when keyboard resets.
#define PS2_MOUSE_INIT_DELAY 500
