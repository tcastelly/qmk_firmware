# try_trackpoint

Bench board for bringing up a **PS/2 touchpad** on QMK — specifically an
Elantech pad salvaged from an ASUS X453SA laptop, driven in **absolute mode** so
it reports multiple fingers, which is what makes two-finger scroll possible.

Not really a keyboard: one key and one PS/2 port. It exists to develop and tune
the driver, which lives in the userspace overlay and is meant to be shared with
real boards.

* Maintainer: Thomas CASTELLY
* MCU: RP2040 (`development_board: helios`)

---

## Hardware

| Signal | Pin | Notes |
|--------|-----|-------|
| PS/2 DATA | `GP0` | the PIO driver requires `clock == data + 1` |
| PS/2 CLOCK | `GP1` | uses PIO1 (`PS2_PIO_USE_PIO1` in `config.h`) |
| Key | `GP9` | direct pin, matrix `[0,0]` |

**Pull-ups.** PS/2 is open-drain and needs something holding the idle lines
high. The RP2040's internal pull-ups are enabled in
`platforms/chibios/drivers/vendor/RP/RP2040/ps2_vendor.c` (`PAL_RP_PAD_PUE`),
but at 50–80 kΩ they are marginal — PS/2 wants 1–10 kΩ. The symptom of them
being too weak is `err 10` (parity) under load, which can then wedge the bus.
**A real build should fit physical 4.7 kΩ resistors from GP0 and GP1 to 3V3**,
after which both `ps2_vendor.c` edits mentioned below become unnecessary.

---

## The pad

Identified on the bench with the `probe` keymap:

```
magic knock   3C 03 00        -> Elantech (Linux elantech_detect signature)
fw_version    0x381F18        -> IC gen 8 -> hw_version 4, 6-byte packets
abs range     3097 x 2119
traces        20 x 14
crc_enabled   no
fw & 0x001000 set             -> CLICKPAD (one button, no hardware right-click)
```

`hw_version 4` is the good case: up to 5 fingers via the status-packet bitmap.

---

## Keymaps

### `probe` — diagnostics

Read-only interrogation of whatever is on the PS/2 bus. Keeps QMK's `ps2_mouse`
module and only asks questions; it never switches the pad into absolute mode.

```bash
qmk compile -kb tcy/try_trackpoint -km probe
qmk console
```

Prints an electrical line test, a raw bus sniff, the plain PS/2 identity
(`0xF2` / `0xE9`), the Elantech magic knock, and queries `0x00`–`0x04` in both
wire forms, then a decoded verdict: vendor, fw/hw version, geometry, traces.
The single key re-runs it live.

Most useful when a pad does not respond at all — the line test separates "no
clock is being generated" (power, wiring, pinout) from "clock exists but is not
being decoded" (firmware).

### `tp` — the driver

```bash
qmk compile -kb tcy/try_trackpoint -km tp
qmk flash   -kb tcy/try_trackpoint -km tp
```

| Gesture | Action |
|---------|--------|
| One finger | cursor |
| Two fingers | scroll (vertical + horizontal) |
| One-finger tap | left click |
| Two-finger tap | right click |
| Three-finger tap | middle click |
| Physical click | left; right if two fingers are down |
| `GP9` key | cycle DPI 150 → 200 → 250 → 300 → 400 → 600 |

`PS2_MOUSE_ENABLE = no` is load-bearing: QMK's mouse module reads 3-byte
relative packets and would fight the driver over the same RX queue.

---

## Driver

Lives in `users/tcy/drivers/sensors/elantech/` in the **userspace overlay**, not
in this board directory — it is transport-agnostic and intended to be shared.

It uses only the QMK `ps2_host_*` API, so it runs on anything providing it: the
RP2040 PIO driver here, or `users/tcy/stm32/ps2.c` through its raw-byte shim
(`PS2_HOST_API_DEV` + `stm32/ps2_host_compat.h`). QMK's own `ps2_mouse.c` cannot
be used — it reads 3/4-byte relative packets straight into `report_mouse_t` and
never sees a finger count.

### Tuning

All in `elantech.h`. The ones most likely to need adjusting:

