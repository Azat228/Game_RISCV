#ifndef EEPROM_STORAGE_H
#define EEPROM_STORAGE_H

#include <stdint.h>
#include <stdbool.h>
#include "./ch32v003fun/ch32v003_i2c.h"
#include "./data/colors.h"

// EEPROM Configuration
#define EEPROM_ADDR 0x52
#define PAGE_SIZE 64
#define MATRIX_HORI 16
#define DELAY_MS 1000

// Memory Layout Constants
#define INIT_STATUS_ADDR_BEGIN 0
#define INIT_STATUS_ADDR_END 7
#define INIT_STATUS_REG_SIZE 8
#define INIT_STATUS_FORMAT "  %c "
extern const uint8_t init_status_data[INIT_STATUS_REG_SIZE];

#define PAGE_STATUS_ADDR_BEGIN 8
#define PAGE_STATUS_ADDR_END 511
#define PAGE_STATUS_REG_SIZE 504

#define PAINT_ADDR_BEGIN 8
#define SIZE_OF_PAINT_DATA (3 * NUM_LEDS)  // 192 bytes
#define SIZE_OF_PAINT_DATA_AS_PAGE (SIZE_OF_PAINT_DATA / PAGE_SIZE)  // 3 pages
#define PAINT_ADDR_END (PAINT_ADDR_BEGIN + 8 * SIZE_OF_PAINT_DATA_AS_PAGE - 1)

#define OPCODE_ADDR_BEGIN 54
#define SIZE_OF_OPCODE_DATA 64
#define SIZE_OF_OPCODE_DATA_AS_PAGE (SIZE_OF_OPCODE_DATA / PAGE_SIZE)  // 1 page
#define OPCODE_ADDR_END 61

// Storage Management Functions
void init_storage(void);
uint8_t is_storage_initialized(void);
void reset_storage(void);
void print_status_storage(void);

// Page Management Functions
void set_page_status(uint16_t page_no, uint8_t status);
uint8_t is_page_used(uint16_t page_no);
uint16_t calculate_page_no(uint16_t paint_no, uint8_t is_icon);

// Data Operations
void save_paint(uint16_t paint_no, color_t *data, uint8_t is_icon);
void load_paint(uint16_t paint_no, color_t *data, uint8_t is_icon);

#endif // EEPROM_STORAGE_H
