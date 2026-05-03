#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "timer.h"
#include "matrix.h"
#include "debug.h"
#include "wait.h"
#include "print.h"
#include "platforms/chibios/gpio.h"
#include "i2c2_handler.h"

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

// Global variable to store the state of the GPA6 mouse button
uint8_t mcp_click_state = 0;

void mcp_matrix_init_custom(void) {
    wait_ms(500); 
    i2c2_init_custom(); // Initialize I2C2 (B10/B11) instead of I2C1
    wait_ms(100);

    // 1. Configure MCP23017 (Left Side)
    uint8_t iocon = 0b00100000;
    if (i2c2_write_reg(MCP23017_I2C_ADDRESS << 1, MCP23017_IOCON, &iocon, 1, 10) == I2C_STATUS_SUCCESS) {
        
        // Port A: Pins 0-5 are Outputs (Cols), Pin 6 is Input (Mouse Switch)
        // 0x40 = 0b01000000
        uint8_t iodir_a = 0x40; 
        uint8_t iodir_b = 0xFF; // Port B (Rows) = Inputs
        i2c2_write_reg(MCP23017_I2C_ADDRESS << 1, MCP23017_IODIR_A, &iodir_a, 1, 10);
        i2c2_write_reg(MCP23017_I2C_ADDRESS << 1, MCP23017_IODIR_B, &iodir_b, 1, 10);
        
        // Disable internal pull-ups for Port A (GPA6 uses external 10k pull-down)
        uint8_t gppu_a = 0x00; 
        i2c2_write_reg(MCP23017_I2C_ADDRESS << 1, MCP23017_GPPU_A, &gppu_a, 1, 10);
        
        // Port B uses external 10k resistors (no internal pull-ups)
        uint8_t gppu_b = 0x00; 
        i2c2_write_reg(MCP23017_I2C_ADDRESS << 1, 0x0D, &gppu_b, 1, 10);
        
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

bool mcp_matrix_scan_custom(matrix_row_t current_matrix[]) {
    if (!matrix_initialized) return false;

    matrix_row_t scanned_matrix[MATRIX_ROWS]; 
    memset(scanned_matrix, 0, sizeof(scanned_matrix));

    // --- CATCH THE CLICK FROM GPA6 (Active High via VCC) ---
    uint8_t port_a_read = 0;
    if (i2c2_read_reg(MCP23017_I2C_ADDRESS << 1, MCP23017_GPIO_A, &port_a_read, 1, 10) == I2C_STATUS_SUCCESS) {
        // Pressed = pin sees VCC = bit is 1. If high, mcp_click_state = 1.
        mcp_click_state = (port_a_read & (1 << 6)) ? 1 : 0;
    }

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
        // Ensure we don't accidentally drive GPA6 High while resetting columns
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
