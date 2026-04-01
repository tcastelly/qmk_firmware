# 42 keys matrix using MCP
OPT_DEFS += -include users/tcy/stm32/config_mcp_matrix.h

-include users/$(KEYMAP)/stm32/rules.mk
-include users/$(KEYMAP)/stm32/rules_mcp.mk
-include users/$(KEYMAP)/stm32/rules_ps2.mk

# ── Pointing device ──────────────────────────────────────────
POINTING_DEVICE_ENABLE = yes
POINTING_DEVICE_DRIVER = azoteq_iqs5xx


# ── Audio ────────────────────────────────────────────────────
# MANDATORY lines for PWM audio:
#   AUDIO_ENABLE = yes
#   AUDIO_DRIVER = pwm_hardware  (uses TIM1_CH1 on PA8)
# Pin and timer config is in config.h.
# AUDIO_ENABLE = no
AUDIO_DRIVER = pwm_hardware

