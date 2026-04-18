#include "quantum.h"
#include "pointing_device.h"
#include "ps2_acceleration.c"
#include "print.h"
#include "ch.h"
#include "hal.h"

/* ============================================================
 * Multi-device PS/2 driver
 *
 * Usage — define a list of pinset numbers in config.h:
 * #define PS2_PINSETS {1, 3}       // PB8/PB9 + PA4/PA5
 * #define PS2_PINSETS {1, 2}       // PB8/PB9 + PB10/PB11
 * #define PS2_PINSETS {1, 2, 3}    // all three
 *
 * Available pinsets:
 * 1 = PB8  (CLK) / PB9  (DAT)  — frees PB10/PB11 for I2C2
 * 2 = PB10 (CLK) / PB11 (DAT)  — uses I2C2 pull-up resistors
 * 3 = PA4  (CLK) / PA5  (DAT)  — SPI1 pins, not 5V tolerant
 *
 * Reports from all devices are merged into a single mouse report.
 * ============================================================ */

#ifndef PS2_PINSETS
    #error "PS2_PINSETS must be defined, e.g. #define PS2_PINSETS {1, 2}"
#endif

static const uint8_t ps2_pinset_list[] = PS2_PINSETS;
#define PS2_NUM_DEVICES (sizeof(ps2_pinset_list) / sizeof(ps2_pinset_list[0]))

#define CLAMP(v, lo, hi) ((v) < (lo) ? (lo) : (v) > (hi) ? (hi) : (v))
#define PS2_PACKET_TIMEOUT_MS 10
#define PS2_FIFO_SIZE 2


/* ============================================================
 * Pin configuration lookup table
 * One entry per supported pinset number (index = pinset number).
 * ============================================================ */

typedef struct {
    stm32_gpio_t *gpio;
    uint8_t       clk_nr;
    uint8_t       dat_nr;
    uint8_t       exticr_idx;
    uint8_t       exticr_shift;
    uint8_t       port_id;   /* 0x0=PortA  0x1=PortB  ... */
    bool          use_afrl;  /* true = pins<8 → AFR[0], false = pins>=8 → AFR[1] */
} ps2_pin_config_t;

/* Index 0 unused — pinsets start at 1 */
static const ps2_pin_config_t ps2_pin_configs[] = {
    [0] = { NULL,  0,  0, 0, 0,  0,     false },  /* unused       */
    [1] = { GPIOB, 8,  9, 2, 0,  0x1U,  false },  /* PB8  / PB9   */
    [2] = { GPIOB, 10, 11,2, 8,  0x1U,  false },  /* PB10 / PB11  */
    [3] = { GPIOA, 4,  5, 1, 0,  0x0U,  true  },  /* PA4  / PA5   */
};
#define PS2_MAX_PINSET 3


/* ============================================================
 * Per-device state
 * ============================================================ */

typedef struct {
    /* Bit receiver */
    volatile uint8_t  ps2_data;
    volatile uint8_t  ps2_bitcount;
    volatile uint32_t clock_interrupt_count;

    /* Packet assembly */
    volatile uint8_t  packet[3];
    volatile uint8_t  packet_idx;
    volatile uint32_t packet_last_ms;

    /* 2-deep FIFO */
    volatile uint8_t  fifo_buf[PS2_FIFO_SIZE][3];
    volatile uint8_t  fifo_head;
    volatile uint8_t  fifo_tail;
    volatile uint8_t  fifo_count;

    /* Button state */
    uint8_t held_buttons;
    uint8_t prev_buttons;
} ps2_device_state_t;

static ps2_device_state_t ps2_state[PS2_NUM_DEVICES];

/* Exposed for keymap use — OR of all devices' button states */
uint8_t ps2_buttons_state = 0;


/* ============================================================
 * Register helpers (per-device, no globals)
 * ============================================================ */

static inline void ps2_pin_input_pullup(stm32_gpio_t *gpio, uint8_t nr, bool afrl) {
    gpio->MODER &= ~(3U << (nr * 2));
    gpio->PUPDR &= ~(3U << (nr * 2));
    gpio->PUPDR |=  (1U << (nr * 2));
    if (afrl) { gpio->AFRL &= ~(0xFU << (nr * 4));         }
    else       { gpio->AFRH &= ~(0xFU << ((nr - 8) * 4)); }
}

