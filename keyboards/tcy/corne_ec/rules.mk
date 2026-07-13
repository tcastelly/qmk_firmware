SERIAL_DRIVER = usart

# NOTE: every keymap must set VIA_ENABLE = yes. The cipulot common driver's
# ec_board.c registers via_cmd_slave_handler (defined in via_ec.c) for every
# SPLIT_KEYBOARD build, so a VIA-less keymap fails to link — and the EC
# per-key calibration is done through VIA anyway. (QMK lint forbids enabling
# VIA at keyboard level, hence per-keymap.)
