#define WS2812BSIMPLE_IMPLEMENTATION
#include "ch32v003fun/ch32v003fun.h"
#include "./data/colors.h"
#include "./ch32v003fun/driver.h"
#include "./ch32v003fun/ws2812b_simple.h"
#include "./data/fonts.h"
#include <stdbool.h>
#include "ch32v003fun/i2c_tx.h"
#define LED_PINS GPIOA, 2
// Game constants
#define INITIAL_SPEED 500  // ms between moves
#define GRID_SIZE 8        // 8x8 grid
#define MAX_SCORES 10      // Number of high scores to keep
#define AT24C256_ADDR 0x52
#define SCORE_SIZE 2 // Each score uses 2 bytes (index + value)

typedef struct snakePartDir {
    char part;      // 'h'=head, 'b'=body, 't'=tail, 'a'=apple, '0'=empty
    int8_t direction; // movement direction
} snakePartDir;

// Colors
color_t appleColor = {.r = 255, .g = 0, .b = 0};        // red
color_t snakeHeadColor = {.r = 0, .g = 45, .b = 45};    // cyan
color_t snakeBodyColor = {.r = 51, .g = 255, .b = 51};  // green
color_t snakeTailColor = {.r = 55, .g = 12, .b = 51};   // yellow
color_t blankColor = {.r = 0, .g = 0, .b = 0};          // off
color_t scoreColor = {.r = 90, .g = 55, .b = 12};    // green for scores
color_t speedBoostColor = {.r = 255, .g = 0, .b = 255}; // Purple indicator
// Game state
snakePartDir gameBoard[GRID_SIZE * GRID_SIZE];
int8_t snakeHead, snakeTail;
uint8_t score;
uint8_t scoreHistory[MAX_SCORES] = {0};
uint8_t currentScoreIndex = 0;
bool gameOver;
bool game_regime = false;
uint16_t speedCounter = 0;
uint16_t Score_start_adress = 0x0008;

// Generate a new apple at random empty position
void generate_apple(void) {
    uint8_t applePos;
    do {
        applePos = JOY_random() % (GRID_SIZE * GRID_SIZE);
    } while (gameBoard[applePos].part != '0');
    gameBoard[applePos] = (snakePartDir){'a', 0};
}

// Initialize game board and snake
void game_init() {
    // Clear game board
    for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
        gameBoard[i] = (snakePartDir){'0', 0};
    }

    // Initialize snake (length 3)
    snakeTail = 47;
    snakeHead = 45;
    gameBoard[47] = (snakePartDir){'t', -1}; // tail
    gameBoard[46] = (snakePartDir){'b', -1}; // body
    gameBoard[45] = (snakePartDir){'h', -1}; // head

    // Reset score and speed
    score = 0;
    speedCounter = 0;

    // Place first apple
    generate_apple();
}

void clear_board(void) {
    for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
        gameBoard[i] = (snakePartDir){'0', 0};
    }
}

// Update LED display based on game board
void display() {
    clear();
    if (game_regime) {
             set_color(63, speedBoostColor); // Top-right corner
         }
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
    for (int i = 0; i < (GRID_SIZE * GRID_SIZE-1); i++) {
        switch (gameBoard[i].part) {
            case 't': set_color(i, snakeTailColor); break;
            case 'b': set_color(i, snakeBodyColor); break;
            case 'h': set_color(i, snakeHeadColor); break;
            case 'a': set_color(i, appleColor); break;
            default: set_color(i, blankColor); break;
        }
    }
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);

}

// Get new direction from joystick input
int8_t get_new_direction(int8_t currentDirection) {
    if (JOY_2_pressed() && currentDirection != -8) return 8;    // up
    if (JOY_8_pressed() && currentDirection != 8) return -8;    // down
    if (JOY_4_pressed() && currentDirection != -1) return 1;    // left
    if (JOY_6_pressed() && currentDirection != 1) return -1;    // right
    return currentDirection;
}

// Check if snake ate an apple
bool check_apple(int8_t newHeadPos) {
    return gameBoard[newHeadPos].part == 'a';
}