static inline void ps2_pin_output_low(stm32_gpio_t *gpio, uint8_t nr, bool afrl) {
    gpio->ODR   &= ~(1U << nr);
    if (afrl) { gpio->AFRL &= ~(0xFU << (nr * 4));         }
    else       { gpio->AFRH &= ~(0xFU << ((nr - 8) * 4)); }
    gpio->MODER &= ~(3U << (nr * 2));
    gpio->MODER |=  (1U << (nr * 2));
}

static inline uint8_t ps2_pin_read(stm32_gpio_t *gpio, uint8_t nr) {
    return (gpio->IDR >> nr) & 1U;
}

static inline void ps2_pin_guard(stm32_gpio_t *gpio, uint8_t nr, bool afrl) {
    gpio->MODER &= ~(3U << (nr * 2));
    if (afrl) { gpio->AFRL &= ~(0xFU << (nr * 4));         }
    else       { gpio->AFRH &= ~(0xFU << ((nr - 8) * 4)); }
}


/* ============================================================
 * PS/2 Interrupt Handler (per device)
 * arg = pointer to device index (cast from uintptr_t)
 * ============================================================ */

static void ps2_interrupt_handler(void *arg) {
    uint8_t dev_idx = (uint8_t)(uintptr_t)arg;
    ps2_device_state_t     *dev = &ps2_state[dev_idx];
    const ps2_pin_config_t *cfg = &ps2_pin_configs[ps2_pinset_list[dev_idx]];

    uint8_t bit = ps2_pin_read(cfg->gpio, cfg->dat_nr);
    dev->clock_interrupt_count++;

    /* Start bit */
    if (dev->ps2_bitcount == 0) {
        if (bit == 0) { dev->ps2_bitcount = 1; dev->ps2_data = 0; }
        return;
    }

    /* Data bits 1-8 */
    if (dev->ps2_bitcount >= 1 && dev->ps2_bitcount <= 8) {
        dev->ps2_data >>= 1;
        if (bit) dev->ps2_data |= 0x80;
        dev->ps2_bitcount++;
        return;
    }

    /* Parity bit (ignored) */
    if (dev->ps2_bitcount == 9) { dev->ps2_bitcount++; return; }

    /* Stop bit — byte complete */
    if (dev->ps2_bitcount == 10) {
        dev->ps2_bitcount = 0;
        uint32_t now = timer_read32();

        if (dev->packet_idx > 0 &&
            timer_elapsed32(dev->packet_last_ms) > PS2_PACKET_TIMEOUT_MS) {
            dev->packet_idx = 0;
        }
        dev->packet_last_ms = now;

        if (dev->packet_idx == 0) {
            if ((dev->ps2_data & 0x08) && !(dev->ps2_data & 0xC0)) {
                dev->packet[0]  = dev->ps2_data;
                dev->packet_idx = 1;
            }
        } else {
            dev->packet[dev->packet_idx++] = dev->ps2_data;
            if (dev->packet_idx >= 3) {
                dev->fifo_buf[dev->fifo_head][0] = dev->packet[0];
                dev->fifo_buf[dev->fifo_head][1] = dev->packet[1];
                dev->fifo_buf[dev->fifo_head][2] = dev->packet[2];
                dev->fifo_head = (dev->fifo_head + 1) % PS2_FIFO_SIZE;
                if (dev->fifo_count < PS2_FIFO_SIZE) {
                    dev->fifo_count++;
                } else {
                    dev->fifo_tail = (dev->fifo_tail + 1) % PS2_FIFO_SIZE;
                }
                dev->packet_idx = 0;
            }
        }
    }
}

/* One static handler per device (ChibiOS PAL needs a unique function pointer per line) */
static void ps2_handler_dev0(void *arg) { (void)arg; ps2_interrupt_handler((void*)0); }
static void ps2_handler_dev1(void *arg) { (void)arg; ps2_interrupt_handler((void*)1); }
static void ps2_handler_dev2(void *arg) { (void)arg; ps2_interrupt_handler((void*)2); }

