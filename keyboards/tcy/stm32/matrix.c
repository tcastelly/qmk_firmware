#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "timer.h"
#include "matrix.h"
#include "debug.h"
#include "wait.h"
#include "print.h"
#include "platforms/chibios/gpio.h"
#include "i2c2_handler.h" // Switched from i2c_master.h to your custom handler

#define MCP23017_I2C_ADDRESS 0x20
#define MCP23017_IODIR_A 0x00 
#define MCP23017_IODIR_B 0x01 
#define MCP23017_GPPU_A  0x0C 
#define MCP23017_GPIO_A  0x12 
#define MCP23017_GPIO_B  0x13 
#define MCP23017_IOCON   0x0A

static const ioline_t RIGHT_ROWS[] = MATRIX_ROW_PINS_MCU;
static const ioline_t RIGHT_COLS[] = MATRIX_COL_PINS_MCU;
static bool matrix_initialized = false;

void matrix_init_custom(void) {
    wait_ms(1000); 
    i2c2_init_custom(); // Initialize I2C2 (B10/B11) instead of I2C1
    wait_ms(100);

    // 1. Configure MCP23017 (Left Side) - Active High for 10k resistors
    uint8_t iocon = 0b00100000;
    // Note: Addresses are shifted left for the 7-bit + R/W format
    if (i2c2_write_reg(MCP23017_I2C_ADDRESS << 1, MCP23017_IOCON, &iocon, 1, 10) == I2C_STATUS_SUCCESS) {
        // Port A (Cols) = Outputs, Port B (Rows) = Inputs
        uint8_t iodir_a = 0x00; 
        uint8_t iodir_b = 0xFF; 
        i2c2_write_reg(MCP23017_I2C_ADDRESS << 1, MCP23017_IODIR_A, &iodir_a, 1, 10);
        i2c2_write_reg(MCP23017_I2C_ADDRESS << 1, MCP23017_IODIR_B, &iodir_b, 1, 10);
        
        // No pull-ups (external 10k resistors used)
        uint8_t gppu = 0x00; 
        i2c2_write_reg(MCP23017_I2C_ADDRESS << 1, MCP23017_GPPU_A, &gppu, 1, 10);
        i2c2_write_reg(MCP23017_I2C_ADDRESS << 1, 0x0D, &gppu, 1, 10);
        
        matrix_initialized = true;
        print("MCP23017 on I2C2 Init: Success\n");
    }

    // 2. Configure MCU Pins (Right Side) - Active Low
    for (uint8_t i = 0; i < 4; i++) {
        palSetLineMode(RIGHT_ROWS[i], PAL_MODE_OUTPUT_PUSHPULL);
        palSetLine(RIGHT_ROWS[i]); // Idle HIGH
    }
    for (uint8_t i = 0; i < 6; i++) {
        palSetLineMode(RIGHT_COLS[i], PAL_MODE_INPUT_PULLUP); // Internal Pull-ups
    }
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    if (!matrix_initialized) return false;

    matrix_row_t scanned_matrix[MATRIX_ROWS]; 
    memset(scanned_matrix, 0, sizeof(scanned_matrix));

    // --- SCAN RIGHT SIDE (MCU) - Direct GPIO ---
    for (uint8_t row = 0; row < 4; row++) {
        palClearLine(RIGHT_ROWS[row]); // Drive Row LOW
        wait_us(30); 
        for (uint8_t col = 0; col < 6; col++) {
            if (!palReadLine(RIGHT_COLS[col])) { // If Col is LOW, key is pressed
                scanned_matrix[row] |= (1 << (col + 6));
            }
        }
        palSetLine(RIGHT_ROWS[row]); // Return Row HIGH
    }

    // --- SCAN LEFT SIDE (MCP23017) - Via I2C2 ---
    for (uint8_t col = 0; col < 6; col++) {
        uint8_t col_mask = (1 << col); 
        // Write to GPIO_A to set the column high
        if (i2c2_write_reg(MCP23017_I2C_ADDRESS << 1, MCP23017_GPIO_A, &col_mask, 1, 10) == I2C_STATUS_SUCCESS) {
            uint8_t row_data = 0;
            // Read from GPIO_B to check which rows are active
            if (i2c2_read_reg(MCP23017_I2C_ADDRESS << 1, MCP23017_GPIO_B, &row_data, 1, 10) == I2C_STATUS_SUCCESS) {
                for (uint8_t row = 0; row < 4; row++) {
                    if (row_data & (1 << row)) { 
                        scanned_matrix[row] |= (1 << col);
                    }
                }
            }
        }
        uint8_t low = 0x00;
        i2c2_write_reg(MCP23017_I2C_ADDRESS << 1, MCP23017_GPIO_A, &low, 1, 10);
    }

    // Apply changes to the matrix
    bool updated = false;
    for (uint8_t r = 0; r < MATRIX_ROWS; r++) {
        if (current_matrix[r] != scanned_matrix[r]) {
            current_matrix[r] = scanned_matrix[r];
            updated = true;
        }
    }
    return updated;
}
