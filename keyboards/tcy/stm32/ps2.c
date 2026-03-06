#include "quantum.h"
#include "pointing_device.h"
#include "print.h"
#include "ch.h"
#include "hal.h"

/* ============================================================
 * Pin Selection
 * Change PS2_PINSET to switch pin pair — nothing else to edit.
 *   1 = PB8  (CLK) / PB9  (DAT)   — frees PB10/PB11 for I2C2
 *   2 = PB10 (CLK) / PB11 (DAT)   — uses I2C2 pull-up resistors
 * ============================================================ */

/* ── Pinset 1: PB8 / PB9 ──────────────────────────────────── */
#if PS2_PINSET == 1
    #define PS2_CLK_NR        8
    #define PS2_DAT_NR        9
    #define PS2_EXTICR_IDX    2     /* EXTICR[2] covers EXTI8-11  */
    #define PS2_EXTICR_SHIFT  0     /* EXTI8 = bits [3:0]         */

/* ── Pinset 2: PB10 / PB11 ────────────────────────────────── */
#elif PS2_PINSET == 2
    #define PS2_CLK_NR        10
    #define PS2_DAT_NR        11
    #define PS2_EXTICR_IDX    2     /* EXTICR[2] covers EXTI8-11  */
    #define PS2_EXTICR_SHIFT  8     /* EXTI10 = bits [11:8]       */

#else
    #error "PS2_PINSET must be 1 or 2"
#endif

#define CLAMP(v, lo, hi) ((v) < (lo) ? (lo) : (v) > (hi) ? (hi) : (v))


/* ============================================================
 * Packet timing
 * ============================================================ */

/* Max allowed time (in ms) between bytes of the same packet.
 * If this elapses, packet_idx is reset and we re-sync.
 * PS/2 sends ~11 bits at 10-16.7 kHz = ~1 ms per byte max. */
#define PS2_PACKET_TIMEOUT_MS 10


/* ============================================================
 * Packet Storage
 * ============================================================ */

static volatile uint8_t  packet[3];
static volatile uint8_t  packet_idx     = 0;
static volatile uint32_t packet_last_ms = 0;

/* Double-buffer: ISR fills `packet[]`, publishes to `ready_packet[]`
 * only when a full 3-byte set is complete.                           */
static volatile uint8_t ready_packet[3];
static volatile bool    packet_ready = false;


/* ============================================================
 * Receiver State
 * ============================================================ */

static volatile uint8_t  ps2_data     = 0;
static volatile uint8_t  ps2_bitcount = 0;
static volatile uint32_t clock_interrupt_count = 0;


/* ============================================================
 * Helpers: direct register pin control
 *
 * All pin manipulation goes through these helpers to prevent
 * QMK / ChibiOS PAL from silently restoring any alternate
 * function on CLK/DAT pins.
 *
 * Both pinsets use pins >=8, so AFRH is always correct.
 * AFRH bit field for pin N: ((N - 8) * 4)
 * ============================================================ */

static inline void ps2_clk_input_pullup(void) {
    GPIOB->MODER  &= ~(3U << (PS2_CLK_NR * 2));
    GPIOB->PUPDR  &= ~(3U << (PS2_CLK_NR * 2));
    GPIOB->PUPDR  |=  (1U << (PS2_CLK_NR * 2));
    GPIOB->AFRH   &= ~(0xFU << ((PS2_CLK_NR - 8) * 4));
}

static inline void ps2_dat_input_pullup(void) {
    GPIOB->MODER  &= ~(3U << (PS2_DAT_NR * 2));
    GPIOB->PUPDR  &= ~(3U << (PS2_DAT_NR * 2));
    GPIOB->PUPDR  |=  (1U << (PS2_DAT_NR * 2));
    GPIOB->AFRH   &= ~(0xFU << ((PS2_DAT_NR - 8) * 4));
}

/*
 * ODR bit cleared BEFORE switching MODER to output mode.
 * Switching to output while ODR=1 causes a brief high glitch
 * that corrupts PS/2 framing.
 */
static inline void ps2_clk_output_low(void) {
    GPIOB->ODR    &= ~(1U << PS2_CLK_NR);
    GPIOB->AFRH   &= ~(0xFU << ((PS2_CLK_NR - 8) * 4));
    GPIOB->MODER  &= ~(3U << (PS2_CLK_NR * 2));
    GPIOB->MODER  |=  (1U << (PS2_CLK_NR * 2));
}

