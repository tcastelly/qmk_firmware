# Corne EC F072

Custom split 3x6+3 (42-key, Corne layout) **electrostatic capacitive** keyboard.
One STM32F072CBT6 per half, both halves electrically identical and running the
**same firmware image** (handedness strap on C13). Firmware side it is a thin
wrapper around the unmodified **cipulot common EC driver**
(`keyboards/cipulot/common`), so it rides qmk master and gets Cipulot's VIA
per-key calibration (APC / Rapid Trigger, bottoming calibration, SOCD) for free.

Hardware topology per half (same electrical design as ssbb/tako rev0 and
cipulot ec_split_60):

- 4 drive (row) lines strobed by GPIO
- 6 sense (column) pads into one **74HC4051** 8:1 analog mux
- **OPA350** rail-to-rail op-amp buffering the peak-hold cap into the ADC
- one discharge pin resetting the peak-hold cap between reads

## Pinout (both halves identical)

| Function | Pins |
|---|---|
| Rows (drive) | B12, B13, B14, B15 |
| AMUX select S0/S1/S2 | B3, B4, B5 |
| AMUX enable (active low) | B0 |
| Discharge | A2 |
| ADC in (op-amp out) | A3 (ADC1_IN3) |
| Split UART full duplex | A9 (TX) → other half RX, A10 (RX) ← other half TX (AF1, crossed in the cable) |
| Handedness strap | C13 — **high = left, low = right** |
| USB | A11/A12 (crystal-less, HSI48) |
| SWD | A13/A14 |
| **Sacrificed** (dummy row padding, gets strobed — leave unconnected) | B2 |

Reserved for later peripherals (users/tcy conventions, already enabled in
halconf/mcuconf): **I2C1** B6/B7 (trackpad), **SPI1** A4–A7 (PMW3360 trackball),
**A8** TIM1_CH1 (piezo). Still free: A0, A1, A15, B1, B8, B9, B10, B11, F0, F1,
C14, C15.

`AMUX_0_COL_CHANNELS` in `config.h` is a straight 0–5 mapping — reorder it to
match the PCB routing instead of fighting the router.

## Build

VIA is always on (the split RPC handler lives in the driver's `via_ec.c`).

```bash
cd ~/Documents/dev/workspaces/qmk/qmk_userspace
qmk compile -j 0 -kb tcy/corne_ec -km tcy       # shared users/tcy keymap
qmk compile -j 0 -kb tcy/corne_ec -km default
```

Flash the same `.bin` to both halves (`stm32-dfu`: BOOT0 button, or `QK_BOOT`).

## Calibration

Same workflow as the other cipulot-driver boards: per-key APC/RT thresholds and
bottoming calibration are edited live over VIA raw HID (Cipulot's EC VIA custom
UI); values persist in the emulated EEPROM (wear-leveled MCU flash, 8 KB
backing store at the top of the 128 KB).
