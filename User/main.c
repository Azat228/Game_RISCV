#define CH32V003_I2C_IMPLEMENTATION
#define WS2812BSIMPLE_IMPLEMENTATION
#include <stdbool.h>
#include <stdio.h>
#include "./ch32v003fun/ch32v003_i2c.h"
#include "./data/colors.h"
#include "./ch32v003fun/driver.h"
#include "./data/fonts.h"
#include "./data/music.h"
#include "./ch32v003fun/ws2812b_simple.h"
#define LED_PINS GPIOA, 2
// Game constants
#define INITIAL_SPEED 500  // ms between moves
#define GRID_SIZE 8        // 8x8 grid
#define MAX_SCORES 10      // Number of high scores to keep
#define SCORE_SIZE 2 // Each score uses 2 bytes (index + value)
//Storage defines
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
#define SCORE_START_ADDR 0x0000
// initialize file storage structure for 32kb/512pages
// first 8 pages are used for status
void init_storage(void);
void save_paint(uint16_t paint_no, color_t * data, uint8_t is_icon);    // save paint data to eeprom, paint 0 stored in page ?? (out of page 0 to 511)
void load_paint(uint16_t paint_no, color_t * data, uint8_t is_icon);    // load paint data from eeprom, paint 0 stored in page ?? (out of page 0 to 511)
void set_page_status(uint16_t page_no, uint8_t status); // set page status to 0 or 1
void reset_storage(void);   // reset to default storage status
void print_status_storage(void);    // print storage data to console
uint8_t is_page_used(uint16_t page_no); // check if page[x] is already used
uint8_t is_storage_initialized(void);   // check if already initialized data, aka init_status_data is set
// save opcode data to eeprom, paint 0 stored in page ?? (out of page 0 to 511)
uint16_t calculate_page_no(uint16_t paint_no, uint8_t is_icon);


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
void save_currentScore_EEPROM(uint8_t Saved_value){



}
void reset_all_scores(void){
    printf("All scores are reseted");
    reset_storage();
}
/*****************************************/
/*****************************************/
/**************EEPROM*********************/
/*****************************************/
/*****************************************/
/*****************************************/
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
// Save current score to history
/*****************************************/
/*****************************************/
/**************EEPROM*********************/
/*****************************************/
/*****************************************/
/*****************************************/
//Sorry for shit code:(, especially part with EEPROM
int main(void) {
    // Initialize hardware
    SystemInit();
    ADC_init();
    JOY_setseed_default();
    printf("Lets start debug\n");
    i2c_init();
    init_storage();
    JOY_sound(1000, 100);
    // Game loop
    while(1) {
//        load_scores();
        game_init();
        display();
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

        if(gameOver) {
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
                   uint32_t timeout = 10000; // 10 second timeout
                   while(timeout > 0) {
                       if(JOY_3_pressed()) {
                           while(JOY_3_pressed()) Delay_Ms(10);
                           reset_all_scores();
                           timeout = 10000; // Reset timeout
                       }
                       else if(JOY_5_pressed()) {
                           while(JOY_5_pressed()) Delay_Ms(10);
                           break; // Restart game
                       }

                       Delay_Ms(10);
                       timeout -= 10;
                   }
               }
    }
    return 0;
}
