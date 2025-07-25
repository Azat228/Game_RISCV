#pragma once
#include <ch32v003fun/ch32v003fun.h>
#include "funconfig.h"
#include "../data/buttons.h"
#include "ch32v003_GPIO_branchless.h"

#ifdef WS2812BSIMPLE_IMPLEMENTATION
#if FUNCONF_SYSTICK_USE_HCLK != 1
#error WS2812B Driver Requires FUNCONF_SYSTICK_USE_HCLK
#endif

// README: ACT Button is connected to PA2 !!!!!
void gpio_init_act(void) {
    // Enable GPIOA
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOA;
    // PA2 is input
    GPIOA->CFGLR &= ~(0xF << (4 * 2)); // CNF = 01: Floating input, MODE = 00: Input
	GPIOA->CFGLR |= (GPIO_Speed_In | GPIO_CNF_IN_PUPD)<<(4*2);
    GPIOA->BSHR = ((uint32_t)1<<2);
}
// CHANGE THIS TOO IF YOU DONT USE PA2 FOR ACT BUTTON
uint32_t gpio_act_pressed(void) {
    // check the value of pa2 is low
    //return ((GPIOA->INDR & (1 << 2)) == 0);
    return !(GPIOA->INDR >> 2);
}
void ADC_init(void) {
    // gpio_init_act();
    GPIO_ADCinit();
}

uint16_t ADC_read(void) {
    return GPIO_analogRead(GPIO_Ain7_D4);
}

uint16_t ADC_read_pad(void) {
    return GPIO_analogRead(GPIO_Ain2_C4);
}

uint16_t ADC_read_smallboard(void) {
    //return GPIO_analogRead(GPIO_Ain4_D3);
    return GPIO_analogRead(GPIO_Ain2_C4);
}

#include <stdio.h>
uint16_t multiple_ADC_reads(uint16_t (*matrix)(void), uint8_t samples) {
    uint64_t adc = 0;
    for (int8_t i = 0; i < samples; i++) {
        uint16_t _adc = matrix();
        adc += _adc;
        Delay_Ms(1);
    }
    adc /= samples;
    return adc;
}

#define no_button_pressed -1
int8_t matrix_pressed(uint16_t (*matrix)(void)) {
    const int8_t samples = 5;
    uint16_t adc = multiple_ADC_reads(matrix, samples);
    for (int8_t i = 0; i < NUM_BUTTONS; i++) {
        int deviation = abs(adc - buttons[i]);
        if (deviation <= BUTTON_DEVIATION) {
            return i;
        }
    }
    return no_button_pressed;
}

#ifdef INTERNAL_INSPIRE_MATRIX
typedef enum {
    JOY_UP = -2,
    JOY_DOWN = -3,
    JOY_LEFT = -4,
    JOY_RIGHT = -5,
    JOY_X = -6,
    JOY_Y = -7,
    JOY_ACT = -8,
    JOY_A = -10,
    JOY_B = -11,
    JOY_MIDDLE = -9
} JOY_Button;

JOY_Button JOY_check_button(uint16_t adc_value) {
     //printf("Special ADC: %d\n", adc_value);
    if (abs(adc_value - JOY_2) <= SPECIAL_BUTTON_DEVIATION)
        return JOY_UP;
    else if (abs(adc_value - JOY_8) <= SPECIAL_BUTTON_DEVIATION)
        return JOY_DOWN;
    else if (abs(adc_value - JOY_4) <= SPECIAL_BUTTON_DEVIATION)
        return JOY_LEFT;
    else if (abs(adc_value - JOY_6) <= SPECIAL_BUTTON_DEVIATION)
        return JOY_RIGHT;
    else if (abs(adc_value - JOY_5) <= SPECIAL_BUTTON_DEVIATION)
        return JOY_MIDDLE;
    else if (abs(adc_value - JOY_1) <= SPECIAL_BUTTON_DEVIATION)
        return JOY_A;
    else if (abs(adc_value - JOY_3) <= SPECIAL_BUTTON_DEVIATION)
        return JOY_B;
    else if (abs(adc_value - JOY_7) <= SPECIAL_BUTTON_DEVIATION)
        return JOY_X;
    else if (abs(adc_value - JOY_9) <= SPECIAL_BUTTON_DEVIATION)
        return JOY_Y;
    else
        return no_button_pressed;
}

