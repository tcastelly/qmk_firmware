# 42 keys matrix using MCP
OPT_DEFS += -include $(QMK_USERSPACE)/users/tcy/stm32/config_mcp_matrix.h

-include $(QMK_USERSPACE)/users/$(KEYMAP)/stm32/rules.mk
-include $(QMK_USERSPACE)/users/$(KEYMAP)/stm32/rules_mcp.mk
-include $(QMK_USERSPACE)/users/$(KEYMAP)/stm32/rules_pmw3360.mk
-include $(QMK_USERSPACE)/users/$(KEYMAP)/stm32/rules_ps2.mk
-include $(QMK_USERSPACE)/users/$(KEYMAP)/stm32/rules_elantech.mk
-include $(QMK_USERSPACE)/users/$(KEYMAP)/attiny85_rgb/rules.mk

# ── Pointing device ──────────────────────────────────────────
# custom, NOT azoteq: elantech.c defines pointing_device_driver_*, so the
# Elantech pad IS the QMK driver. This is the one difference from tp_mini_choc
# that is not additive — the PMW3360 and the trackpoint still ride along as
# extra devices (PMW3360_CUSTOM_ENABLE / the ps2.c merge in
# pointing_device_task_kb), but there can only be one real driver.
POINTING_DEVICE_ENABLE = yes
POINTING_DEVICE_DRIVER = custom

# ── MCP23008 — mouse buttons + scroll encoder (I2C2 @ 0x21) ──
# The define is read by users/tcy/stm32/mcp_matrix.c, which calls
# mcp_mouse_init() from matrix_init_custom() once I2C2 is up. Guarded rather
# than unconditional so the other STM32 boards, which do not compile
# mcp_mouse_handler.c, still link.
OPT_DEFS += -DMCP_MOUSE_ENABLE
SRC += users/tcy/mcp_mouse_handler.c

# ── Audio ────────────────────────────────────────────────────
# MANDATORY lines for PWM audio:
#   AUDIO_ENABLE = yes
#   AUDIO_DRIVER = pwm_hardware  (uses TIM1_CH1 on PA8)
# Pin and timer config is in config.h.
AUDIO_DRIVER = pwm_hardware