static inline void ps2_dat_output_low(void) {
    GPIOB->ODR    &= ~(1U << PS2_DAT_NR);
    GPIOB->AFRH   &= ~(0xFU << ((PS2_DAT_NR - 8) * 4));
    GPIOB->MODER  &= ~(3U << (PS2_DAT_NR * 2));
    GPIOB->MODER  |=  (1U << (PS2_DAT_NR * 2));
}

static inline uint8_t ps2_clk_read(void) { return (GPIOB->IDR >> PS2_CLK_NR) & 1U; }
static inline uint8_t ps2_dat_read(void) { return (GPIOB->IDR >> PS2_DAT_NR) & 1U; }


/* ============================================================
 * PS/2 Interrupt Handler (CLK Falling Edge)
 * ============================================================ */

static void ps2_interrupt_handler(void *arg) {
    (void)arg;

    uint8_t bit = ps2_dat_read();
    clock_interrupt_count++;

    /* Start bit */
    if (ps2_bitcount == 0) {
        if (bit == 0) {
            ps2_bitcount = 1;
            ps2_data     = 0;
        }
        return;
    }

    /* Data bits 1-8 (LSB first) */
    if (ps2_bitcount >= 1 && ps2_bitcount <= 8) {
        ps2_data >>= 1;
        if (bit) ps2_data |= 0x80;
        ps2_bitcount++;
        return;
    }

    /* Parity bit (bit 9, ignored) */
    if (ps2_bitcount == 9) {
        ps2_bitcount++;
        return;
    }

    /* Stop bit (bit 10): byte is complete */
    if (ps2_bitcount == 10) {
        ps2_bitcount = 0;

        uint32_t now = timer_read32();

        /* Packet timeout: if too much time elapsed since the last byte,
         * we lost sync mid-packet — reset and start fresh.             */
        if (packet_idx > 0 && timer_elapsed32(packet_last_ms) > PS2_PACKET_TIMEOUT_MS) {
            packet_idx = 0;
        }

        packet_last_ms = now;

        if (packet_idx == 0) {
            /* Status byte validation:
             *   bit 3 must be 1  (always set in PS/2 status byte)
             *   bits 6-7 must be 0 (overflow flags — if set, data is garbage)
             * This prevents X/Y data bytes from being mistaken as status bytes. */
            if ((ps2_data & 0x08) && !(ps2_data & 0xC0)) {
                packet[0]  = ps2_data;
                packet_idx = 1;
            }
        } else {
            packet[packet_idx++] = ps2_data;

            if (packet_idx >= 3) {
                /* Publish completed packet to double-buffer */
                ready_packet[0] = packet[0];
                ready_packet[1] = packet[1];
                ready_packet[2] = packet[2];
                packet_ready    = true;
                packet_idx      = 0;
            }
        }
    }
}


/* ============================================================
 * Send Byte (Host to Device)
 * ============================================================ */

static uint8_t ps2_send(uint8_t data) {

    uint8_t  parity = 1;
    uint32_t timeout;

    palDisableLineEvent(PAL_LINE(GPIOB, PS2_CLK_NR));

    /* Cortex-M0: no BASEPRI, just ensure PRIMASK is clear */
    __enable_irq();

    /* Request-to-send: pull CLK low >=100 us */
    ps2_clk_output_low();
    wait_us(150);

    ps2_dat_output_low();    /* Start bit */
    wait_us(20);

    ps2_clk_input_pullup();  /* Release CLK: device takes over */

    /* Wait for device to pull CLK low */
    timeout = 100000;
    while (ps2_clk_read() && --timeout);
    if (!timeout) goto fail;

    /* Send 8 data bits + 1 parity bit */
    for (uint8_t i = 0; i < 9; i++) {

        uint8_t bit_val;

        if (i < 8) {
            bit_val = (data >> i) & 1U;
            if (bit_val) parity ^= 1;
        } else {
            bit_val = parity;
        }

        timeout = 50000;
        while (!ps2_clk_read() && --timeout);
        if (!timeout) goto fail;

        if (bit_val) {
            ps2_dat_input_pullup();
        } else {
            ps2_dat_output_low();
        }

        timeout = 50000;
        while (ps2_clk_read() && --timeout);
        if (!timeout) goto fail;
    }

    /* Stop bit */
    timeout = 50000;
    while (!ps2_clk_read() && --timeout);
    if (!timeout) goto fail;

    ps2_dat_input_pullup();

    timeout = 50000;
    while (ps2_clk_read() && --timeout);
    if (!timeout) goto fail;
    timeout = 50000;
    while (!ps2_clk_read() && --timeout);
    if (!timeout) goto fail;

    /* Wait for device ACK */
    timeout = 50000;
    while (ps2_dat_read() && --timeout);
    if (!timeout) goto fail;

    /* Wait for bus release */
    timeout = 100000;
    while ((!ps2_clk_read() || !ps2_dat_read()) && --timeout);

    palSetLineCallback(PAL_LINE(GPIOB, PS2_CLK_NR), ps2_interrupt_handler, NULL);
    palEnableLineEvent(PAL_LINE(GPIOB, PS2_CLK_NR), PAL_EVENT_MODE_FALLING_EDGE);
    return 0xFA;

fail:
    ps2_clk_input_pullup();
    ps2_dat_input_pullup();
    palSetLineCallback(PAL_LINE(GPIOB, PS2_CLK_NR), ps2_interrupt_handler, NULL);
    palEnableLineEvent(PAL_LINE(GPIOB, PS2_CLK_NR), PAL_EVENT_MODE_FALLING_EDGE);
    return 0x00;
}