#define JOY_act_pressed()        (JOY_check_button(multiple_ADC_reads(ADC_read_smallboard, 5)) == JOY_ACT)
#define JOY_act_released()      (JOY_check_button(multiple_ADC_reads(ADC_read_smallboard, 5)) != JOY_ACT)
#define JOY_up_pressed()    (JOY_check_button(multiple_ADC_reads(ADC_read_smallboard, 5)) == JOY_UP)
#define JOY_down_pressed()  (JOY_check_button(multiple_ADC_reads(ADC_read_smallboard, 5)) == JOY_DOWN)
#define JOY_left_pressed() (JOY_check_button(multiple_ADC_reads(ADC_read_smallboard, 5)) == JOY_LEFT)
#define JOY_right_pressed() (JOY_check_button(multiple_ADC_reads(ADC_read_smallboard, 5)) == JOY_RIGHT)
#define JOY_middle_pressed() (JOY_check_button(multiple_ADC_reads(ADC_read_smallboard, 5)) == JOY_MIDDLE)
#define JOY_X_pressed() (JOY_check_button(multiple_ADC_reads(ADC_read_smallboard, 5)) == JOY_X)
#define JOY_Y_pressed() (JOY_check_button(multiple_ADC_reads(ADC_read_smallboard, 5)) == JOY_Y)
#define JOY_A_pressed() (JOY_check_button(multiple_ADC_reads(ADC_read_smallboard, 5)) == JOY_A)
#define JOY_B_pressed() (JOY_check_button(multiple_ADC_reads(ADC_read_smallboard, 5)) == JOY_B)
#define JOY_Large_pressed() ((matrix_pressed_two()))
///////////////////////////////////////////////////////////////////////////
#define JOY_1_pressed() (JOY_check_button(multiple_ADC_reads(ADC_read_smallboard, 5)) == JOY_A)
#define JOY_2_pressed() (JOY_check_button(multiple_ADC_reads(ADC_read_smallboard, 5)) == JOY_UP)
#define JOY_3_pressed() (JOY_check_button(multiple_ADC_reads(ADC_read_smallboard, 5)) == JOY_B)
#define JOY_4_pressed() (JOY_check_button(multiple_ADC_reads(ADC_read_smallboard, 5)) == JOY_LEFT)
#define JOY_5_pressed() (JOY_check_button(multiple_ADC_reads(ADC_read_smallboard, 5)) == JOY_MIDDLE)
#define JOY_6_pressed() (JOY_check_button(multiple_ADC_reads(ADC_read_smallboard, 5)) == JOY_RIGHT)
#define JOY_7_pressed() (JOY_check_button(multiple_ADC_reads(ADC_read_smallboard, 5)) == JOY_X)
#define JOY_8_pressed() (JOY_check_button(multiple_ADC_reads(ADC_read_smallboard, 5)) == JOY_DOWN)
#define JOY_9_pressed() (JOY_check_button(multiple_ADC_reads(ADC_read_smallboard, 5)) == JOY_Y)

uint16_t lower_half_ADC_channel(void) {return GPIO_analogRead(GPIO_Ain4_D3);}
uint16_t upper_half_ADC_channel(void) {return GPIO_analogRead(GPIO_Ain3_D2);}
#define no_button_pressed -1

int8_t matrix_pressed_panel(void) {
    const int8_t samples = 5;
    uint16_t adc = multiple_ADC_reads(ADC_read_smallboard, samples);
    #define PANEL_BUTTONS 9
    for (int8_t i = 0; i < PANEL_BUTTONS; i++) {
        int deviation = abs(adc - buttonsPanel[i]);
        if (deviation <= BUTTON_DEVIATION) {
            return i;
        }
    }
    return no_button_pressed;
}