typedef void (*ps2_handler_fn)(void *);
static const ps2_handler_fn ps2_handlers[] = {
    ps2_handler_dev0,
    ps2_handler_dev1,
    ps2_handler_dev2,
};
#define PS2_MAX_HANDLERS (sizeof(ps2_handlers) / sizeof(ps2_handlers[0]))


/* ============================================================
 * Send Byte (per device)
 * ============================================================ */

static uint8_t ps2_send_to(uint8_t dev_idx, uint8_t data) {
    const ps2_pin_config_t *cfg = &ps2_pin_configs[ps2_pinset_list[dev_idx]];
    uint8_t  parity = 1;
    uint32_t timeout;

    palDisableLineEvent(PAL_LINE(cfg->gpio, cfg->clk_nr));
    __enable_irq();

    ps2_pin_output_low(cfg->gpio, cfg->clk_nr, cfg->use_afrl);
    wait_us(150);
    ps2_pin_output_low(cfg->gpio, cfg->dat_nr, cfg->use_afrl);
    wait_us(20);
    ps2_pin_input_pullup(cfg->gpio, cfg->clk_nr, cfg->use_afrl);

    timeout = 100000;
    while (ps2_pin_read(cfg->gpio, cfg->clk_nr) && --timeout);
    if (!timeout) goto fail;

    for (uint8_t i = 0; i < 9; i++) {
        uint8_t bit_val;
        if (i < 8) {
            bit_val = (data >> i) & 1U;
            if (bit_val) parity ^= 1;
        } else {
            bit_val = parity;
        }

        timeout = 50000;
        while (!ps2_pin_read(cfg->gpio, cfg->clk_nr) && --timeout);
        if (!timeout) goto fail;

        if (bit_val) { ps2_pin_input_pullup(cfg->gpio, cfg->dat_nr, cfg->use_afrl); }
        else          { ps2_pin_output_low(cfg->gpio, cfg->dat_nr, cfg->use_afrl);   }

        timeout = 50000;
        while (ps2_pin_read(cfg->gpio, cfg->clk_nr) && --timeout);
        if (!timeout) goto fail;
    }

    timeout = 50000;
    while (!ps2_pin_read(cfg->gpio, cfg->clk_nr) && --timeout);
    if (!timeout) goto fail;

    ps2_pin_input_pullup(cfg->gpio, cfg->dat_nr, cfg->use_afrl);

    timeout = 50000;
    while (ps2_pin_read(cfg->gpio, cfg->clk_nr) && --timeout);
    if (!timeout) goto fail;
    timeout = 50000;
    while (!ps2_pin_read(cfg->gpio, cfg->clk_nr) && --timeout);
    if (!timeout) goto fail;

    timeout = 50000;
    while (ps2_pin_read(cfg->gpio, cfg->dat_nr) && --timeout);
    if (!timeout) goto fail;

    timeout = 100000;
    while ((!ps2_pin_read(cfg->gpio, cfg->clk_nr) ||
            !ps2_pin_read(cfg->gpio, cfg->dat_nr)) && --timeout);

    palSetLineCallback(PAL_LINE(cfg->gpio, cfg->clk_nr), ps2_handlers[dev_idx], NULL);
    palEnableLineEvent(PAL_LINE(cfg->gpio, cfg->clk_nr), PAL_EVENT_MODE_FALLING_EDGE);
    return 0xFA;

fail:
    ps2_pin_input_pullup(cfg->gpio, cfg->clk_nr, cfg->use_afrl);
    ps2_pin_input_pullup(cfg->gpio, cfg->dat_nr, cfg->use_afrl);
    palSetLineCallback(PAL_LINE(cfg->gpio, cfg->clk_nr), ps2_handlers[dev_idx], NULL);
    palEnableLineEvent(PAL_LINE(cfg->gpio, cfg->clk_nr), PAL_EVENT_MODE_FALLING_EDGE);
    return 0x00;
}


/* ============================================================
 * Init
 * ============================================================ */

