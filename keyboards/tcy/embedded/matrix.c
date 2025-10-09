/*
Copyright 2024 Atsushi Morimoto (@74th)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
        gpio_set_pin_input_low(RIGHT_ROWS[i]);
    }
    for (uint8_t i = 0; i < ARRAY_SIZE(RIGHT_COLS); i++) {
        gpio_set_pin_output_push_pull(RIGHT_COLS[i]);
    }

    // BANK0ICON設定（念のため）
    uint8_t iocon = 0x00;
    (void)i2c_write_register(MCP23017_I2C_ADDRESS << 1, MCP23017_IOCON_BANK0, &iocon, 1, MCP21017_I2C_TIMEOUT);
    (void)i2c_write_register(MCP23017_I2C_ADDRESS << 1, MCP23017_IOCON_BANK1, &iocon, 1, MCP21017_I2C_TIMEOUT);

    // GPIOAをInputに設定
    uint8_t iodir_a = 0xff;
    (void)i2c_write_register(MCP23017_I2C_ADDRESS << 1, MCP23017_IODIR_A, &iodir_a, 1, MCP21017_I2C_TIMEOUT);

    // GPIOBをOutputに設定
    uint8_t iodir_b = 0x00;
    (void)i2c_write_register(MCP23017_I2C_ADDRESS << 1, MCP23017_IODIR_B, &iodir_b, 1, MCP21017_I2C_TIMEOUT);
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool debug = false;
    uint8_t iocon, ioaddr_a, ioaddr_b;

    i2c_status_t s1 = I2C_STATUS_SUCCESS, s2 = I2C_STATUS_SUCCESS, s3 = I2C_STATUS_SUCCESS;
    (void)s1;
    (void)s2;
    (void)s3;


    if (timer_elapsed(d_timer) > CUSTOM_MATRIX_DEBUG_INTERVAL) {
        debug   = true;
        d_timer = timer_read();
    }


    if (debug) {
        dprintf("-- matrix_scan_custom --\n");

        // MCP23017の設定の確認
        s1 = i2c_read_register(MCP23017_I2C_ADDRESS << 1, MCP23017_IOCON_BANK0, &iocon, 1, MCP21017_I2C_TIMEOUT);
        s2 = i2c_read_register(MCP23017_I2C_ADDRESS << 1, MCP23017_IODIR_A, &ioaddr_a, 1, MCP21017_I2C_TIMEOUT);
        s3 = i2c_read_register(MCP23017_I2C_ADDRESS << 1, MCP23017_IODIR_B, &ioaddr_b, 1, MCP21017_I2C_TIMEOUT);
        dprintf("IOCON:%02X IODIR_A:%02X IODIR_B:%02X (status:%d, %d, %d)\n", iocon, ioaddr_a, ioaddr_b, s1, s2, s3);
    }

    matrix_row_t scanned_matrix[MATRIX_ROWS];
    memset(scanned_matrix, 0, sizeof(scanned_matrix));

    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
        if (col < ARRAY_SIZE(RIGHT_COLS)) {
            // MCU側のMatrix
            for (uint8_t i = 0; i < ARRAY_SIZE(RIGHT_COLS); i++) {
                if (i == col) {
                    gpio_write_pin_high(RIGHT_COLS[i]);
                } else {
                    gpio_write_pin_low(RIGHT_COLS[i]);
                }
            }

            wait_us(100);

            for (uint8_t row = 0; row < ARRAY_SIZE(RIGHT_ROWS); row++) {
                bool v = gpio_read_pin(RIGHT_ROWS[row]);
                if (v) {
                    scanned_matrix[row] |= 1 << col;
                }
            }
        } else {
            // MCP23017側のMatrix
            // COL側、特定のCOLだけHIGHにして、他はLOWにする
            uint8_t write_buf = LEFT_COL_BITS[col - ARRAY_SIZE(RIGHT_COLS)];
            i2c_status_t status = i2c_write_register(MCP23017_I2C_ADDRESS << 1, MCP23017_GPIO_A, &write_buf, 1, MCP21017_I2C_TIMEOUT);
            if (debug) {
                dprintf("write I2C GPIOB status:%d value:0x%02X col:%d\n", status, write_buf, col);
            }

            if (status != I2C_STATUS_SUCCESS) {
                return false;
            }

            // ROW側、読み取り
            uint8_t read_buf = 0;
            status = i2c_read_register(MCP23017_I2C_ADDRESS << 1, MCP23017_GPIO_B, &read_buf, 1, MCP21017_I2C_TIMEOUT);
            if (debug) {
                dprintf("read I2C GPIOA status:%d value:0x%02X col:%d\n", status, read_buf, col);
            }

            if (status != I2C_STATUS_SUCCESS) {
                return false;
            }

            for (uint8_t row = 0; row < ARRAY_SIZE(LEFT_ROW_BITS); row++) {
                if (read_buf & LEFT_ROW_BITS[row]) {
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

