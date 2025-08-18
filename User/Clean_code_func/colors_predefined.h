#ifndef COLORS_PREDEFINED_H
#define COLORS_PREDEFINED_H
#include "./data/colors.h"
color_t appleColor = {.r = 255, .g = 0, .b = 0};        // red
color_t snakeHeadColor = {.r = 0, .g = 45, .b = 45};    // cyan
color_t snakeBodyColor = {.r = 51, .g = 255, .b = 51};  // green
color_t snakeTailColor = {.r = 55, .g = 12, .b = 51};   // yellow
color_t blankColor = {.r = 0, .g = 0, .b = 0};          // off
color_t scoreColor = {.r = 90, .g = 55, .b = 12};    // green for scores
color_t speedBoostColor = {.r = 255, .g = 0, .b = 255}; // Purple indicator
color_t brickColor = {.r = 200, .g = 80, .b = 10};
color_t paddleColor = {.r = 10, .g = 100, .b = 255};
color_t ballColor = {.r = 255, .g = 255, .b = 255};
//colors of the letters
color_t letters_color[26] = {
    {.r=255, .g=0,   .b=0},      // A - Red
    {.r=0,   .g=128, .b=255},    // B - Sky Blue
    {.r=255, .g=140, .b=0},      // C - Orange
    {.r=34,  .g=139, .b=34},     // D - Forest Green
    {.r=255, .g=255, .b=0},      // E - Yellow
    {.r=255, .g=20,  .b=147},    // F - Deep Pink
    {.r=0,   .g=255, .b=255},    // G - Cyan
    {.r=138, .g=43,  .b=226},    // H - Blue Violet
    {.r=210, .g=105, .b=30},     // I - Chocolate
    {.r=255, .g=69,  .b=0},      // J - Red Orange
    {.r=0,   .g=255, .b=0},      // K - Lime
    {.r=135, .g=206, .b=250},    // L - Light Sky Blue
    {.r=128, .g=0,   .b=128},    // M - Purple
    {.r=255, .g=215, .b=0},      // N - Gold
    {.r=0,   .g=0,   .b=255},    // O - Blue
    {.r=255, .g=192, .b=203},    // P - Pink
    {.r=255, .g=255, .b=255},    // Q - White
    {.r=0,   .g=255, .b=127},    // R - Spring Green
    {.r=139, .g=69,  .b=19},     // S - Saddle Brown
    {.r=240, .g=230, .b=140},    // T - Khaki
    {.r=255, .g=99,  .b=71},     // U - Tomato
    {.r=64,  .g=224, .b=208},    // V - Turquoise
    {.r=255, .g=0,   .b=255},    // W - Magenta
    {.r=47,  .g=79,  .b=79},     // X - Dark Slate Gray
    {.r=0,   .g=0,   .b=139},    // Y - Dark Blue
    {.r=173, .g=255, .b=47}      // Z - Green Yellow
};
#endif // COLORS_PREDEFINED_H
