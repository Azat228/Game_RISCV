#define CH32V003_I2C_IMPLEMENTATION
#define WS2812BSIMPLE_IMPLEMENTATION
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./ch32v003fun/ch32v003_i2c.h"
#include "./data/colors.h"
#include "./ch32v003fun/driver.h"
#include "./data/fonts.h"
#include "./data/music.h"
#include "./ch32v003fun/ws2812b_simple.h"
#define LED_PINS GPIOA, 2
#define EEPROM_ADDR 0x52 // obtained from i2c_scan(), before shifting by 1 bit
#define page_size 64    // range of byte that stores status of page[x]
#define opcode_size 28    // range of byte that stores opcodes
#define init_status_addr_begin 0
#define init_status_addr_end 7
#define init_status_reg_size (init_status_addr_end - init_status_addr_begin + 1) // size  = 8
#define init_status_format "  %c "
#define init_status_data (uint8_t *)"IL000001"
#define page_status_addr_begin 8 // page 8
#define page_status_addr_end 511 // page 511
#define page_status_reg_size (page_status_addr_end - page_status_addr_begin + 1) // page size = 504
#define paint_addr_begin 8 //paint page start at 8
#define sizeof_paint_data (3 * NUM_LEDS) //paint page size = 192
#define sizeof_paint_data_aspage (sizeof_paint_data / page_size) // no. of paint page = 3
#define paint_addr_end (paint_addr_begin + 8 * sizeof_paint_data_aspage - 1) // paint page end at addr = 31
#define paint_page_no (0 * sizeof_paint_data_aspage) //no = 0
#define paint_page_no_max (8 * sizeof_paint_data_aspage) //size = 24
#define num_paint_saves (paint_page_no_max / sizeof_paint_data_aspage) //size = 8
#define opcode_addr_begin (paint_addr_end + paint_page_no_max - 1) //addr = 54
#define sizeof_opcode_data 64 //size = 64
#define sizeof_opcode_data_aspage (sizeof_opcode_data / page_size) // size = 1
#define opcode_addr_end (opcode_addr_begin + 8 * sizeof_paint_data_aspage - 1) //addr = 61
#define opcode_page_no (0 * sizeof_opcode_data_aspage) //no = 8
#define opcode_page_no_max (8 * sizeof_opcode_data_aspage) //size = 8
#define matrix_hori 16
#define app_icon_page_no (0 * sizeof_paint_data_aspage) //no = 0
#define app_icon_page_no_max (8 * sizeof_paint_data_aspage) //size = 24
#define delay 1000
// Storage functions
void init_storage(void);
void save_paint(uint16_t paint_no, color_t *data, uint8_t is_icon);
void load_paint(uint16_t paint_no, color_t *data, uint8_t is_icon);
void set_page_status(uint16_t page_no, uint8_t status);
void reset_storage(void);
void print_status_storage(void);
uint8_t is_page_used(uint16_t page_no);
uint8_t is_storage_initialized(void);
uint16_t calculate_page_no(uint16_t paint_no, uint8_t is_icon);
void init_storage(void) {
    if (!is_storage_initialized()) {
        reset_storage();
        printf("Storage initialized\n");
    }
    else {
        printf("Storage already initialized\n");
    }
}

uint8_t is_storage_initialized(void) {
    uint8_t data[init_status_reg_size];
    i2c_read(EEPROM_ADDR, init_status_addr_begin, I2C_REGADDR_2B, data, init_status_reg_size);
    for (uint8_t i = 0; i < init_status_reg_size; i++) {
        if (data[i] != *(init_status_data + i)) {
            return 0;
        }
    }
    return 1;
}

void reset_storage(void) {
    i2c_write(EEPROM_ADDR, init_status_addr_begin, I2C_REGADDR_2B, init_status_data,
        init_status_reg_size);
    Delay_Ms(3);
    for (uint16_t addr = page_status_addr_begin;
         addr < page_status_addr_begin + page_status_reg_size; addr++) {
        i2c_write(EEPROM_ADDR, addr, I2C_REGADDR_2B, (uint8_t[]){0}, sizeof(uint8_t));
        Delay_Ms(3);
    }
    printf("Storage reset\n");
}

