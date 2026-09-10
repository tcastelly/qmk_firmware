# 42 keys matrix using MCP
OPT_DEFS += -include $(QMK_USERSPACE)/users/tcy/stm32/config_mcp_matrix.h

-include $(QMK_USERSPACE)/users/$(KEYMAP)/stm32/rules.mk
-include $(QMK_USERSPACE)/users/$(KEYMAP)/stm32/rules_mcp.mk
-include $(QMK_USERSPACE)/users/$(KEYMAP)/stm32/rules_pmw3360.mk
-include $(QMK_USERSPACE)/users/$(KEYMAP)/stm32/rules_ps2.mk
# -include $(QMK_USERSPACE)/users/$(KEYMAP)/stm32/rules_cirque_gen4.mk

# ── Pointing device ──────────────────────────────────────────
# Azoteq IQS572 is the primary QMK driver; pmw3360, ps2 and cirque_gen4 are
# added as extra devices and merged manually in pointing_device_task_user.
POINTING_DEVICE_ENABLE = yes
POINTING_DEVICE_DRIVER = azoteq_iqs5xx

# ── Audio ────────────────────────────────────────────────────
# MANDATORY lines for PWM audio:
#   AUDIO_ENABLE = yes
#   AUDIO_DRIVER = pwm_hardware  (uses TIM1_CH1 on PA8)
# Pin and timer config is in config.h.
# AUDIO_ENABLE = no
AUDIO_DRIVER = pwm_hardware

