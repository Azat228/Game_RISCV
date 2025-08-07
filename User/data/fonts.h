#pragma once

#include "colors.h"
#include "funconfig.h"

#define font_width 3
#define font_height 5
#define LED_size 8
#define font_size (font_width * font_height)
#define Letter_size (LED_size * LED_size)

// clang-format off

const uint8_t font_1[font_size] = {
    1,1,0,
    0,1,0,
    0,1,0,
    0,1,0,
    1,1,1};

const uint8_t font_2[font_size] = {
    1,1,1,
    0,0,1,
    1,1,1,
    1,0,0,
    1,1,1};

const uint8_t font_3[font_size] = {
    1,1,1,
    0,0,1,
    1,1,1,
    0,0,1,
    1,1,1};

const uint8_t font_4[font_size] = {
    1,0,1,
    1,0,1,
    1,1,1,
    0,0,1,
    0,0,1};

const uint8_t font_5[font_size] = {
    1,1,1,
    1,0,0,
    1,1,1,
    0,0,1,
    1,1,1};

const uint8_t font_6[font_size] = {
    1,1,1,
    1,0,0,
    1,1,1,
    1,0,1,
    1,1,1};

const uint8_t font_7[font_size] = {
    1,1,1,
    0,0,1,
    0,0,1,
    0,0,1,
    0,0,1};

const uint8_t font_8[font_size] = {
    1,1,1,
    1,0,1,
    1,1,1,
    1,0,1,
    1,1,1};

const uint8_t font_9[font_size] = {
    1,1,1,
    1,0,1,
    1,1,1,
    0,0,1,
    1,1,1};

const uint8_t font_0[font_size] = {
    1,1,1,
    1,0,1,
    1,0,1,
    1,0,1,
    1,1,1};

const uint8_t font_X[font_size] = {
    1,0,1,
    1,0,1,
    0,1,0,
    1,0,1,
    1,0,1};

const uint8_t font_D[font_size] = {
    1,1,0,
    1,0,1,
    1,0,1,
    1,0,1,
    1,1,0};
const uint8_t A[Letter_size] = {
            0,0,1,1,1,1,0,0,
            0,1,1,0,0,1,1,0,
            1,1,0,0,0,0,1,1,
            1,1,0,0,0,0,1,1,
            1,1,1,1,1,1,1,1,
            1,1,0,0,0,0,1,1,
            1,1,0,0,0,0,1,1,
            1,1,0,0,0,0,1,1
};
const uint8_t B[Letter_size] = {
    1,1,1,1,1,1,0,0,
    1,1,0,0,0,1,1,0,
    1,1,0,0,0,1,1,0,
    1,1,1,1,1,1,0,0,
    1,1,0,0,0,1,1,0,
    1,1,0,0,0,1,1,0,
    1,1,0,0,0,1,1,0,
    1,1,1,1,1,1,0,0
};

const uint8_t C[Letter_size] = {
    0,0,1,1,1,1,0,0,
    0,1,1,0,0,1,1,0,
    1,1,0,0,0,0,0,0,
    1,1,0,0,0,0,0,0,
    1,1,0,0,0,0,0,0,
    1,1,0,0,0,0,0,0,
    0,1,1,0,0,1,1,0,
    0,0,1,1,1,1,0,0
};

const uint8_t D[Letter_size] = {
    1,1,1,1,1,0,0,0,
    1,1,0,0,1,1,0,0,
    1,1,0,0,0,1,1,0,
    1,1,0,0,0,1,1,0,
    1,1,0,0,0,1,1,0,
    1,1,0,0,0,1,1,0,
    1,1,0,0,1,1,0,0,
    1,1,1,1,1,0,0,0
};

const uint8_t E[Letter_size] = {
    1,1,1,1,1,1,1,1,
    1,1,0,0,0,0,0,0,
    1,1,0,0,0,0,0,0,
    1,1,1,1,1,1,0,0,
    1,1,0,0,0,0,0,0,
    1,1,0,0,0,0,0,0,
    1,1,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1
};

const uint8_t F[Letter_size] = {
    1,1,1,1,1,1,1,1,
    1,1,0,0,0,0,0,0,
    1,1,0,0,0,0,0,0,
    1,1,1,1,1,1,0,0,
    1,1,0,0,0,0,0,0,
    1,1,0,0,0,0,0,0,
    1,1,0,0,0,0,0,0,
    1,1,0,0,0,0,0,0
};

const uint8_t G[Letter_size] = {
    0,0,1,1,1,1,0,0,
    0,1,1,0,0,1,1,0,
    1,1,0,0,0,0,0,0,
    1,1,0,0,0,0,0,0,
    1,1,0,0,1,1,1,1,
    1,1,0,0,0,1,1,0,
    0,1,1,0,0,1,1,0,
    0,0,1,1,1,1,0,0
};