| Define | Default | Effect |
|--------|---------|--------|
| `ELANTECH_ACCEL_MIN_GAIN` | 128 | 0.5× at slow speed — lower = finer control |
| `ELANTECH_ACCEL_MAX_GAIN` | 320 | 1.25× at high speed — higher = more reach |
| `ELANTECH_SCROLL_DIVISOR` | 60 | pad units per wheel tick; higher = slower |
| `ELANTECH_TAP_TIME_MS` | 260 | tap window |
| `ELANTECH_SNAP_MIN` | 90 | step size treated as a snap rather than motion |

The acceleration curve is **CPI-independent by design**: gain is keyed to
physical finger speed in pad units, and CPI is applied afterwards as a uniform
multiplier. Changing DPI moves overall speed without altering how the
acceleration feels, so the constants never need retuning per DPI step.

Set `elantech_debug = true` for a running commentary (init, tap decisions, snap
suppressions).

---

## Things that cost time — read before debugging

**Init must be deferred off the boot path.** Host→device sends are not answered
from inside `keyboard_init()` on the RP2040 PIO transport: RX works (the pad's
power-on bytes arrive and can be drained) but every command times out. Measured
0/31 ACKs from `pointing_device_init()` versus 78/78 from the task context. The
driver arms a timer and initialises from
`pointing_device_driver_get_report()` instead.

**The pad's state survives a firmware reflash.** Reflashing resets the RP2040,
not the pad. A previous run can leave it in absolute mode, or leave the PIO
holding CLOCK low from an aborted transfer — which inhibits the device
permanently, so it presents as a dead pad. `ps2_host_transport_reset()` in
`ps2_vendor.c` recovers by restarting the state machine; it is declared **weak**
in the driver, so that core patch is optional. Without it, unplug/replug USB.

**The v4 register write is NINE commands:**

```
F8 00   F8 <reg>   F8 00   F8 <val>   E6
```

The `F8`+READWRITE pair appears **twice**. The v3 form (seven commands), and a
bare `00` in place of the second pair, are both ACKed by the pad and silently do
nothing — absolute mode never engages while init still reports success. ACKs
prove only that bytes were accepted, never that they were acted on. Verify by
looking at the packet format the pad actually emits.

**Setting the sample rate knocks the pad out of absolute mode.** `0xF3` must be
sent *before* the reg `0x07` write, never after. Linux wraps `set_rate` purely
to rewrite reg `0x07` afterwards.

**At light contact the pad repeats a stale position**, then snaps to the
finger's true location once tracking returns. That is a step, not a spike, so no
smoothing filter can remove it — a median makes it worse. It is detected (a
repeated sample followed by a large step) and suppressed. Suppression must
*hold* the last good position rather than re-anchor to the bad one, or the
discarded displacement biases the cursor consistently in one direction.

**Sanity-check thresholds against physics.** The pad is ~31 units/mm, so a 1 m/s
flick is ~310 units per sample at 100 Hz. A `MAX_DELTA` below that silently
discards real fast motion, which reads as "moving slowly goes further than
moving fast".

Reference: Linux `drivers/input/mouse/elantech.c`. Read it rather than recalling
it — several of the traps above came from misremembering it.

---

## Porting to another board

1. Provide the `ps2_host_*` API — QMK's PIO/interrupt driver, or the shim in
   `users/tcy/stm32/ps2.c` (`#define PS2_HOST_API_DEV <index into PS2_PINSETS>`).
2. In the keymap `rules.mk`: `POINTING_DEVICE_ENABLE = yes`,
   `POINTING_DEVICE_DRIVER = custom`, `PS2_MOUSE_ENABLE = no`.
3. `SRC += $(QMK_USERSPACE)/users/tcy/drivers/sensors/elantech/elantech.c` and
   add that directory to `EXTRAFLAGS -I`.

On a `PS2_CUSTOM_ENABLE` build the driver includes `stm32/ps2_host_compat.h`
automatically instead of QMK's `ps2.h`.

## Bootloader

* Double-tap the reset button, or the button on the board
* `QK_BOOT` is not mapped — the single key is the DPI cycle
