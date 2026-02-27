# ── Matrix ──────────────────────────────────────────────────
CUSTOM_MATRIX = lite

# Left half MCP23017 handler (uncomment when in use)
# SRC += matrix.c i2c2_handler.c

# ── PS/2 trackpoint (custom driver) ─────────────────────────
SRC += ps2.c
PS2_ENABLE = no


# ── Pointing device ──────────────────────────────────────────
POINTING_DEVICE_ENABLE = yes
POINTING_DEVICE_DRIVER = azoteq_iqs5xx

# MOUSE_ENABLE: required for mouse report USB HID descriptor.
MOUSE_ENABLE = yes


# ── I2C ─────────────────────────────────────────────────────
# MANDATORY for:
#   - Azoteq IQS572 trackpad  (I2C1 — PB6/PB7)
#   - MCP23017 left half      (I2C2 — PB10/PB11)
I2C_DRIVER_REQUIRED = yes
I2C_ENABLE = yes


# ── Audio ────────────────────────────────────────────────────
# MANDATORY lines for PWM audio:
#   AUDIO_ENABLE = yes
#   AUDIO_DRIVER = pwm_hardware  (uses TIM1_CH1 on PA8)
# Pin and timer config is in config.h.
AUDIO_ENABLE = yes
AUDIO_DRIVER = pwm_hardware


# ── Debug ────────────────────────────────────────────────────
# CONSOLE_ENABLE: enables uprintf() for debug logs over USB.
# Disable in production to save flash space.
CONSOLE_ENABLE = yes

