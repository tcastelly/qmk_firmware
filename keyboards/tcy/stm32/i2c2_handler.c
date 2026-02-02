#include "hal.h"
#include "i2c2_handler.h"

// Configuration for I2CD2
static const I2CConfig i2ccfg2 = {
    .timingr = STM32_TIMINGR_PRESC(2U) |
               STM32_TIMINGR_SCLDEL(1U) |
               STM32_TIMINGR_SDADEL(0U) |
               STM32_TIMINGR_SCLH(9U) |
               STM32_TIMINGR_SCLL(26U),
    .cr1 = 0,
    .cr2 = 0
};

void i2c2_init_custom(void) {
    // Setup pins for I2CD2: B10=SCL, B11=SDA
    palSetPadMode(GPIOB, 10, PAL_MODE_ALTERNATE(1) | PAL_STM32_OTYPE_OPENDRAIN);
    palSetPadMode(GPIOB, 11, PAL_MODE_ALTERNATE(1) | PAL_STM32_OTYPE_OPENDRAIN);
    i2cStart(&I2CD2, &i2ccfg2);
}

i2c_status_t i2c2_write_reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t* data, uint16_t len, uint16_t timeout) {
    uint8_t txbuf[len + 1];
    txbuf[0] = reg_addr;
    for(int i = 0; i < len; i++) txbuf[i+1] = data[i];

    msg_t status = i2cMasterTransmitTimeout(&I2CD2, (dev_addr >> 1), txbuf, len + 1, NULL, 0, TIME_MS2I(timeout));
    return (status == MSG_OK) ? I2C_STATUS_SUCCESS : I2C_STATUS_ERROR;
}

i2c_status_t i2c2_read_reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t* data, uint16_t len, uint16_t timeout) {
    msg_t status = i2cMasterTransmitTimeout(&I2CD2, (dev_addr >> 1), &reg_addr, 1, data, len, TIME_MS2I(timeout));
    return (status == MSG_OK) ? I2C_STATUS_SUCCESS : I2C_STATUS_ERROR;
}
