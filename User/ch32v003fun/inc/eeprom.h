/*
 * eeprom.h
 *
 *  Created on: 29 Jul 2025
 *      Author: azati
*/

#ifndef USER_CH32V003FUN_INC_EEPROM_H_
#define USER_CH32V003FUN_INC_EEPROM_H_

#include <stdint.h>

#define EEPROM_I2C_ADDR    0x52    // AT24C256 base address
#define EEPROM_PAGE_SIZE   64      // AT24C256 page size
#define EEPROM_MAX_ADDR    0x7FFF  // 32KB capacity

void EEPROM_init(void);
uint8_t EEPROM_write(uint16_t addr, uint8_t data);
uint8_t EEPROM_read(uint16_t addr);
//void EEPROM_write_page(uint16_t addr, uint8_t* data, uint8_t len);
//void EEPROM_read_seq(uint16_t addr, uint8_t* buf, uint16_t len);

#endif /* USER_CH32V003FUN_INC_EEPROM_H_ */

