/* ── I2C1 — Imported by rules.mk  ─────────────────────────
 * QMK will use I2C1 as default for pointing device like Azoteq
 * MANDATORY lines for I2C1:
 *   I2C_DRIVER, I2C1_SCL_PIN, I2C1_SDA_PIN,
 *   I2C1_SCL_PAL_MODE, I2C1_SDA_PAL_MODE,
 *   I2C1_TIMINGR_* (all 5 values)
 *
 * Pins: PB6 (SCL) / PB7 (SDA) — R4/R5 5.1k pull-ups on PCB
 * AF1 = I2C1 on PB6/PB7
 * ─────────────────────────────────────────────────────────── */
#define I2C_DRIVER          I2CD1
#define I2C1_SCL_PIN        B6
#define I2C1_SDA_PIN        B7
#define I2C1_SCL_PAL_MODE   1
#define I2C1_SDA_PAL_MODE   1
#define I2C1_TIMINGR_PRESC  0x00U
#define I2C1_TIMINGR_SCLDEL 0x03U
#define I2C1_TIMINGR_SDADEL 0x01U
#define I2C1_TIMINGR_SCLH   0x03U
#define I2C1_TIMINGR_SCLL   0x09U
