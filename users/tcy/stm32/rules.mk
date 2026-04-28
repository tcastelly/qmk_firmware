EXTRAFLAGS += -Iusers/tcy/stm32

# enable i2c1 correctly
OPT_DEFS += -include users/tcy/stm32/config_i2c1.h
SRC += users/tcy/stm32/i2c1.c

# handle i2c2 manually
SRC += users/tcy/stm32/i2c2_handler.c