void print_status_storage(void) {
    printf("Status storage data:\n");
    for (uint16_t addr = init_status_addr_begin;
         addr < init_status_addr_begin + init_status_reg_size; addr++) {
        uint8_t data = 0;
        i2c_read(EEPROM_ADDR, addr, I2C_REGADDR_2B, &data, sizeof(data));
        printf(" %d: ", addr);
        printf(init_status_format, data);
    }
    printf("\n");
    for (uint16_t addr = page_status_addr_begin;
         addr < page_status_addr_begin + page_status_reg_size; addr++) {
        uint8_t data = 0;
        i2c_read(EEPROM_ADDR, addr, I2C_REGADDR_2B, &data, sizeof(data));
        if (data) {
            printf("%d ", addr);
        }
        else {
            printf("    ");
        }
        if ((addr + 1) % matrix_hori == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

void set_page_status(uint16_t page_no, uint8_t status) {
    if (status > 1) {
        printf("Invalid status %d\n", status);
        printf("DEBUG: %d\n", __LINE__);
        while (1)
            ;
    }
    if (page_no < page_status_addr_begin || page_no > page_status_addr_end) {
        printf("Invalid page number %d\n", page_no);
        printf("DEBUG: %d\n", __LINE__);
        while (1)
            ;
    }
    i2c_write(EEPROM_ADDR, page_no, I2C_REGADDR_2B, &status, sizeof(status));
    Delay_Ms(3);
    //printf("Page %d status set to %d\n", page_no, status);
}

uint8_t is_page_used(uint16_t page_no) {
    if (page_no < page_status_addr_begin || page_no > page_status_addr_end) {
        printf("Invalid page number %d\n", page_no);
        printf("DEBUG: %d\n", __LINE__);
        while (1);
    }
    uint8_t data = 0;
    i2c_read(EEPROM_ADDR, page_no, I2C_REGADDR_2B, &data, sizeof(data));
    //printf("Page %d is %s\n", page_no, data ? "used" : "empty");
    return data;
}

uint16_t calculate_page_no(uint16_t paint_no, uint8_t is_icon) {
    if (is_icon==1) {
        return (paint_no + app_icon_page_no) * sizeof_paint_data_aspage +
               paint_addr_begin;
    }
    else {
        return paint_no * sizeof_opcode_data_aspage +
               opcode_addr_begin;
    }
}
void save_paint(uint16_t paint_no, color_t * data, uint8_t is_icon) {
    if (paint_no < 0 || paint_no > paint_addr_end) {
        printf("Invalid paint number %d\n", paint_no);
        printf("DEBUG: %d\n", __LINE__);
        while (1)
            ;
    }
    uint16_t page_no_start = calculate_page_no(paint_no, is_icon);
    for (uint16_t i = page_no_start; i < page_no_start + sizeof_paint_data_aspage; i++) {
        if (is_page_used(i)) {
            printf("Paint %d already used, overwriting\n", paint_no);
            Delay_Ms(500);
        }
        set_page_status(i, 1);
    }
    i2c_result_e err = i2c_write_pages(EEPROM_ADDR, page_no_start * page_size,
        I2C_REGADDR_2B, (uint8_t *)data, sizeof_paint_data);
    printf("Save paint result: %d\n", err);
    Delay_Ms(3);
    printf("Paint %d saved\n", paint_no);
}
void load_paint(uint16_t paint_no, color_t * data, uint8_t is_icon) {
    if (paint_no < 0 || paint_no > paint_addr_end) {
        printf("Invalid paint number %d\n", paint_no);
        printf("DEBUG: %d\n", __LINE__);
        while (1)
            ;
    }
    uint16_t page_no_start = calculate_page_no(paint_no, is_icon);
    printf("Loading paint_no %d from page %d, is_icon: %d\n", paint_no, page_no_start,
        is_icon);
    if (!is_page_used(page_no_start)) {
        printf("Paint %d not found\n", paint_no);
        printf("DEBUG: %d\n", __LINE__);
        while (1)
            ;
    }
    i2c_result_e err = i2c_read_pages(EEPROM_ADDR, page_no_start * page_size,
        I2C_REGADDR_2B, (uint8_t *)data, sizeof_paint_data);
    printf("Load paint result: %d\n", err);
    Delay_Ms(3);
    printf("Paint %d loaded\n", paint_no);
}
// Show all scores with flashing animation

//
