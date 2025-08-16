#ifndef NAME_FUNC_H
#define NAME_FUNC_H

#include <stdint.h>
#include <stdbool.h>
#include "./data/colors.h"
#include "./ch32v003fun/ws2812b_simple.h"

// LED configuration
#define LED_PINS GPIOA, 2

// Letter definitions
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
#define LETTER_V  21
#define LETTER_W  22
#define LETTER_X  23
#define LETTER_Y  24
#define LETTER_Z  25

/**
 * @brief Display a single letter on the LED matrix
 *
 * @param letter_idx Index of the letter (LETTER_A to LETTER_Z)
 * @param color Color to display the letter in
 * @param delay_ms Delay after displaying (in milliseconds)
 */
void display_letter(uint8_t letter_idx, color_t color, int delay_ms);

/**
 * @brief Display a sequence of letters as a message
 *
 * @param letters Array of letter indices to display
 * @param count Number of letters in the array
 * @param color Color to display the letters in
 * @param delay_ms Delay between letters (in milliseconds)
 */
void display_full_message(const uint8_t* letters, uint8_t count, color_t color, uint16_t delay_ms);

/**
 * @brief Display "NAME" animation for name selection menu
 */
void choose_your_name(void);

#endif // LETTER_DISPLAY_H
