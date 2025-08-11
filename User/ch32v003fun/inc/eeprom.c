/*
 * eeprom.c
 *
 *  Created on: 29 Jul 2025
 *      Author: azati

*/

#include "eeprom.h"
#include "ch32v003fun/i2c_tx.h"


static void EEPROM_wait_ready(void) {
    uint8_t retries = 20; // 20ms timeout

    while(retries--) {
        I2C_start(EEPROM_I2C_ADDR << 1);
        if(I2C1->STAR1 & I2C_STAR1_SB) {
            I2C_stop();
            return;
        }
        Delay_Ms(1);
    }
    I2C_stop();
}

void EEPROM_init(void) {
    I2C_init();
}

uint8_t EEPROM_write(uint16_t addr, uint8_t data) {
    if(addr > EEPROM_MAX_ADDR) return 0;

    I2C_start(EEPROM_I2C_ADDR << 1);
    I2C_write(addr >> 8);
    I2C_write(addr & 0xFF);
    I2C_write(data);
    I2C_stop();

    EEPROM_wait_ready();
    return 1;
}

uint8_t EEPROM_read(uint16_t addr) {
    uint8_t data = 0;

    if(addr > EEPROM_MAX_ADDR) return 0;

    I2C_start(EEPROM_I2C_ADDR << 1);
    I2C_write(addr >> 8);
    I2C_write(addr & 0xFF);

    I2C_start((EEPROM_I2C_ADDR << 1) | 0x01);
    data = I2C_read_NACK();
    I2C_stop();

    return data;
}


