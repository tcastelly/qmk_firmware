CONSOLE_ENABLE = yes
CUSTOM_MATRIX = lite

 SRC += matrix.c i2c2_handler.c
SRC += ps2.c

POINTING_DEVICE_ENABLE = yes
# POINTING_DEVICE_DRIVER = custom
POINTING_DEVICE_DRIVER = azoteq_iqs5xx

I2C_DRIVER_REQUIRED = yes
I2C_ENABLE = yes

# Disable the built-in interrupt driver
PS2_ENABLE = no           

MOUSE_ENABLE = yes
