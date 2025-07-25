/// @brief Button calibration values from ADC_read()
#pragma once

#include "funconfig.h"

// Joypad calibration values
/*#define JOY_N 197  // joypad UP
#define JOY_NE 259 // joypad UP + RIGHT
#define JOY_E 90   // joypad RIGHT
#define JOY_SE 388 // joypad DOWN + RIGHT
#define JOY_S 346  // joypad DOWN
#define JOY_SW 616 // joypad DOWN + LEFT
#define JOY_W 511  // joypad LEFT
#define JOY_NW 567 // JOYPAD UP + LEFT
#define JOY_DEV 20 // deviation*/




#ifndef NUM_BUTTONS
#define NUM_BUTTONS NUM_LEDS
#endif

#if INTERNAL_INSPIRE_MATRIX

#define JOY_1 683  // joypad UP
#define JOY_2 620 // joypad UP + RIGHT
#define JOY_3 564   // joypad RIGHT
#define JOY_4 514// joypad DOWN + RIGHT
#define JOY_5 469  // joypad DOWN
#define JOY_6 427 // joypad DOWN + LEFT
#define JOY_7 389  // joypad LEFT
#define JOY_8 354 // JOYPAD UP + LEFT
#define JOY_9 320 // deviation

#define BUTTON_NONE 30///31
#define BUTTON_DEVIATION 8
#define BUTTON_DEVIATION_UPPER_HALF 5
// PD2 PIN as for now
#define BUTTON_63 683
#define BUTTON_62 639
#define BUTTON_61 600
#define BUTTON_60 565
#define BUTTON_59 533
#define BUTTON_58 504
#define BUTTON_57 478
#define BUTTON_56 453//
#define BUTTON_55 432
#define BUTTON_54 410
#define BUTTON_53 391
#define BUTTON_52 373
#define BUTTON_51 356
#define BUTTON_50 340
#define BUTTON_49 324
#define BUTTON_48 309//
#define BUTTON_47 296
#define BUTTON_46 283
#define BUTTON_45 271
#define BUTTON_44 258
#define BUTTON_43 246
#define BUTTON_42 235
#define BUTTON_41 224
#define BUTTON_40 213//
#define BUTTON_39 202
#define BUTTON_38 192
#define BUTTON_37 181
#define BUTTON_36 172
#define BUTTON_35 162
#define BUTTON_34 150
#define BUTTON_33 140
#define BUTTON_32 130//
// PD3 PIN as for now
#define BUTTON_31 683
#define BUTTON_30 639
#define BUTTON_29 600
#define BUTTON_28 565
#define BUTTON_27 533
#define BUTTON_26 504
#define BUTTON_25 477
#define BUTTON_24 453//
#define BUTTON_23 431
#define BUTTON_22 410
#define BUTTON_21 391
#define BUTTON_20 372
#define BUTTON_19 356
#define BUTTON_18 340
#define BUTTON_17 324
#define BUTTON_16 309//
#define BUTTON_15 296
#define BUTTON_14 283
#define BUTTON_13 271
#define BUTTON_12 258
#define BUTTON_11 246
#define BUTTON_10 235
#define BUTTON_9 224
#define BUTTON_8 213//
#define BUTTON_7 202
#define BUTTON_6 192
#define BUTTON_5 181
#define BUTTON_4 171
#define BUTTON_3 162
#define BUTTON_2 150
#define BUTTON_1 140
#define BUTTON_0 130//
#define BUTTON_X 20
#define BUTTON_Y 10
#define BUTTON_UP 46
#define BUTTON_DOWN 29
#define BUTTON_LEFT 55
#define BUTTON_RIGHT 37 /// @note When using battery power source without laptop grounding,
// right button seems cannot reliably detected.
#define SPECIAL_BUTTON_DEVIATION 15

/// @brief Array of buttons corresponding to the ADC values, for linear searching
static const int buttons[NUM_BUTTONS] = {BUTTON_0, BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4,
    BUTTON_5, BUTTON_6, BUTTON_7, BUTTON_8, BUTTON_9, BUTTON_10, BUTTON_11, BUTTON_12,
    BUTTON_13, BUTTON_14, BUTTON_15, BUTTON_16, BUTTON_17, BUTTON_18, BUTTON_19,
    BUTTON_20, BUTTON_21, BUTTON_22, BUTTON_23, BUTTON_24, BUTTON_25, BUTTON_26,
    BUTTON_27, BUTTON_28, BUTTON_29, BUTTON_30, BUTTON_31, BUTTON_32, BUTTON_33,
    BUTTON_34, BUTTON_35, BUTTON_36, BUTTON_37, BUTTON_38, BUTTON_39, BUTTON_40,
    BUTTON_41, BUTTON_42, BUTTON_43, BUTTON_44, BUTTON_45, BUTTON_46, BUTTON_47,
    BUTTON_48, BUTTON_49, BUTTON_50, BUTTON_51, BUTTON_52, BUTTON_53, BUTTON_54,
    BUTTON_55, BUTTON_56, BUTTON_57, BUTTON_58, BUTTON_59, BUTTON_60, BUTTON_61,
    BUTTON_62, BUTTON_63};

