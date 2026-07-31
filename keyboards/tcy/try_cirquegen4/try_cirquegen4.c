/* =============================================================================
 * try_cirquegen4 — bare RP2040 (Sea-Picro) + Cirque Gen4 trackpad test board.
 *
 * Board-level logic is the two routes into the RP2040's BOOTSEL bootloader
 * (the ROM mass-storage mode that mounts as RPI-RP2 and accepts a .uf2 — the
 * RP2040 has no USB DFU class, so there is nothing to 'DFU' into):
 *
 *   1. hold GP25 low (short it to GND), debounced ~50 ms
 *   2. no pad activity for BOOTSEL_IDLE_MS after boot
 *
 * Route 2 exists because route 1 does not currently work on this board, and
 * there is no physical reset button.
 * ========================================================================== */

#include "quantum.h"
#include "print.h"

#ifndef BOOTSEL_TRIGGER_PIN
#    define BOOTSEL_TRIGGER_PIN GP25
#endif

/* ── Idle BOOTSEL trigger ───────────────────────────────────────────────────
 * No pointer activity for BOOTSEL_IDLE_MS after boot -> jump to the bootloader.
 * Exists because the GP25 jumper does not work on this board, and without a
 * reset button there has to be SOME way in.
 *
 * BOOTSEL_IDLE_ONESHOT is what makes this usable rather than hostile: the first
 * touch disarms it for the rest of the session. Otherwise the board would drop
 * into BOOTSEL five seconds after every boot, and again every time you paused —
 * you could never actually use the pad. Set it to 0 for a literal
 * "always idle-triggered" behaviour if you really want that.
 *
 * So: plug in and DO NOT touch the pad -> BOOTSEL after 5 s. Touch it within
 * those 5 s -> normal operation until the next replug.
 * ────────────────────────────────────────────────────────────────────────── */
#ifndef BOOTSEL_IDLE_MS
#    define BOOTSEL_IDLE_MS 5000
#endif
#ifndef BOOTSEL_IDLE_ONESHOT
#    define BOOTSEL_IDLE_ONESHOT 1
#endif

static uint32_t bootsel_last_activity = 0;
static bool     bootsel_idle_disarmed = false;

/* Any pointer activity counts as "in use". Runs for every pointing-device
 * cycle, before the report reaches the host. */
report_mouse_t pointing_device_task_kb(report_mouse_t mouse_report) {
    if (mouse_report.x || mouse_report.y || mouse_report.v || mouse_report.h || mouse_report.buttons) {
        bootsel_last_activity = timer_read32();
#if BOOTSEL_IDLE_ONESHOT
        if (!bootsel_idle_disarmed) {
            bootsel_idle_disarmed = true;
            uprintf("bootsel: activity seen, idle trigger disarmed for this session\n");
        }
#endif
    }
    return pointing_device_task_user(mouse_report);
}

void keyboard_post_init_kb(void) {
    gpio_set_pin_input_high(BOOTSEL_TRIGGER_PIN);   /* idle high; grounding = enter BOOTSEL */

    /* Confirms this file is actually compiled in AND that the pin idles high.
     * If this line never appears, the board .c is not being built; if it says
     * state=0 with nothing touching the pin, the pull-up is not taking and the
     * trigger would fire immediately rather than never. */
    uprintf("bootsel: trigger pin armed, idle state=%d\n", gpio_read_pin(BOOTSEL_TRIGGER_PIN) ? 1 : 0);
    uprintf("bootsel: idle trigger armed - no pad activity for %d ms = bootloader\n", BOOTSEL_IDLE_MS);

    bootsel_last_activity = timer_read32();
    keyboard_post_init_user();
}

void housekeeping_task_kb(void) {
    static uint16_t low_since = 0;
    static bool     armed     = false;
    static bool     last      = true;

    bool level = gpio_read_pin(BOOTSEL_TRIGGER_PIN) ? true : false;

    /* Log every transition. Grounding the pin must show state=0 here; if it
     * does not, the short is not reaching the MCU (wrong pad, pin not broken
     * out) and no amount of firmware will help. */
    if (level != last) {
        last = level;
        uprintf("bootsel: pin -> %d\n", level ? 1 : 0);
    }

    if (!level) {                                 /* pin pulled low = grounded */
        if (!armed) {
            armed     = true;
            low_since = timer_read();
        } else if (timer_elapsed(low_since) > 50) {
            uprintf("bootsel: jumping to bootloader\n");
            wait_ms(10);                          /* let the console line flush */
            bootloader_jump();                    /* → RP2040 BOOTSEL (RPI-RP2) */
        }
    } else {
        armed = false;
    }

    /* Idle trigger — the working route into BOOTSEL while the GP25 jumper does not. */
    if (!bootsel_idle_disarmed && timer_elapsed32(bootsel_last_activity) > BOOTSEL_IDLE_MS) {
        uprintf("bootsel: idle %d ms, jumping to bootloader\n", BOOTSEL_IDLE_MS);
        wait_ms(10); /* let the console line flush */
        bootloader_jump();
    }

    housekeeping_task_user();
}
