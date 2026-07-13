# 42 keys matrix using MCP
OPT_DEFS += -include $(QMK_USERSPACE)/users/tcy/stm32/config_mcp_matrix.h

-include $(QMK_USERSPACE)/users/$(KEYMAP)/stm32/rules.mk
-include $(QMK_USERSPACE)/users/$(KEYMAP)/stm32/rules_mcp.mk

# ── Pointing device ──────────────────────────────────────────
POINTING_DEVICE_ENABLE = yes
POINTING_DEVICE_DRIVER = azoteq_iqs5xx