static const int buttonsPanel[9] = {JOY_1, JOY_2, JOY_3, JOY_4, JOY_5,
        JOY_6, JOY_7, JOY_8, JOY_9};

#else

#define JOY_1 683  // joypad UP
#define JOY_2 620 // joypad UP + RIGHT
#define JOY_3 564   // joypad RIGHT
#define JOY_4 514// joypad DOWN + RIGHT
#define JOY_5 469  // joypad DOWN
#define JOY_6 425 // joypad DOWN + LEFT
#define JOY_7 389  // joypad LEFT
#define JOY_8 354 // JOYPAD UP + LEFT
#define JOY_9 319 // deviation

#define BUTTON_NONE 30///31
#define BUTTON_DEVIATION 8
#define BUTTON_DEVIATION_UPPER_HALF 8
// PD2 PIN as for now
#define BUTTON_63 684
#define BUTTON_62 640
#define BUTTON_61 602
#define BUTTON_60 566
#define BUTTON_59 534
#define BUTTON_58 505
#define BUTTON_57 478
#define BUTTON_56 454//
#define BUTTON_55 432
#define BUTTON_54 411
#define BUTTON_53 391
#define BUTTON_52 373
#define BUTTON_51 356
#define BUTTON_50 341
#define BUTTON_49 325
#define BUTTON_48 311//
#define BUTTON_47 296
#define BUTTON_46 282
#define BUTTON_45 270
#define BUTTON_44 258
#define BUTTON_43 245
#define BUTTON_42 234
#define BUTTON_41 223
#define BUTTON_40 212//
#define BUTTON_39 202
#define BUTTON_38 192
#define BUTTON_37 182
#define BUTTON_36 170
#define BUTTON_35 160
#define BUTTON_34 150
#define BUTTON_33 140
#define BUTTON_32 130//
// PD3 PIN as for now
#define BUTTON_31 683
#define BUTTON_30 640
#define BUTTON_29 600
#define BUTTON_28 565
#define BUTTON_27 534
#define BUTTON_26 504
#define BUTTON_25 478
#define BUTTON_24 453//
#define BUTTON_23 431
#define BUTTON_22 410
#define BUTTON_21 391
#define BUTTON_20 373
#define BUTTON_19 356
#define BUTTON_18 341
#define BUTTON_17 325
#define BUTTON_16 310//
#define BUTTON_15 297
#define BUTTON_14 283
#define BUTTON_13 271
#define BUTTON_12 260
#define BUTTON_11 246
#define BUTTON_10 235
#define BUTTON_9 224
#define BUTTON_8 214//
#define BUTTON_7 202
#define BUTTON_6 192
#define BUTTON_5 181
#define BUTTON_4 170
#define BUTTON_3 160
#define BUTTON_2 150
#define BUTTON_1 140
#define BUTTON_0 130//
#define BUTTON_X 20
#define BUTTON_Y 10
#define BUTTON_UP 46
#define BUTTON_DOWN 29
#define BUTTON_LEFT 55
#define BUTTON_RIGHT 37 /// @note When using battery power source without laptop grounding,
// right button seems cannot reliably detected.
#define SPECIAL_BUTTON_DEVIATION 12

/// @brief Array of buttons corresponding to the ADC values, for linear searching
static const int buttons[NUM_BUTTONS] = {BUTTON_0, BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4,
    BUTTON_5, BUTTON_6, BUTTON_7, BUTTON_8, BUTTON_9, BUTTON_10, BUTTON_11, BUTTON_12,
    BUTTON_13, BUTTON_14, BUTTON_15, BUTTON_16, BUTTON_17, BUTTON_18, BUTTON_19,
    BUTTON_20, BUTTON_21, BUTTON_22, BUTTON_23, BUTTON_24, BUTTON_25, BUTTON_26,
    BUTTON_27, BUTTON_28, BUTTON_29, BUTTON_30, BUTTON_31, BUTTON_32, BUTTON_33,
    BUTTON_34, BUTTON_35, BUTTON_36, BUTTON_37, BUTTON_38, BUTTON_39, BUTTON_40,
    BUTTON_41, BUTTON_42, BUTTON_43, BUTTON_44, BUTTON_45, BUTTON_46, BUTTON_47,
    BUTTON_48, BUTTON_49, BUTTON_50, BUTTON_51, BUTTON_52, BUTTON_53, BUTTON_54,
    BUTTON_55, BUTTON_56, BUTTON_57, BUTTON_58, BUTTON_59, BUTTON_60, BUTTON_61,
    BUTTON_62, BUTTON_63};

static const int buttonsPanel[9] = {JOY_1, JOY_2, JOY_3, JOY_4, JOY_5,
        JOY_6, JOY_7, JOY_8, JOY_9};

#endif