// Check for collisions
bool check_collision(int8_t newHeadPos) {
    // Check walls
    if (newHeadPos < 0 || newHeadPos >= GRID_SIZE * GRID_SIZE) return true;

    // Check if new head position wraps around the grid edges
    int8_t col = newHeadPos % GRID_SIZE;
    int8_t row = newHeadPos / GRID_SIZE;

    // Check left/right wall collision
    if ((col == 0 && gameBoard[snakeHead].direction == 1) ||
        (col == GRID_SIZE-1 && gameBoard[snakeHead].direction == -1)) {
        return true;
    }

    // Check snake body collision (excluding tail)
    return (gameBoard[newHeadPos].part == 'b' || gameBoard[newHeadPos].part == 'h');
}

// Move snake to new position
void move_snake(int8_t newDirection, bool ateApple) {
    // Update current head to body
    gameBoard[snakeHead].part = 'b';
    gameBoard[snakeHead].direction = newDirection;

    // Calculate new head position
    int8_t newHeadPos = snakeHead + newDirection;

    // Handle grid wrapping (teleport to opposite side)
    if (newHeadPos < 0) newHeadPos += GRID_SIZE * GRID_SIZE;
    if (newHeadPos >= GRID_SIZE * GRID_SIZE) newHeadPos -= GRID_SIZE * GRID_SIZE;

    // Set new head
    snakeHead = newHeadPos;
    gameBoard[snakeHead].part = 'h';

    // If didn't eat apple, move tail
    if (!ateApple) {
        int8_t tailDirection = gameBoard[snakeTail].direction;
        gameBoard[snakeTail].part = '0'; // Clear old tail
        snakeTail += tailDirection;

        // Handle grid wrapping for tail
        if (snakeTail < 0) snakeTail += GRID_SIZE * GRID_SIZE;
        if (snakeTail >= GRID_SIZE * GRID_SIZE) snakeTail -= GRID_SIZE * GRID_SIZE;

        gameBoard[snakeTail].part = 't'; // Set new tail
    }
}

// Display current score
void show_current_score() {
    clear();
    const uint8_t tenth_digit = score / 10;
    const uint8_t unit_digit = score % 10;

    // Display digits side by side
    font_draw(font_list[tenth_digit], scoreColor, 4);
    font_draw(font_list[unit_digit], scoreColor, 0);
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
}

// Display score history
void show_score_history() {
    clear();

    // Display each score with a delay
    for (uint8_t i = 0; i < MAX_SCORES; i++) {
        if (scoreHistory[i] == 0) continue; // Skip empty slots

        const uint8_t tenth_digit = scoreHistory[i] / 10;
        const uint8_t unit_digit = scoreHistory[i] % 10;

        // Display score number (1-10)
        set_color(63, scoreColor); // First LED as indicator
        set_color(62, scoreColor); // Second LED as indicator

        // Display the actual score
        font_draw(font_list[tenth_digit], scoreColor, 4);
        font_draw(font_list[unit_digit], scoreColor, 0);

        WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
        Delay_Ms(1500);
        clear();
        WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
        Delay_Ms(300);
    }
}
void show_score(uint8_t score, color_t color) {
    clear();
    const uint8_t tenth = score / 10;
    const uint8_t unit = score % 10;

    // Show tens digit on left, units on right
    if(tenth > 0) {
        font_draw(font_list[tenth], color, 4);
    }
    font_draw(font_list[unit], color, 0);
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
}

/* ********************************************************************
* *********************************************************************
***********************EEPROM handling*********************************
 * *********************************************************************
 * *********************************************************************
 */
