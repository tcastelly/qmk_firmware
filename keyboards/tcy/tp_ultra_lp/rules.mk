-include $(QMK_USERSPACE)/users/$(KEYMAP)/rp2040/rules.mk

SERIAL_DRIVER = vendor
POINTING_DEVICE_ENABLE = yes
POINTING_DEVICE_DRIVER = azoteq_iqs5xx
AUDIO_DRIVER = pwm_hardware
PS2_USE_USART = yes
WPM_ENABLE = yes

SRC += $(QMK_USERSPACE)/users/tcy/ps2_acceleration.c
SRC += $(QMK_USERSPACE)/users/tcy/ps2_rotation.c
