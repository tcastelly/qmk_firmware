#include "stdio.h"
#include "tcy.h"

// Bongo pages 0-1: cat head arc at page 0 cols 63-86, face/eyes at page 1 cols 51-82.
// Content naturally sits at the bottom of the page bytes (high bit = bottom row), so
// shifting up by 2 pixels centres it on the 16-px display.
// Text layout (96 px wide, 6 px/char = 16 char-cols):
//   row 0: cat arc left (cols 0-35) | layer name (cursor 6, col 36+) | cat arc gap | PS2 (cursor 13, cols 78-95)
//   row 1: cat face/eyes visible (raw frame) | caps [] (cursor 14, cols 84-95)

#ifndef OLED_HEIGHT
#    define OLED_HEIGHT 16
#endif

#define MINI_PAGES    (OLED_HEIGHT / 8)
#define MINI_BUF_SIZE (OLED_DISPLAY_WIDTH * MINI_PAGES)

#define SHIFT_UP 2   // pixels to shift image toward display top (bit 0 = top row)

static void _oled_off(void) {
    oled_clear();
    oled_off();
}

static void draw_minimal(void) {
    eval_anim_state();

    const char *frame;
    switch (anim_state) {
        case Idle:
            frame = idle_draw[abs((IDLE_FRAMES - 1) - current_idle_frame)];
            if (timer_elapsed32(anim_timer) > ANIM_FRAME_DURATION) {
                current_idle_frame = (current_idle_frame + 1) % IDLE_FRAMES;
                anim_timer         = timer_read32();
            }
            break;
        case Prep:
            frame = prep[0];
            break;
        case Tap:
            frame = tap_dflt[abs((TAP_FRAMES - 1) - current_tap_frame)];
            current_tap_frame = (current_tap_frame + 1) % TAP_FRAMES;
            break;
        default:
            frame = idle_draw[0];
            break;
    }

    // Build display buffer: pages 0-1 of bongo frame, shifted up by SHIFT_UP pixels.
    // Each page byte: bit 0 = top display row, bit 7 = bottom. Shifting up → >> SHIFT_UP.
    // Carry: top SHIFT_UP rows of page 1 spill into the bottom of page 0.
    uint8_t buf[MINI_BUF_SIZE];
    const uint8_t carry_mask = (1 << SHIFT_UP) - 1;
    for (uint16_t col = 0; col < OLED_DISPLAY_WIDTH; col++) {
        uint8_t p0 = pgm_read_byte(&frame[col]);
        uint8_t p1 = pgm_read_byte(&frame[OLED_DISPLAY_WIDTH + col]);
        buf[col]                      = (p0 >> SHIFT_UP) | ((p1 & carry_mask) << (8 - SHIFT_UP));
        buf[OLED_DISPLAY_WIDTH + col] = p1 >> SHIFT_UP;
    }

    oled_set_cursor(0, 0);
    oled_write_raw((const char *)buf, MINI_BUF_SIZE);

    // ── Row 0: layer name at cursor 6, PS2 right-aligned to last 3 cols ────
    // No padding: raw name write avoids erasing cat arc pixels in the gap.
    // oled_max_chars() drives PS2/[] to the true right edge of the buffer.
    const char *layer_name;
#ifdef LAYER_STATE_8BIT
    switch (get_highest_layer(layer_state)) {
#else
    switch (get_highest_layer(layer_state & ~SIGNAL_LAYERS_MASK)) {
#endif
        case _QWERTY:       layer_name = is_osx ? "Q-OSX" : "Q";       break;
        case _ESC:          layer_name = is_osx ? "ESC-OSX" : "ESC";   break;
        case _LOWER:        layer_name = "Lower";                        break;
        case _RAISE:        layer_name = "Raise";                        break;
        case _ADJUST:       layer_name = "Adjust";                       break;
        case _ACCENTS_RALT: layer_name = "Accents";                      break;
        default:            layer_name = "";                              break;
    }
    oled_set_cursor(6, 0);
    oled_write(layer_name, false);

#if defined(PS2_ENABLE) || defined(PS2_CUSTOM_ENABLE)
    char ps2_buf[4];
    snprintf(ps2_buf, sizeof(ps2_buf), "%3d", ps2_acceleration_setting);
    oled_set_cursor(oled_max_chars() - 3, 0);
    oled_write(ps2_buf, false);
#endif

    // ── Row 1: caps lock right-aligned, no leading spaces (preserves cat) ─
    oled_set_cursor(oled_max_chars() - 2, 1);
    oled_write_P(is_kc_caps ? PSTR("[]") : PSTR("  "), false);
}
