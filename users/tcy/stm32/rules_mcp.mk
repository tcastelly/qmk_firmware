OPT_DEFS += -include users/tcy/stm32/config_i2c1.h

# ── Matrix ──────────────────────────────────────────────────
CUSTOM_MATRIX = lite

# Left half MCP23017 handler
SRC += users/tcy/stm32/mcp_matrix.c users/tcy/stm32/i2c2_handler.c


# ── I2C ─────────────────────────────────────────────────────
# MANDATORY for:
#   - Azoteq IQS572 trackpad  (I2C1 — PB6/PB7)
#   - MCP23017 left half      (I2C2 — PB10/PB11)
I2C_DRIVER_REQUIRED = yes
I2C_ENABLE = yes


# MOUSE_ENABLE: required for mouse report USB HID descriptor.
MOUSE_ENABLE = yes

