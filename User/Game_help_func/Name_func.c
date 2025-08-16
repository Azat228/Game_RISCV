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
#include "Game_help_func/Colours_predefined.h"
#define LED_PINS GPIOA, 2
#define LETTER_A  0
#define LETTER_B  1
#define LETTER_C  2
#define LETTER_D  3
#define LETTER_E  4
#define LETTER_F  5
#define LETTER_G  6
#define LETTER_H  7
#define LETTER_I  8
#define LETTER_J  9
#define LETTER_K  10
#define LETTER_L  11
#define LETTER_M  12
#define LETTER_N  13
#define LETTER_O  14
#define LETTER_P  15
#define LETTER_Q  16
#define LETTER_R  17
#define LETTER_S  18
#define LETTER_T  19
#define LETTER_U  20
#define LETTER_v  21
#define LETTER_W  22
#define LETTER_X  23
#define LETTER_Y  24
#define LETTER_Z  25
void display_letter(uint8_t letter_idx, color_t color, int delay_ms) {
    Letter_draw(Letter_List[letter_idx], color, 0);
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS*3);
    Delay_Ms(delay_ms);
    clear();
}
void display_full_message(const uint8_t* letters, uint8_t count, color_t color, uint16_t delay_ms) {
    for (uint8_t i = 0; i < count; i++) {
        Letter_draw(Letter_List[letters[i]], letters_color[letters[i]], 0);
        WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS*3);
        Delay_Ms(delay_ms);
        clear();
    }
}
void choose_your_name(void) {
    // Define "CHOOSE YOUR NAME" letter sequence
    const uint8_t message_letters[] = {
        LETTER_N, LETTER_A, LETTER_M, LETTER_E  // NAME
    };

    // Display each letter with animation
    for (int i = 0; i < sizeof(message_letters); i++) {
        if (message_letters[i] == 255) {
            // Handle space (clear screen for longer)
            clear();
            Delay_Ms(300);  // Pause for spaces
        } else {
            display_letter(message_letters[i],letters_color[message_letters[i]],500);
        }
    }
}