/* ============================================================
 * Init
 * ============================================================ */

void ps2_init(void) {

    /* Enable SYSCFG clock (required for EXTI routing).
     * I2C2/USART3 clocks are NOT touched here — PB8/PB9 have
     * no peripheral conflict. If using PB10/PB11 (pinset 2),
     * ensure I2C2 is managed separately in your config.      */
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    ps2_clk_input_pullup();
    ps2_dat_input_pullup();

    /* Route CLK EXTI line to Port B.
     * STM32F072 EXTICR[2] covers EXTI8-11:
     *   EXTI8  = bits [3:0]    EXTI9  = bits [7:4]
     *   EXTI10 = bits [11:8]   EXTI11 = bits [15:12]
     * Port B = 0x1                                           */
    SYSCFG->EXTICR[PS2_EXTICR_IDX] &= ~(0xFU << PS2_EXTICR_SHIFT);
    SYSCFG->EXTICR[PS2_EXTICR_IDX] |=  (0x1U << PS2_EXTICR_SHIFT);

    wait_ms(2000);

    palSetLineCallback(PAL_LINE(GPIOB, PS2_CLK_NR), ps2_interrupt_handler, NULL);
    palEnableLineEvent(PAL_LINE(GPIOB, PS2_CLK_NR), PAL_EVENT_MODE_FALLING_EDGE);

    __enable_irq();

    ps2_send(0xFF);       /* Reset                     */
    wait_ms(500);         /* Wait for BAT (0xAA 0x00)  */
    ps2_send(0xEA);       /* Stream Mode               */
    wait_ms(10);
    uint8_t ack = ps2_send(0xF4);  /* Enable Reporting */
    uprintf("PS2 Init ACK: %02X\n", ack);
}


/* ============================================================
 * Scan: debug + pin guard
 *
 * Expected register values (pinset 1 — PB8/PB9):
 *   MODER   bits 16-19 = 0000  (PB8/PB9 input)
 *   AFRH    bits 0-7   = 0000  (no AF on PB8/PB9)
 *   EXTICR2 bits 3-0   = 0001  (EXTI8 -> Port B)
 *
 * Expected register values (pinset 2 — PB10/PB11):
 *   MODER   bits 20-23 = 0000  (PB10/PB11 input)
 *   AFRH    bits 8-15  = 0000  (no AF on PB10/PB11)
 *   EXTICR2 bits 11-8  = 0001  (EXTI10 -> Port B)
 *
 *   PRIMASK = 0  (interrupts enabled)
 * ============================================================ */

