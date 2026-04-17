#include "pmw3360.h"
#include "quantum.h"
#include "spi_master.h"
#include "wait.h"
#include "print.h"

#ifndef PMW33XX_CS_PIN
#    define PMW33XX_CS_PIN A4
#endif

#ifndef constrain
#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
#endif

// Replace the content inside these brackets with the 4096 bytes 
// from the official QMK pmw3360_srom_v04.h file
const uint8_t pmw3360_srom[] PROGMEM = {
    0x01, 0x04, 0xbf // ... Paste the rest of the 4096 bytes here
};

void pmw3360_write(uint8_t reg, uint8_t data) {
    spi_start(PMW33XX_CS_PIN, false, 3, 64);
    uint8_t addr = reg | 0x80; 
    spi_transmit(&addr, 1);
    spi_transmit(&data, 1);
    wait_us(35); 
    spi_stop();
}

uint8_t pmw3360_read(uint8_t reg) {
    spi_start(PMW33XX_CS_PIN, false, 3, 64);
    spi_transmit(&reg, 1);
    wait_us(35); 
    uint8_t data = 0;
    spi_receive(&data, 1);
    spi_stop();
    return data;
}

void pmw3360_init(void) {
    // 1. Hardware Power-Up
    writePinHigh(PMW33XX_CS_PIN);
    wait_ms(1);
    writePinLow(PMW33XX_CS_PIN);
    wait_ms(1);
    writePinHigh(PMW33XX_CS_PIN);
    wait_ms(50);

    // 2. Reset
    pmw3360_write(0x3A, 0x5A); 
    wait_ms(20);

    // 3. SROM Upload Sequence
    pmw3360_write(0x10, 0x00); // Disable Rest
    pmw3360_write(0x13, 0x04); 
    wait_ms(1);
    pmw3360_write(0x13, 0x18); 

    spi_start(PMW33XX_CS_PIN, false, 3, 64);
    uint8_t addr = 0x62 | 0x80; 
    spi_transmit(&addr, 1);
    wait_us(15);

    for (uint16_t i = 0; i < sizeof(pmw3360_srom); i++) {
        uint8_t b = pgm_read_byte(&pmw3360_srom[i]);
        spi_transmit(&b, 1);
        wait_us(15);
    }
    spi_stop();
    wait_ms(2);

    // 4. Verification
    uint8_t srom_id = pmw3360_read(0x1F);
    if (srom_id > 0) {
        uprintf("PMW3360: SROM Upload Success! ID: 0x%02X\n", srom_id);
    } else {
        uprintf("PMW3360: SROM Upload FAILED!\n");
    }

    pmw3360_write(0x10, 0x00);
}

void pmw3360_set_cpi(uint16_t cpi) {
    uint8_t val = (constrain(cpi, 100, 12000) / 100) - 1;
    pmw3360_write(0x3F, val);
}

void pmw3360_read_burst(int16_t *dx, int16_t *dy) {
    uint8_t addr = 0x50;
    uint8_t data[12];

    spi_start(PMW33XX_CS_PIN, false, 3, 64);
    spi_transmit(&addr, 1);
    wait_us(35);
    spi_receive(data, 12);
    spi_stop();

    // NEW DEBUG: Print the first byte (Motion Status) every time
    // If this is always 0x00, the sensor is not "seeing" the ball move.
    static uint16_t log_limiter = 0;
    if (timer_elapsed(log_limiter) > 500) {
        uprintf("PMW Burst Status: 0x%02X | SQUAL: %d\n", data[0], data[6]);
        log_limiter = timer_read();
    }

    if (data[0] & 0x80) { // Motion bit
        *dx = (int16_t)(data[2] | (data[3] << 8));
        *dy = (int16_t)(data[4] | (data[5] << 8));
    } else {
        *dx = 0;
        *dy = 0;
    }
}
