#include "quantum.h"
#include "ps2.h"
#include "print.h"

/* External declarations */
extern void ps2_host_init(void);
extern uint8_t ps2_host_recv(void);
extern bool pbuf_has_data(void);
extern report_mouse_t ps2_mouse_task(report_mouse_t mouse_report);

/* Manual Handshake */
uint8_t sprintek_manual_send(uint8_t data) {
    uint8_t parity = 1;
    setPinOutput(B10); writePinLow(B10);
    wait_us(200);       
    setPinOutput(B11); writePinLow(B11);
    wait_us(10);
    setPinInput(B10);   

    uint32_t timeout = 50000;
    while (readPin(B10) && --timeout);
    if (timeout == 0) return 0xEE; 

    for (uint8_t i = 0; i < 9; i++) {
        uint8_t bit = (i < 8) ? (data >> i) & 1 : parity;
        if (i < 8 && bit) parity ^= 1;
        while (!readPin(B10)); 
        if (bit) setPinInput(B11); else { setPinOutput(B11); writePinLow(B11); }
        while (readPin(B10));  
    }
    setPinInput(B11);
    while (!readPin(B10)); 
    timeout = 1000;
    while (readPin(B11) && --timeout);
    uint8_t ack = (readPin(B11) == 0) ? 0xFA : 0x00;
    while (!readPin(B10)); 
    return ack;
}

void pbuf_pre_alloc(void) {}

void matrix_init_custom(void) {
    palSetPadMode(GPIOB, 10, PAL_MODE_INPUT | PAL_STM32_OTYPE_OPENDRAIN);
    palSetPadMode(GPIOB, 11, PAL_MODE_INPUT | PAL_STM32_OTYPE_OPENDRAIN);
    
    wait_ms(2500); 
    uprintf("SK8707: Resetting...\n");
    sprintek_manual_send(0xFF); 
    wait_ms(1500); 

    ps2_host_init();
    palEnableLineEvent(PAL_LINE(GPIOB, 10), PAL_EVENT_MODE_FALLING_EDGE);

    wait_ms(500); 
    uprintf("SK8707: Clearing BAT...\n");
    while(pbuf_has_data()) { ps2_host_recv(); }

    uprintf("SK8707: Enabling stream...\n");
    sprintek_manual_send(0xF4); 
    uprintf("SK8707: Ready.\n");
}

/* Bridging function for POINTING_DEVICE_DRIVER = custom */
report_mouse_t pointing_device_driver_get_report(report_mouse_t mouse_report) {
    if (pbuf_has_data()) {
        // If we have data but no movement, the driver is rejecting the packets.
        // We call the task and see if it updates the report.
        return ps2_mouse_task(mouse_report);
    }
    return mouse_report;
}

void matrix_scan_custom(void) {
    // EMPTY: Do not call ps2_host_recv here!
    // QMK's core task will handle the buffer via the driver.
}
