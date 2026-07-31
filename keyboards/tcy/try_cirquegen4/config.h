#pragma once

/* =============================================================================
 * try_cirquegen4 — bare RP2040 (Sea-Picro) + Cirque Gen4 (TM105065) trackpad.
 * Just the MCU and the trackpad, nothing else. The Cirque is the board's
 * PRIMARY pointing device (POINTING_DEVICE_DRIVER = custom, see rules.mk).
 * ========================================================================== */

/* ── Cirque Gen4 trackpad — primary pointing device ─────────────────────────
 * Driver: users/tcy/drivers/sensors/cirque_gen4. CIRQUE_GEN4_CUSTOM_ENABLE is
 * NOT set here, so the driver supplies the QMK pointing_device_driver_* hooks
 * and QMK polls it automatically — no pointing_device_task_user needed.
 * ────────────────────────────────────────────────────────────────────────── */
#define CIRQUE_GEN4_I2C_BUS 1
#define CIRQUE_GEN4_ADDR    0x2A   /* 0x2C is the factory alt; 0x2A seen on the tp_mini_choc pad */

/* ── I2C0 on GP0 (SDA) / GP1 (SCL) ──────────────────────────────────────────
 * GP0/GP1 are RP2040 I2C0-capable pins → I2CD0. QMK's macro names keep the
 * "I2C1_" prefix for the first bus regardless of which peripheral it maps to.
 * ────────────────────────────────────────────────────────────────────────── */
#undef  I2C_DRIVER
#define I2C_DRIVER   I2CD0
#undef  I2C1_SDA_PIN
#define I2C1_SDA_PIN GP0
#undef  I2C1_SCL_PIN
#define I2C1_SCL_PIN GP1

/* ── Pointing task rate ─────────────────────────────────────────────────────
 * The Gen4 updates internally at ~100 Hz and clock-stretches on every blind
 * read (no DR pin wired). On this bare RP2040 the main loop is so fast the
 * pointing task would otherwise fire at ~1000 Hz (QMK's default throttle is
 * 1 ms), hammering the pad ~10x too fast and saturating the loop with
 * back-to-back stretched reads → laggy cursor. Match the pad's 100 Hz.
 * (tp_mini_choc never hit this: its heavier loop already polled near 100 Hz.)
 * ────────────────────────────────────────────────────────────────────────── */
#define POINTING_DEVICE_TASK_THROTTLE_MS 10

/* ── Data Ready (DR) line ───────────────────────────────────────────────────
 * Pin 7 of the module's 10-pin FFC, wired to GP29.
 *
 * Without this the driver reads blind every poll, and an I2C-HID device with
 * nothing to report stretches the clock while it answers — that stalling is
 * the main source of cursor lag here. With DR wired, an idle poll is a single
 * GPIO read and touches the bus not at all.
 *
 * NOTE: GP29 used to be the dummy matrix pin; keyboard.json now uses GP28 so
 * the matrix scan and the DR read do not fight over the same pin.
 * ────────────────────────────────────────────────────────────────────────── */
#define CIRQUE_GEN4_DR_PIN GP29

/* Temporary: 1 Hz poll/I2C stats on the console. Remove once measured. */
#define CIRQUE_GEN4_STATS 1

/* Temporary: log report-0x08 keyboard frames (gesture keystrokes). */
#define CIRQUE_GEN4_KBD_LOG 1

/* Native gestures: forward the pad's own keyboard frames (Ctrl+wheel zoom,
 * three-finger back/forward/Start). Linux/Windows only - see the header. */
#define CIRQUE_GEN4_FORWARD_KBD 1

/* ── Bootloader trigger ─────────────────────────────────────────────────────
 * Short GP25 → GND to jump into the RP2040 BOOTSEL mass-storage bootloader
 * (BOOTSEL) for reflashing. Handled in try_cirquegen4.c (input pull-up + poll).
 * ────────────────────────────────────────────────────────────────────────── */
#define BOOTSEL_TRIGGER_PIN GP25
