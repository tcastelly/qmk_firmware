#include "timer.h"
#include "matrix.h"
#include "debug.h"
#include "wait.h"
#include "print.h"
#include "platforms/chibios/gpio.h"
#include "i2c_master.h"

#define MCP23017_I2C_ADDRESS 0x20
#define MCP23017_IODIR_A 0x00
#define MCP23017_IODIR_B 0x01
#define MCP23017_GPIO_A  0x12
#define MCP23017_GPIO_B  0x13
#define MCP23017_IOCON_BANK0 0x0A
#define MCP23017_IOCON_BANK1 0x05

#ifndef MCP21017_I2C_TIMEOUT
#    define MCP21017_I2C_TIMEOUT 100
#endif
#ifndef CUSTOM_MATRIX_DEBUG_INTERVAL
#    define CUSTOM_MATRIX_DEBUG_INTERVAL 500
#endif

// MCU側（右半分）
static const ioline_t RIGHT_ROWS[] = { A0, A1, A2, A3 };
static const ioline_t RIGHT_COLS[] = { B0, B1, B2, B3, B4, B5 };

// MCP23017側（左半分）
static const uint8_t LEFT_COL_BITS[] = { 1 << 0, 1 << 1, 1 << 2, 1 << 3, 1 << 4, 1 << 5 }; // GPA0–GPA5
static const uint8_t LEFT_ROW_BITS[] = { 1 << 0, 1 << 1, 1 << 2, 1 << 3 };                 // GPB0–GPB3

static uint16_t d_timer = 0;

void matrix_init_custom(void) {
    i2c_init();
    wait_ms(10);

    // MCU側のMatrixのピンの方向設定
    for (uint8_t i = 0; i < ARRAY_SIZE(RIGHT_ROWS); i++) {
        gpio_set_pin_input_high(RIGHT_ROWS[i]); // ROW: input (external pull-up)
    }
    for (uint8_t i = 0; i < ARRAY_SIZE(RIGHT_COLS); i++) {
        gpio_set_pin_output_push_pull(RIGHT_COLS[i]);
        gpio_write_pin_high(RIGHT_COLS[i]); // COL: idle HIGH
    }

    // BANK0 ICON設定（SEQOP=1）
    uint8_t iocon = 0b00100000;
    (void)i2c_write_register(MCP23017_I2C_ADDRESS << 1, MCP23017_IOCON_BANK0, &iocon, 1, MCP21017_I2C_TIMEOUT);
    (void)i2c_write_register(MCP23017_I2C_ADDRESS << 1, MCP23017_IOCON_BANK1, &iocon, 1, MCP21017_I2C_TIMEOUT);

    // GPIOAをOutput（COL）に設定
    uint8_t iodir_a = 0x00;
    (void)i2c_write_register(MCP23017_I2C_ADDRESS << 1, MCP23017_IODIR_A, &iodir_a, 1, MCP21017_I2C_TIMEOUT);

    // GPIOBをInput（ROW）に設定
    uint8_t iodir_b = 0xFF;
    (void)i2c_write_register(MCP23017_I2C_ADDRESS << 1, MCP23017_IODIR_B, &iodir_b, 1, MCP21017_I2C_TIMEOUT);
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool debug = false;
    uint8_t iocon, ioaddr_a, ioaddr_b;

    if (timer_elapsed(d_timer) > CUSTOM_MATRIX_DEBUG_INTERVAL) {
        debug   = true;
        d_timer = timer_read();
    }

    if (debug) {
        dprintf("-- matrix_scan_custom --\n");

        i2c_read_register(MCP23017_I2C_ADDRESS << 1, MCP23017_IOCON_BANK0, &iocon, 1, MCP21017_I2C_TIMEOUT);
        i2c_read_register(MCP23017_I2C_ADDRESS << 1, MCP23017_IODIR_A, &ioaddr_a, 1, MCP21017_I2C_TIMEOUT);
        i2c_read_register(MCP23017_I2C_ADDRESS << 1, MCP23017_IODIR_B, &ioaddr_b, 1, MCP21017_I2C_TIMEOUT);
        dprintf("IOCON:%02X IODIR_A:%02X IODIR_B:%02X\n", iocon, ioaddr_a, ioaddr_b);
    }

    matrix_row_t scanned_matrix[MATRIX_ROWS];
    memset(scanned_matrix, 0, sizeof(scanned_matrix));

    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
        if (col < ARRAY_SIZE(RIGHT_COLS)) {
            // MCU側のMatrix
            for (uint8_t i = 0; i < ARRAY_SIZE(RIGHT_COLS); i++) {
                gpio_write_pin_high(RIGHT_COLS[i]);
            }
            gpio_write_pin_low(RIGHT_COLS[col]); // active LOW

            wait_us(100);

            for (uint8_t row = 0; row < ARRAY_SIZE(RIGHT_ROWS); row++) {
                if (!gpio_read_pin(RIGHT_ROWS[row])) { // active LOW
                    scanned_matrix[row] |= 1 << col;
                }
            }
        } else {
            // MCP23017側のMatrix
            uint8_t col_index = col - ARRAY_SIZE(RIGHT_COLS);

            // COL側、1本だけLOW、他はHIGH
            uint8_t write_buf = 0xFF & ~LEFT_COL_BITS[col_index];
            i2c_status_t status =
                i2c_write_register(MCP23017_I2C_ADDRESS << 1, MCP23017_GPIO_A, &write_buf, 1, MCP21017_I2C_TIMEOUT);

            if (debug) {
                dprintf("write I2C GPIOA status:%d value:0x%02X col:%d\n", status, write_buf, col);
            }

            if (status != I2C_STATUS_SUCCESS) {
                return false;
            }

            // ROW側、読み取り（active LOW）
            uint8_t read_buf = 0xFF;
            status = i2c_read_register(MCP23017_I2C_ADDRESS << 1, MCP23017_GPIO_B, &read_buf, 1, MCP21017_I2C_TIMEOUT);

            if (debug) {
                dprintf("read I2C GPIOB status:%d value:0x%02X col:%d\n", status, read_buf, col);
            }

            if (status != I2C_STATUS_SUCCESS) {
                return false;
            }

            for (uint8_t row = 0; row < ARRAY_SIZE(LEFT_ROW_BITS); row++) {
                if (!(read_buf & LEFT_ROW_BITS[row])) { // active LOW
                    scanned_matrix[row] |= 1 << col;
                }
            }
        }
    }

    bool updated = false;
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        if (current_matrix[row] != scanned_matrix[row]) {
            current_matrix[row] = scanned_matrix[row];
            updated = true;
        }
    }

    if (debug) {
        dprintf("scanned ");
        for (uint8_t r = 0; r < MATRIX_ROWS; r++) {
            dprintf("0x%04X ", scanned_matrix[r]);
        }
        dprintf("\n");
    }

    return updated;
}