const uint8_t H[Letter_size] = {
    1,1,0,0,0,0,1,1,
    1,1,0,0,0,0,1,1,
    1,1,0,0,0,0,1,1,
    1,1,1,1,1,1,1,1,
    1,1,0,0,0,0,1,1,
    1,1,0,0,0,0,1,1,
    1,1,0,0,0,0,1,1,
    1,1,0,0,0,0,1,1
};

const uint8_t I[Letter_size] = {
    0,1,1,1,1,1,1,0,
    0,0,0,1,1,0,0,0,
    0,0,0,1,1,0,0,0,
    0,0,0,1,1,0,0,0,
    0,0,0,1,1,0,0,0,
    0,0,0,1,1,0,0,0,
    0,0,0,1,1,0,0,0,
    0,1,1,1,1,1,1,0
};

const uint8_t J[Letter_size] = {
    0,0,0,0,0,1,1,0,
    0,0,0,0,0,1,1,0,
    0,0,0,0,0,1,1,0,
    0,0,0,0,0,1,1,0,
    0,0,0,0,0,1,1,0,
    1,1,0,0,0,1,1,0,
    1,1,0,0,0,1,1,0,
    0,1,1,1,1,1,0,0
};

const uint8_t K[Letter_size] = {
    1,1,0,0,0,1,1,0,
    1,1,0,0,1,1,0,0,
    1,1,0,1,1,0,0,0,
    1,1,1,1,0,0,0,0,
    1,1,1,1,0,0,0,0,
    1,1,0,1,1,0,0,0,
    1,1,0,0,1,1,0,0,
    1,1,0,0,0,1,1,0
};

const uint8_t L[Letter_size] = {
    1,1,0,0,0,0,0,0,
    1,1,0,0,0,0,0,0,
    1,1,0,0,0,0,0,0,
    1,1,0,0,0,0,0,0,
    1,1,0,0,0,0,0,0,
    1,1,0,0,0,0,0,0,
    1,1,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1
};

const uint8_t M[Letter_size] = {
    1,1,0,0,0,0,1,1,
    1,1,1,0,0,1,1,1,
    1,1,1,1,1,1,1,1,
    1,1,0,1,1,0,1,1,
    1,1,0,0,0,0,1,1,
    1,1,0,0,0,0,1,1,
    1,1,0,0,0,0,1,1,
    1,1,0,0,0,0,1,1
};

const uint8_t N[Letter_size] = {
    1,1,0,0,0,0,1,1,
    1,1,1,0,0,0,1,1,
    1,1,1,1,0,0,1,1,
    1,1,0,1,1,0,1,1,
    1,1,0,0,1,1,1,1,
    1,1,0,0,0,1,1,1,
    1,1,0,0,0,0,1,1,
    1,1,0,0,0,0,1,1
};

const uint8_t O[Letter_size] = {
    0,0,1,1,1,1,0,0,
    0,1,1,0,0,1,1,0,
    1,1,0,0,0,0,1,1,
    1,1,0,0,0,0,1,1,
    1,1,0,0,0,0,1,1,
    1,1,0,0,0,0,1,1,
    0,1,1,0,0,1,1,0,
    0,0,1,1,1,1,0,0
};

const uint8_t P[Letter_size] = {
    1,1,1,1,1,1,0,0,
    1,1,0,0,0,1,1,0,
    1,1,0,0,0,1,1,0,
    1,1,1,1,1,1,0,0,
    1,1,0,0,0,0,0,0,
    1,1,0,0,0,0,0,0,
    1,1,0,0,0,0,0,0,
    1,1,0,0,0,0,0,0
};

const uint8_t Q[Letter_size] = {
    0,0,1,1,1,1,0,0,
    0,1,1,0,0,1,1,0,
    1,1,0,0,0,0,1,1,
    1,1,0,0,0,0,1,1,
    1,1,0,0,1,0,1,1,
    1,1,0,0,1,1,1,1,
    0,1,1,0,0,1,1,0,
    0,0,1,1,1,1,1,1
};

const uint8_t R[Letter_size] = {
    1,1,1,1,1,1,0,0,
    1,1,0,0,0,1,1,0,
    1,1,0,0,0,1,1,0,
    1,1,1,1,1,1,0,0,
    1,1,0,1,1,0,0,0,
    1,1,0,0,1,1,0,0,
    1,1,0,0,0,1,1,0,
    1,1,0,0,0,0,1,1
};

const uint8_t S[Letter_size] = {
    0,0,1,1,1,1,0,0,
    0,1,1,0,0,1,1,0,
    1,1,0,0,0,0,0,0,
    0,1,1,1,1,1,0,0,
    0,0,0,0,0,1,1,0,
    0,0,0,0,0,0,1,1,
    0,1,1,0,0,1,1,0,
    0,0,1,1,1,1,0,0
};