void ps2_stm32_init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    for (uint8_t i = 0; i < PS2_NUM_DEVICES; i++) {
        uint8_t pinset = ps2_pinset_list[i];
        if (pinset == 0 || pinset > PS2_MAX_PINSET) {
            uprintf("PS2 dev%d: invalid pinset %d\n", i, pinset);
            continue;
        }
        if (i >= PS2_MAX_HANDLERS) {
            uprintf("PS2 dev%d: no handler available (max %d devices)\n", i, (int)PS2_MAX_HANDLERS);
            continue;
        }

        const ps2_pin_config_t *cfg = &ps2_pin_configs[pinset];

        ps2_pin_input_pullup(cfg->gpio, cfg->clk_nr, cfg->use_afrl);
        ps2_pin_input_pullup(cfg->gpio, cfg->dat_nr, cfg->use_afrl);

        SYSCFG->EXTICR[cfg->exticr_idx] &= ~(0xFU << cfg->exticr_shift);
        SYSCFG->EXTICR[cfg->exticr_idx] |=  (cfg->port_id << cfg->exticr_shift);
    }

    wait_ms(2000);

    for (uint8_t i = 0; i < PS2_NUM_DEVICES; i++) {
        uint8_t pinset = ps2_pinset_list[i];
        if (pinset == 0 || pinset > PS2_MAX_PINSET || i >= PS2_MAX_HANDLERS) continue;

        const ps2_pin_config_t *cfg = &ps2_pin_configs[pinset];
        palSetLineCallback(PAL_LINE(cfg->gpio, cfg->clk_nr), ps2_handlers[i], NULL);
        palEnableLineEvent(PAL_LINE(cfg->gpio, cfg->clk_nr), PAL_EVENT_MODE_FALLING_EDGE);
    }

    __enable_irq();

    for (uint8_t i = 0; i < PS2_NUM_DEVICES; i++) {
        uint8_t pinset = ps2_pinset_list[i];
        if (pinset == 0 || pinset > PS2_MAX_PINSET || i >= PS2_MAX_HANDLERS) continue;

        ps2_send_to(i, 0xFF);   /* Reset            */
        wait_ms(500);
        ps2_send_to(i, 0xEA);   /* Stream Mode      */
        wait_ms(10);
        uint8_t ack = ps2_send_to(i, 0xF4);  /* Enable Reporting */
        (void)ack;
        uprintf("PS2 dev%d (pinset %d) Init ACK: %02X\n", i, pinset, ack);
    }
}


/* ============================================================
 * Scan: pin guard + debug
 * ============================================================ */

void ps2_stm32_scan(void) {
    static bool late_init_done = false;
    if (!late_init_done) {
        RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
        for (uint8_t i = 0; i < PS2_NUM_DEVICES; i++) {
            uint8_t pinset = ps2_pinset_list[i];
            if (pinset == 0 || pinset > PS2_MAX_PINSET || i >= PS2_MAX_HANDLERS) continue;
            const ps2_pin_config_t *cfg = &ps2_pin_configs[pinset];
            SYSCFG->EXTICR[cfg->exticr_idx] &= ~(0xFU << cfg->exticr_shift);
            SYSCFG->EXTICR[cfg->exticr_idx] |=  (cfg->port_id << cfg->exticr_shift);
            ps2_pin_input_pullup(cfg->gpio, cfg->clk_nr, cfg->use_afrl);
            ps2_pin_input_pullup(cfg->gpio, cfg->dat_nr, cfg->use_afrl);
            palSetLineCallback(PAL_LINE(cfg->gpio, cfg->clk_nr), ps2_handlers[i], NULL);
            palEnableLineEvent(PAL_LINE(cfg->gpio, cfg->clk_nr), PAL_EVENT_MODE_FALLING_EDGE);
        }
        __enable_irq();
        late_init_done = true;
    }

    /* Per-tick pin guard for all devices */
    for (uint8_t i = 0; i < PS2_NUM_DEVICES; i++) {
        uint8_t pinset = ps2_pinset_list[i];
        if (pinset == 0 || pinset > PS2_MAX_PINSET) continue;
        const ps2_pin_config_t *cfg = &ps2_pin_configs[pinset];
        ps2_pin_guard(cfg->gpio, cfg->clk_nr, cfg->use_afrl);
        ps2_pin_guard(cfg->gpio, cfg->dat_nr, cfg->use_afrl);
    }

#ifdef PS2_MOUSE_DEBUG
    static uint16_t timer = 0;
    if (timer_elapsed(timer) > 500) {
        for (uint8_t i = 0; i < PS2_NUM_DEVICES; i++) {
            uint8_t pinset = ps2_pinset_list[i];
            if (pinset == 0 || pinset > PS2_MAX_PINSET) continue;
            const ps2_pin_config_t *cfg = &ps2_pin_configs[pinset];
            char port = (cfg->gpio == (stm32_gpio_t *)GPIOA) ? 'A' : 'B';
            (void)port;
            uprintf(
                "dev%d CLK(P%c%d)=%d DAT(P%c%d)=%d | "
                "MODER=%08lX EXTICR=%08lX | Clocks=%lu\n",
                i, port, cfg->clk_nr, ps2_pin_read(cfg->gpio, cfg->clk_nr),
                   port, cfg->dat_nr, ps2_pin_read(cfg->gpio, cfg->dat_nr),
                cfg->gpio->MODER,
                SYSCFG->EXTICR[cfg->exticr_idx],
                ps2_state[i].clock_interrupt_count
            );
        }
        timer = timer_read();
    }
#endif
}