int8_t matrix_pressed_two(void) {
    const int8_t samples = 5;
    uint16_t adc = multiple_ADC_reads(upper_half_ADC_channel, samples);
    if(adc > 60){
        printf("upper ADC: %d\n", adc);
    }

    #define UPPER_HALF_BUTTONS_START 32
    for (int8_t i = 0; i <UPPER_HALF_BUTTONS_START ; i++) {
        int deviation = abs(adc - buttons[i]);
        if (deviation <= BUTTON_DEVIATION_UPPER_HALF) {
            return i;
        }
    }
    // non-linear ADC delta value 
   /* for (int8_t i = 0; i < LOWER_HALF_BUTTONS / 2; i++) {
        int deviation = abs(adc - buttons[i]);
        if (deviation <= BUTTON_DEVIATION * 2) {
            return i;
        }
    }
    for (int8_t i = LOWER_HALF_BUTTONS / 2; i < LOWER_HALF_BUTTONS; i++) {
        int deviation = abs(adc - buttons[i]);
        if (deviation <= BUTTON_DEVIATION) {
            return i;
        }
    }*/
    // end non linear
    adc = multiple_ADC_reads(lower_half_ADC_channel, samples);
    if(adc > 60){
        printf("lower ADC: %d\n", adc);
    }
    #define LOWER_HALF_BUTTONS 32
    for (int8_t i = LOWER_HALF_BUTTONS; i < NUM_BUTTONS; i++) {
        int deviation = abs(adc - buttons[i]);
        if (deviation <= BUTTON_DEVIATION) {
            //printf("Re Upper: %d\n", i);
            return i;
        }
    }
    return no_button_pressed;
}

#else

// Buttons
#define JOY_act_pressed()         (gpio_act_pressed())
#define JOY_act_released()        (!gpio_act_pressed())
#define JOY_pad_pressed()         (ADC_read_pad() > 10)
#define JOY_pad_released()        (ADC_read_pad() <= 10)
#define JOY_all_released()        (JOY_act_released() && JOY_pad_released())

static inline uint8_t JOY_up_pressed(void) {
 uint16_t val = ADC_read_pad();
 return(   ((val > JOY_N  - JOY_DEV) && (val < JOY_N  + JOY_DEV))
         | ((val > JOY_NE - JOY_DEV) && (val < JOY_NE + JOY_DEV))
         | ((val > JOY_NW - JOY_DEV) && (val < JOY_NW + JOY_DEV)) );
}

static inline uint8_t JOY_down_pressed(void) {
 uint16_t val = ADC_read_pad();
 return(   ((val > JOY_S  - JOY_DEV) && (val < JOY_S  + JOY_DEV))
         | ((val > JOY_SE - JOY_DEV) && (val < JOY_SE + JOY_DEV))
         | ((val > JOY_SW - JOY_DEV) && (val < JOY_SW + JOY_DEV)) );
}

static inline uint8_t JOY_left_pressed(void) {
 uint16_t val = ADC_read_pad();
 return(   ((val > JOY_W  - JOY_DEV) && (val < JOY_W  + JOY_DEV))
         | ((val > JOY_NW - JOY_DEV) && (val < JOY_NW + JOY_DEV))
         | ((val > JOY_SW - JOY_DEV) && (val < JOY_SW + JOY_DEV)) );
}

static inline uint8_t JOY_right_pressed(void) {
 uint16_t val = ADC_read_pad();
 return(   ((val > JOY_E  - JOY_DEV) && (val < JOY_E  + JOY_DEV))
         | ((val > JOY_NE - JOY_DEV) && (val < JOY_NE + JOY_DEV))
         | ((val > JOY_SE - JOY_DEV) && (val < JOY_SE + JOY_DEV)) );
}

#endif

uint16_t rnval;
uint16_t JOY_random(void) {
  rnval = (rnval >> 0x01) ^ (-(rnval & 0x01) & 0xB400);
  return rnval;
}

void JOY_setseed_default(void){
  rnval = 0x1234;
}

void JOY_setseed(uint16_t seed){
  rnval = seed;
}


#endif
