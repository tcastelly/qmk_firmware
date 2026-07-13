// Copyright 2022 sekigon-gonnoc
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Board ported to modern QMK: no RGB / OLED / SPI. EC actuation-point
// calibration is preserved (the part that matters):
//   * Web Serial Plotter  -> send 'e' over the USB CDC (VIRTSER) port and the
//     per-switch AD values stream out (ecsm_dprint_matrix via virtser).
//   * ECSKB Configurator  -> reads/writes LOW/HIGH thresholds over VIA raw-HID
//     (0xec), persisted in the kb eeprom datablock.

#include QMK_KEYBOARD_H
#include "ec_switch_matrix.h"
#include "eeconfig.h"
#include "print.h"
#include "analog.h"

#ifdef VIA_ENABLE
#    include "via.h"
#    include "raw_hid.h"
#endif

#ifdef VIRTSER_ENABLE
#    include "virtser.h"
#endif

#ifndef LOW_THRESHOLD
#    define LOW_THRESHOLD 200
#endif
#ifndef HIGH_THRESHOLD
#    define HIGH_THRESHOLD 300
#endif

typedef struct {
    uint16_t low;
    uint16_t high;
} ec_thresholds_t;

static void apply_thresholds(uint16_t low, uint16_t high) {
    ecsm_config_t c;
    ecsm_get_config(&c);   // preserve per-key array pointers set at matrix init
    c.low_threshold  = low;
    c.high_threshold = high;
    ecsm_init(&c);
}

// Called by QMK when the eeprom is reset — seed defaults.
void eeconfig_init_kb(void) {
    ec_thresholds_t t = {LOW_THRESHOLD, HIGH_THRESHOLD};
    eeconfig_update_kb_datablock(&t, 0, sizeof(t));
    eeconfig_init_user();
}

// --- Web Serial Plotter: route debug print to the CDC serial, 'e' toggles ---
static bool ec_plot = false;

#ifdef VIRTSER_ENABLE
#ifndef CONSOLE_ENABLE
// When CONSOLE_ENABLE is off, dprintf() is a no-op that never calls sendchar,
// so this override has no effect on dprintf. Keep it for any direct sendchar
// callers, but the plotter relies on dprintf — enable CONSOLE_ENABLE instead.
int8_t sendchar(uint8_t c) {
    virtser_send(c);
    return 0;
}
#endif  // !CONSOLE_ENABLE

void virtser_recv(uint8_t c) {
    if (c == 'e') {
        ec_plot = !ec_plot;
    }
}
#endif  // VIRTSER_ENABLE

void keyboard_post_init_kb(void) {
    debug_enable = true; // dprintf() in ecsm_dprint_matrix() is gated on this

    ec_thresholds_t t;
    eeconfig_read_kb_datablock(&t, 0, sizeof(t));
    if (t.low == 0 && t.high == 0) { // uninitialised eeprom
        t.low  = LOW_THRESHOLD;
        t.high = HIGH_THRESHOLD;
        eeconfig_update_kb_datablock(&t, 0, sizeof(t));
    } else if (t.high > 400 || t.high < HIGH_THRESHOLD) { // stale thresholds — auto-migrate
        t.low  = LOW_THRESHOLD;
        t.high = HIGH_THRESHOLD;
        eeconfig_update_kb_datablock(&t, 0, sizeof(t));
    }
    apply_thresholds(t.low, t.high);

    dprintf("EC thresholds low=%u high=%u\n", t.low, t.high);
    {
        matrix_row_t tmp[MATRIX_ROWS] = {0};
        ecsm_matrix_scan(tmp);
        dprintf("EC rest:\n");
        ecsm_dprint_matrix();
    }

    keyboard_post_init_user();
}

void housekeeping_task_kb(void) {
    static uint16_t plot_timer = 0;
    if (ec_plot && timer_elapsed(plot_timer) > 50) {
        plot_timer = timer_read();
        ecsm_dprint_matrix();
    }
}

// --- ECSKB Configurator: VIA raw-HID 0xec threshold read/write ---
#ifdef VIA_ENABLE
bool via_command_kb(uint8_t *data, uint8_t length) {
    // data[0] = command id, data[1] = value id (0xec == our EC thresholds)
    if (data[1] != 0xec) {
        return false; // let VIA handle it
    }

    switch (data[0]) {
        case id_get_keyboard_value: {
            ecsm_config_t c;
            ecsm_get_config(&c);
            data[2] = (c.low_threshold >> 8) & 0xff;
            data[3] = (c.low_threshold) & 0xff;
            data[4] = (c.high_threshold >> 8) & 0xff;
            data[5] = (c.high_threshold) & 0xff;
            raw_hid_send(data, length);
            return true;
        }
        case id_set_keyboard_value: {
            uint16_t low  = (((uint16_t)data[2]) << 8) | data[3];
            uint16_t high = (((uint16_t)data[4]) << 8) | data[5];
            if (low == 0xffff && high == 0xffff) { // reset
                low  = LOW_THRESHOLD;
                high = HIGH_THRESHOLD;
            }
            apply_thresholds(low, high);
            ec_thresholds_t t = {low, high};
            eeconfig_update_kb_datablock(&t, 0, sizeof(t));
            raw_hid_send(data, length);
            return true;
        }
        default:
            return false;
    }
}
#endif