void I2C_Init_Config(void)
{
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOA;    // Enable GPIOA clock
    RCC->APB1PCENR |= RCC_APB1Periph_I2C1;     // Enable I2C1 clock

    // PA1 = I2C1_SCL, PA2 = I2C1_SDA
    GPIOA->CFGLR &= ~((0xF << (4 * 1)) | (0xF << (4 * 2))); // Clear PA1/PA2
    GPIOA->CFGLR |= ((0xB << (4 * 1)) | (0xB << (4 * 2)));  // Set as AF Open-Drain

    I2C1->CTLR1 |= I2C_CTLR1_SWRST;      // Reset I2C1
    I2C1->CTLR1 &= ~I2C_CTLR1_SWRST;     // Release reset

    I2C1->CTLR2 = 36;                    // 36 MHz PCLK1
    I2C1->CKCFGR = 180;                  // 100kHz @36MHz (check datasheet for exact value)
    I2C1->OADDR1 = 0;                    // Own address not used as master
    I2C1->CTLR1 |= I2C_CTLR1_PE;         // Enable I2C1
}
void AT24C256_WriteByte(uint16_t mem_addr, uint8_t data)
{
    // Wait until I2C not busy
    while (I2C1->STAR2 & I2C_STAR2_BUSY);

    I2C1->CTLR1 |= I2C_CTLR1_START; // Generate START
    while (!(I2C1->STAR1 & I2C_STAR1_SB));
    I2C1->DATAR = (AT24C256_ADDR << 1) | 0; // Write address
    while (!(I2C1->STAR1 & I2C_STAR1_ADDR));
    (void)I2C1->STAR2;

    I2C1->DATAR = (mem_addr >> 8) & 0xFF; // High byte
    while (!(I2C1->STAR1 & I2C_STAR1_TXE));
    I2C1->DATAR = mem_addr & 0xFF;        // Low byte
    while (!(I2C1->STAR1 & I2C_STAR1_TXE));
    I2C1->DATAR = data;
    while (!(I2C1->STAR1 & I2C_STAR1_TXE));

    I2C1->CTLR1 |= I2C_CTLR1_STOP;
    for (volatile int wait = 0; wait < 50000; ++wait); // Wait for EEPROM write cycle (max 5ms)
}

uint8_t AT24C256_ReadByte(uint16_t mem_addr)
{
    uint8_t data;

    while (I2C1->STAR2 & I2C_STAR2_BUSY);

    // Send memory address
    I2C1->CTLR1 |= I2C_CTLR1_START;
    while (!(I2C1->STAR1 & I2C_STAR1_SB));
    I2C1->DATAR = (AT24C256_ADDR << 1) | 0; // Write mode
    while (!(I2C1->STAR1 & I2C_STAR1_ADDR));
    (void)I2C1->STAR2;
    I2C1->DATAR = (mem_addr >> 8) & 0xFF;
    while (!(I2C1->STAR1 & I2C_STAR1_TXE));
    I2C1->DATAR = mem_addr & 0xFF;
    while (!(I2C1->STAR1 & I2C_STAR1_TXE));

    // Repeated start
    I2C1->CTLR1 |= I2C_CTLR1_START;
    while (!(I2C1->STAR1 & I2C_STAR1_SB));
    I2C1->DATAR = (AT24C256_ADDR << 1) | 1; // Read mode
    while (!(I2C1->STAR1 & I2C_STAR1_ADDR));
    (void)I2C1->STAR2;

    I2C1->CTLR2 |= I2C_CTLR2_LAST; // Single byte read
    I2C1->CTLR1 &= ~I2C_CTLR1_ACK; // NACK after receive

    while (!(I2C1->STAR1 & I2C_STAR1_RXNE));
    data = I2C1->DATAR;

    I2C1->CTLR1 |= I2C_CTLR1_STOP;

    I2C1->CTLR1 |= I2C_CTLR1_ACK; // Re-enable ACK for next communication

    return data;
}
void save_all_scores() {
    uint16_t addr = Score_start_adress;
    for(uint8_t i = 0;i<MAX_SCORES;i++){
        AT24C256_WriteByte(addr, score);
        addr = addr++;
    }
}

// Load all scores from EEPROM
void load_all_scores() {
    uint16_t addr = Score_start_adress;

    for(uint8_t i = 0; i < MAX_SCORES; i++) {
        scoreHistory[i] = AT24C256_ReadByte(addr);
        addr++; // Move to next slot
        Delay_Ms(5); // Short delay between reads
    }
}