/* ============================================================
 * Mouse Report — merge all devices Per and Device Acceleration
 * ============================================================ */
static bool ps2_process_device(uint8_t dev_idx, report_mouse_t *mouse_report) {
    ps2_device_state_t *dev = &ps2_state[dev_idx];

    chSysLock();
    if (dev->fifo_count == 0) {
        chSysUnlock();
        mouse_report->buttons |= dev->held_buttons;
        ps2_buttons_state     |= dev->held_buttons;
        return false;
    }

    uint8_t b0 = dev->fifo_buf[dev->fifo_tail][0];
    uint8_t b1 = dev->fifo_buf[dev->fifo_tail][1];
    uint8_t b2 = dev->fifo_buf[dev->fifo_tail][2];
    dev->fifo_tail  = (dev->fifo_tail + 1) % PS2_FIFO_SIZE;
    dev->fifo_count--;
    chSysUnlock();

    if (b0 & 0xC0) return false;

    int8_t x = (int8_t)b1;
    int8_t y = (int8_t)b2;
    if (b0 & 0x10) x -= 256;
    if (b0 & 0x20) y -= 256;

    report_mouse_t dev_report = {0};
    dev_report.x = x;
    dev_report.y = -y;

    uint8_t pinset = ps2_pinset_list[dev_idx];

    // Process Pinset 1
    if (pinset == 1) {
        // run acceleration first
        dev_report = ps2_acceleration_task(dev_report);
    }

    // Buttons
    uint8_t raw_buttons   = b0 & 0x07;
    uint8_t buttons       = raw_buttons | (dev->prev_buttons & raw_buttons);
    dev->prev_buttons     = raw_buttons;
    dev->held_buttons     = buttons;
    ps2_buttons_state    |= buttons;

    // Merge
    mouse_report->x = (int8_t)CLAMP((int16_t)mouse_report->x + dev_report.x, -127, 127);
    mouse_report->y = (int8_t)CLAMP((int16_t)mouse_report->y + dev_report.y, -127, 127);
    mouse_report->buttons |= buttons;

    return true;
}

report_mouse_t pointing_device_task_kb(report_mouse_t mouse_report) {
    ps2_buttons_state = 0;

    for (uint8_t i = 0; i < PS2_NUM_DEVICES; i++) {
        ps2_process_device(i, &mouse_report);
    }

    return pointing_device_task_user(mouse_report);
}
/* ============================================================
 * Matrix hooks

void matrix_init_custom(void) {
    ps2_stm32_init();
}

bool matrix_scan_custom(void) {
    ps2_stm32_scan();
    return false;
}
 * ============================================================ */
