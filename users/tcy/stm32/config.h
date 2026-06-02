/* ── PS/2 trackpoint (custom driver in ps2.c) ───────────────
 * MANDATORY: PS2_PINSET selects the CLK/DAT pin pair.
 *
 *   PS2_PINSET 1 → PB8  (CLK) / PB9  (DAT)  free pins, no conflict
 *   PS2_PINSET 2 → PB10 (CLK) / PB11 (DAT)  R6/R7 pull-ups, conflicts I2C2
 *
 * Must also match STM32_EXTI_USE_EXTIx in mcuconf.h.
 * ─────────────────────────────────────────────────────────── */
#define PS2_MOUSE_DEBUG
#define PS2_DELAY  40

#define POINTING_DEVICE_TASK_EVERY_MS 1


/* ── SPI1 — PMW3360 trackball ───────────────────────────────
 * MANDATORY lines for SPI:
 *   SPI_DRIVER, SPI_SCK_PIN, SPI_MOSI_PIN, SPI_MISO_PIN
 *
 * MANDATORY for PMW3360:
 *   PMW33XX_CS_PIN, PMW33XX_CPI
 *
 * Pins: PA4 (CS) / PA5 (SCK) / PA6 (MISO) / PA7 (MOSI)
 * ─────────────────────────────────────────────────────────── */
#define SPI_DRIVER   SPID1
#define SPI_SCK_PIN  A5
#define SPI_MOSI_PIN A7
#define SPI_MISO_PIN A6

#define SPI_SCK_PAL_MODE  0
#define SPI_MOSI_PAL_MODE 0
#define SPI_MISO_PAL_MODE 0

// prevent the keyboard to sleep
#define NO_USB_WAKE_FROM_SUSPEND

#define PMW33XX_CS_PIN  A4
#define PMW33XX_CPI     1600

#ifndef __ASSEMBLER__
  #include "drivers/sensors/pmw3360.h"

  // Immediately overwrite them with correct PMW3360 values
  #undef REG_Config1
  #define REG_Config1 0x3f
  #undef REG_Config2
  #define REG_Config2 0x11
  #undef REG_Angle_Tune
  #define REG_Angle_Tune 0x42
  #undef REG_Inverse_Product_ID
  #define REG_Inverse_Product_ID 0x01
  #undef REG_Lift_Config
  #define REG_Lift_Config 0x4e
  #undef REG_SROM_ID
  #define REG_SROM_ID 0x2a
  #undef REG_SROM_Enable
  #define REG_SROM_Enable 0x13
  #undef REG_SROM_Load_Burst
  #define REG_SROM_Load_Burst 0x62
  #undef REG_Motion_Burst
  #define REG_Motion_Burst 0x50
  #undef REG_Power_Up_Reset
  #define REG_Power_Up_Reset 0x3a
#endif

// Official Driver Requirements
#define PMW33XX_CPI 1600
#define PMW33XX_CPI_MIN 100
#define PMW33XX_CPI_MAX 12000
#define PMW33XX_CPI_STEP 100
#define PMW33XX_LIFTOFF_DISTANCE 0x02
#define ROTATIONAL_TRANSFORM_ANGLE 0

#ifndef CONSTRAIN
#    define CONSTRAIN(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
#endif


/* ── Audio — PKLCS1212E4001 piezo buzzer ────────────────────
 * MANDATORY lines for PWM audio:
 *   AUDIO_PIN, AUDIO_PWM_DRIVER, AUDIO_PWM_CHANNEL,
 *   AUDIO_PWM_PAL_MODE (STM32-specific, sets AF number)
 *
 * Pin: PA8 = TIM1_CH1, AF2
 *   AUDIO_PWM_DRIVER  = PWMD[1]  ← from TIM[1]
 *   AUDIO_PWM_CHANNEL = [1]      ← from CH[1]
 *   AUDIO_PWM_PAL_MODE = [2]     ← from AF[2]
 * ─────────────────────────────────────────────────────────── */
#define AUDIO_PIN          A8
#define AUDIO_PWM_DRIVER   PWMD1
#define AUDIO_PWM_CHANNEL  1
#define AUDIO_PWM_PAL_MODE 2

#define AUDIO_VOICES
#define AUDIO_VOICE_DEFAULT butts_fader

#ifdef AUDIO_ENABLE
  #define STARTUP_SOUND E__NOTE(_E6), E__NOTE(_A6), ED_NOTE(_E7),
#endif

/* -- RGB -- */
#define RGBLIGHT_LED_COUNT 3
#define WS2812_DI_PIN A10
