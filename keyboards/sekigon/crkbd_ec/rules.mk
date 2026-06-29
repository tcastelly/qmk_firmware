
CUSTOM_MATRIX = lite
SRC += analog.c ec_switch_matrix.c matrix.c
# RP2040 processor/bootloader now declared in info.json; modern QMK drives it
# via ChibiOS, so the EC driver takes its non-PLATFORM_PICO (ChibiOS) path.

SPLIT_KEYBOARD = yes
# The ChibiOS bitbang serial driver (default) uses palWaitLineTimeout which
# does not work reliably on RP2040. Use the RP2040-native PIO serial driver.
SERIAL_DRIVER = vendor
RGBLIGHT_ENABLE = no
RGB_MATRIX_ENABLE = no
# board frozen: no RGB / OLED / SPI (only tapdance shared via users/tcy)

# old GPIO/analog API names used by the EC driver -> modern names
CFLAGS += -include keyboards/sekigon/crkbd_ec/api_compat.h

# Build Options
#   change yes to no to disable
#
VIA_ENABLE = yes
VIRTSER_ENABLE = yes        # CDC serial for the Web Serial Plotter (EC AD-value stream)
BOOTMAGIC_ENABLE = yes      # Virtual DIP switch configuration
MOUSEKEY_ENABLE = yes       # Mouse keys
TAP_DANCE_ENABLE = yes
EXTRAKEY_ENABLE = yes       # Audio control and System control
CONSOLE_ENABLE = no         # (console was for the removed pico_cdc debug)
COMMAND_ENABLE = no         # Commands for debug and configuration
# Do not enable SLEEP_LED_ENABLE. it uses the same timer as BACKLIGHT_ENABLE
SLEEP_LED_ENABLE = no       # Breathing sleep LED during USB suspend
# if this doesn't work, see here: https://github.com/tmk/tmk_keyboard/wiki/FAQ#nkro-doesnt-work
NKRO_ENABLE = no            # USB Nkey Rollover
BACKLIGHT_ENABLE = no       # Enable keyboard backlight functionality
BLUETOOTH_ENABLE = no       # Enable Bluetooth
AUDIO_ENABLE = no           # Audio output

ifeq ($(HANDEDNESS),right)
	SRC += handedness_right.c
else
	SRC += handedness_left.c
endif