// Show all scores with flashing animation
void show_all_scores() {
    for(uint8_t i = 0; i < MAX_SCORES; i++) {
        // Flash the score position indicator
        set_color(63, scoreColor);
        set_color(62, scoreColor);
        WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
        Delay_Ms(200);

        // Display the score
        show_score(scoreHistory[i], scoreColor);
        Delay_Ms(1500);

        // Clear before next score
        clear();
        WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
        Delay_Ms(300);
    }
}
//
// Save current score to history
/* ********************************************************************
* *********************************************************************
***********************EEPROM handling end*********************************
 * *********************************************************************
 * *********************************************************************
 */
int main(void) {
    // Initialize hardware
    SystemInit();
    ADC_init();
    JOY_setseed_default();
    I2C_Init_Config();
    uint8_t value = 0x5A;
    AT24C256_WriteByte(0x0000, value);
    Delay_Ms(5);
    uint8_t read_val = AT24C256_ReadByte(0x0000);
    // Game loop
    while(1) {
//        load_scores();
        game_init();
        display();
        I2C_init();
        Delay_Ms(1000); // Initial delay

        int8_t currentDirection = -1; // Start moving left
        gameOver = false;

        while(!gameOver) {

            if (JOY_1_pressed()) { // changing hame mode
                          while(JOY_1_pressed()) Delay_Ms(20);
                          game_regime = !game_regime;
                          Delay_Ms(100);

                      }
            // Get input
            currentDirection = get_new_direction(currentDirection);
            speedCounter++;
            // Calculate new head position
            int8_t newHeadPos = snakeHead + currentDirection;

            // Check for collisions
            if (check_collision(newHeadPos)) {
                gameOver = true;
                break;
            }

            // Check if apple was eaten
            bool ateApple = check_apple(newHeadPos);
            if (ateApple) {
                speedCounter -= 5; // Make game slightly slower
                score++;
                generate_apple();
            }

            // Move snake
            move_snake(currentDirection, ateApple);

            // Update display
            display();

            // Calculate dynamic speed (gets faster as score increases)
            uint16_t speedReduction = game_regime ? (speedCounter * 5) : (speedCounter);
            uint16_t currentSpeed = INITIAL_SPEED - speedReduction;
            if (currentSpeed < 100) currentSpeed = 100; // Minimum speed
            Delay_Ms(currentSpeed);
        }

                   // Save the current score
                   if(currentScoreIndex < MAX_SCORES) {
                       scoreHistory[currentScoreIndex] = score;
                       currentScoreIndex++;
                   }

                   // Save all scores to EEPROM


                   // Show current score with flashing
                   for(uint8_t i = 0; i < 3; i++) {
                       show_current_score();
                       Delay_Ms(500);
                       clear();
                       WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
                       Delay_Ms(200);
                   }

                   // Post-game menu
                   uint32_t timeout = 100000; // 10 second timeout
                   while(timeout > 0) {
                       if(JOY_3_pressed()) {
                           while(JOY_3_pressed()) Delay_Ms(10);


                           show_all_scores(); // Show all saved scores
                           save_all_scores();
                           timeout = 10000; // Reset timeout
                       }
                       else if(JOY_5_pressed()) {
                           while(JOY_5_pressed()) Delay_Ms(10);
                           break; // Restart game
                       }

                       Delay_Ms(10);
                       timeout -= 10;
                   }

        /*
        // Game over - save and show score
        show_current_score();
        save_score();

        Delay_Ms(2000);

        // Wait for button press
        while(1) {

            // Show current score with flashing effect
            for(uint8_t i = 0; i < 3; i++) {
                show_current_score();
                Delay_Ms(300);
                clear();
                WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
                Delay_Ms(200);
            }
            show_current_score();

            // More responsive button polling
            uint32_t timeout = 5000; // 5 second timeout
            while(timeout > 0) {
                if(JOY_7_pressed()) {
                    while(JOY_7_pressed()) Delay_Ms(10);
                    show_score_history();
                    timeout = 5000; // Reset timeout
                }
                else if(JOY_5_pressed()) {
                    while(JOY_5_pressed()) Delay_Ms(10);
                    break; // Exit to restart game
                }

                Delay_Ms(10);
                timeout -= 10;
            }

            // If we get here, either timeout or button press
            break; // Restart game

        }
    */
    }


    return 0;
}
