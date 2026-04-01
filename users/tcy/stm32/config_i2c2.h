/* ── I2C2 — MCP23017 left half ──────────────────────────────
 * NOTE: I2C2 uses PB10/PB11 which conflict with PS2_PINSET 2.
 * When I2C2 is needed, set PS2_PINSET 1 (PB8/PB9) in ps2.c.
 *
 * These defines are informational — I2C2 driver is configured
 * in mcuconf.h. Uncomment if QMK needs explicit I2C2 defines.
 * ─────────────────────────────────────────────────────────── */
#define I2C_DRIVER       I2CD2
#define I2C2_SCL_PIN     B10
#define I2C2_SDA_PIN     B11
#define I2C2_SCL_PAL_MODE 1
#define I2C2_SDA_PAL_MODE 1
#define I2C2_TIMINGR_PRESC  2U
#define I2C2_TIMINGR_SCLDEL 1U
#define I2C2_TIMINGR_SDADEL 0U
#define I2C2_TIMINGR_SCLH   9U
#define I2C2_TIMINGR_SCLL   26U