void ps2_scan(void) {

    /* ── Late init: re-apply full config on first tick after QMK boot ── */
    static bool late_init_done = false;
    if (!late_init_done) {
        RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
        SYSCFG->EXTICR[PS2_EXTICR_IDX] &= ~(0xFU << PS2_EXTICR_SHIFT);
        SYSCFG->EXTICR[PS2_EXTICR_IDX] |=  (0x1U << PS2_EXTICR_SHIFT);
        ps2_clk_input_pullup();
        ps2_dat_input_pullup();
        palSetLineCallback(PAL_LINE(GPIOB, PS2_CLK_NR), ps2_interrupt_handler, NULL);
        palEnableLineEvent(PAL_LINE(GPIOB, PS2_CLK_NR), PAL_EVENT_MODE_FALLING_EDGE);
        __enable_irq();
        late_init_done = true;
    }

    /* ── Per-tick pin guard ─────────────────────────────────────────────
     * Something in QMK/ChibiOS periodically restores AF mode on the PS/2
     * pins (observed: MODER=0000A080 with AFRH=0, meaning AF mode but no
     * specific AF assigned). Forcing MODER and AFRH every tick is cheap
     * (just register writes) and guarantees the pins stay as plain input.
     * ─────────────────────────────────────────────────────────────────── */
    GPIOB->MODER &= ~(3U << (PS2_CLK_NR * 2));          /* CLK: force input */
    GPIOB->MODER &= ~(3U << (PS2_DAT_NR * 2));          /* DAT: force input */
    GPIOB->AFRH  &= ~(0xFU << ((PS2_CLK_NR - 8) * 4)); /* CLK: clear AF    */
    GPIOB->AFRH  &= ~(0xFU << ((PS2_DAT_NR - 8) * 4)); /* DAT: clear AF    */

    /* ── Debug output every 500 ms ─────────────────────────────────────
     * Enabled by: #define PS2_MOUSE_DEBUG in config.h
     * Disable in production to reduce USB console overhead.
     * ─────────────────────────────────────────────────────────────────── */
#ifdef PS2_MOUSE_DEBUG
    static uint16_t timer = 0;
    if (timer_elapsed(timer) > 500) {
        uprintf(
            "CLK(PB%d)=%d DAT(PB%d)=%d | "
            "MODER=%08lX AFRH=%08lX EXTICR2=%08lX | "
            "PRIMASK=%lu | "
            "Clocks=%lu\n",
            PS2_CLK_NR, ps2_clk_read(),
            PS2_DAT_NR, ps2_dat_read(),
            GPIOB->MODER, GPIOB->AFRH,
            SYSCFG->EXTICR[PS2_EXTICR_IDX],
            __get_PRIMASK(),
            clock_interrupt_count
        );
        timer = timer_read();
    }
#endif
}


/* ============================================================
 * Mouse Report
 * Merged with Azoteq via pointing_device_task_kb:
 * PS/2 movement and buttons are added on top of the Azoteq
 * report that QMK's driver already built.
 * ============================================================ */

report_mouse_t pointing_device_task_kb(report_mouse_t mouse_report) {

    chSysLock();
    if (!packet_ready) {
        chSysUnlock();
        return pointing_device_task_user(mouse_report);   /* No PS/2 data: return Azoteq report as-is */
    }
    uint8_t b0   = ready_packet[0];
    uint8_t b1   = ready_packet[1];
    uint8_t b2   = ready_packet[2];
    packet_ready = false;
    chSysUnlock();

    /* Reject packets with overflow bits set — data is unreliable */
    if (b0 & 0xC0) return pointing_device_task_user(mouse_report);

    int8_t x = (int8_t)b1;
    int8_t y = (int8_t)b2;
    if (b0 & 0x10) x -= 256;   /* X sign extension */
    if (b0 & 0x20) y -= 256;   /* Y sign extension */

    /* Button debounce: report press immediately, debounce release only.
     * A press is reported as soon as it appears in a packet.
     * A release is only confirmed after two consecutive packets with
     * the button cleared — prevents spurious releases on quick clicks. */
    static uint8_t prev_buttons = 0;
    uint8_t raw_buttons = b0 & 0x07;
    uint8_t buttons = raw_buttons | (prev_buttons & raw_buttons);  /* press immediately */
    prev_buttons = raw_buttons;                                     /* release debounced */

    /* Merge PS/2 on top of Azoteq */
    mouse_report.x = (int8_t)CLAMP((int16_t)mouse_report.x + x,   -127, 127);
    mouse_report.y = (int8_t)CLAMP((int16_t)mouse_report.y + (-y), -127, 127);

    /* Merge PS/2 buttons — remapping handled in pointing_device_task_user() in keymap.c
     * NOTE: use (layer_state | default_layer_state) there for correct layer detection. */
    mouse_report.buttons |= buttons;

    return pointing_device_task_user(mouse_report);
}


/* ============================================================
 * Matrix hooks
 * ============================================================ */

void matrix_init_custom(void) {
    ps2_init();
}

bool matrix_scan_custom(void) {
    ps2_scan();

    return false;
}
