# ── PS/2 trackpoint (custom driver) ─────────────────────────
PS2_ENABLE = no
PS2_CUSTOM_ENABLE = yes

ifeq ($(PS2_CUSTOM_ENABLE), yes)
    OPT_DEFS += -DPS2_CUSTOM_ENABLE
endif

SRC += users/tcy/stm32/ps2.c

