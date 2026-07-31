# ── Pointing device: Cirque Gen4 as the PRIMARY custom driver ──────────────
# The driver lives in the userspace overlay. We do NOT set
# CIRQUE_GEN4_CUSTOM_ENABLE here, so cirque_gen4.c compiles its own
# pointing_device_driver_* hooks and QMK polls the pad automatically.
POINTING_DEVICE_ENABLE = yes
POINTING_DEVICE_DRIVER = custom
I2C_DRIVER_REQUIRED    = yes

SRC        += $(QMK_USERSPACE)/users/tcy/drivers/sensors/cirque_gen4/cirque_gen4.c
# Include path so the keymap can #include "cirque_gen4.h" — needed to drain the
# pinch-zoom accumulator (emitting zoom is a keymap concern, not a driver one).
EXTRAFLAGS += -I$(QMK_USERSPACE)/users/tcy/drivers/sensors/cirque_gen4

# Stroke-start motion guard (ms) — overridable from the command line:
#   qmk compile -kb tcy/try_cirquegen4 -km tcy -e CIRQUE_GEN4_GESTURE_GUARD_MS=60
# This is only the compiled-in DEFAULT; the keymap sets the live value per host.
CIRQUE_GEN4_GESTURE_GUARD_MS ?= 45
OPT_DEFS += -DCIRQUE_GEN4_GESTURE_GUARD_MS=$(CIRQUE_GEN4_GESTURE_GUARD_MS)