const uint8_t T[Letter_size] = {
    1,1,1,1,1,1,1,1,
    0,0,0,1,1,0,0,0,
    0,0,0,1,1,0,0,0,
    0,0,0,1,1,0,0,0,
    0,0,0,1,1,0,0,0,
    0,0,0,1,1,0,0,0,
    0,0,0,1,1,0,0,0,
    0,0,0,1,1,0,0,0
};

const uint8_t U[Letter_size] = {
    1,1,0,0,0,0,1,1,
    1,1,0,0,0,0,1,1,
    1,1,0,0,0,0,1,1,
    1,1,0,0,0,0,1,1,
    1,1,0,0,0,0,1,1,
    1,1,0,0,0,0,1,1,
    0,1,1,0,0,1,1,0,
    0,0,1,1,1,1,0,0
};

const uint8_t V[Letter_size] = {
    1,1,0,0,0,0,1,1,
    1,1,0,0,0,0,1,1,
    1,1,0,0,0,0,1,1,
    1,1,0,0,0,0,1,1,
    0,1,1,0,0,1,1,0,
    0,0,1,1,1,1,0,0,
    0,0,0,1,1,0,0,0,
    0,0,0,0,0,0,0,0
};

const uint8_t W[Letter_size] = {
    1,1,0,0,0,0,1,1,
    1,1,0,0,0,0,1,1,
    1,1,0,0,0,0,1,1,
    1,1,0,1,1,0,1,1,
    1,1,1,1,1,1,1,1,
    1,1,1,0,0,1,1,1,
    1,1,0,0,0,0,1,1,
    1,1,0,0,0,0,1,1
};

const uint8_t X[Letter_size] = {
    1,1,0,0,0,0,1,1,
    0,1,1,0,0,1,1,0,
    0,0,1,1,1,1,0,0,
    0,0,0,1,1,0,0,0,
    0,0,0,1,1,0,0,0,
    0,0,1,1,1,1,0,0,
    0,1,1,0,0,1,1,0,
    1,1,0,0,0,0,1,1
};

const uint8_t Y[Letter_size] = {
    1,1,0,0,0,0,1,1,
    0,1,1,0,0,1,1,0,
    0,0,1,1,1,1,0,0,
    0,0,0,1,1,0,0,0,
    0,0,0,1,1,0,0,0,
    0,0,0,1,1,0,0,0,
    0,0,0,1,1,0,0,0,
    0,0,0,1,1,0,0,0
};

const uint8_t Z[Letter_size] = {
    1,1,1,1,1,1,1,1,
    0,0,0,0,0,1,1,0,
    0,0,0,0,1,1,0,0,
    0,0,0,1,1,0,0,0,
    0,0,1,1,0,0,0,0,
    0,1,1,0,0,0,0,0,
    1,1,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1
};
// clang-format on

const uint8_t * font_list[] = {font_0, font_1, font_2, font_3, font_4, font_5, font_6, font_7,
    font_8, font_9, font_X, font_D};
const uint8_t *Letter_List[] = {A, B, C, D, E, F, G, H, I, J, K, L, M,
                                N, O, P, Q, R, S, T, U, V, W, X, Y, Z};

const int num_fonts = sizeof(font_list) / sizeof(font_list[0]);

static inline void font_draw(const uint8_t font[], color_t color, int startpos) {
    // insert the font_width*font_height font to the led_array
    // which has horizontalButtons*verticalButtons leds
    if (startpos < 0 || startpos >= NUM_LEDS ||
        startpos % horizontalButtons + font_width > horizontalButtons ||
        startpos / horizontalButtons + font_height > verticalButtons) {
        return;
    }
    for (int i = 0; i < font_size; i++) {
        if (font[i]) {
            int x = (font_size - i - 1) % font_width;
            int y = (font_size - i - 1) / font_width;
            int pos = startpos + x + y * horizontalButtons;
            set_color(pos, color);
        }
    }
}
static inline void Letter_draw(const uint8_t font[], color_t color, int startpos) {
    if (startpos < 0 || startpos >= NUM_LEDS ||
        startpos % horizontalButtons + LED_size > horizontalButtons ||
        startpos / horizontalButtons + LED_size > verticalButtons) {
        return;
    }
    for (int i = 0; i < Letter_size; i++) {
        if (font[i]) {
            int y = (Letter_size- i-1) / LED_size;  // Row (0 to 7)
            int x = (Letter_size - i -1) % LED_size;  // Column (0 to 7)
            int pos = startpos + x + y * horizontalButtons;
            set_color(pos, color);
        }
    }
}
