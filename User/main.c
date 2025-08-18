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
#include "data/Clean_code_func/colors_predefined.h"
#include "data/Clean_code_func/fonts8x8.h"
#define LED_PINS GPIOA, 2
// Game constants
#define INITIAL_SPEED 500  // ms between moves
#define GRID_SIZE 8        // 8x8 grid
#define MAX_SCORES 10      // Number of high scores to keep
#define SCORE_SIZE 2 // Each score uses 2 bytes (index + value)
//constanst for brekout game
#define PADDLE_WIDTH 2
#define BRICK_ROWS 2
#define IDX(x, y) ((y) * GRID_SIZE + (x))
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
// initialize file storage structure for 32kb/512pages
// first 8 pages are used for status
//Letters define
#define NAME_LENGTH 3
#define NAME_START_ADDR 0x0100  // Starting address for name storage
#define MAX_USERS 3
#define SCORES_PER_USER MAX_SCORES
#define SCORE_RECORD_SIZE 2  // Each score record is 2 bytes (index + value)
#define USER_SCORE_SPACE (SCORES_PER_USER * SCORE_RECORD_SIZE)
#define SCORE_START_ADDR 0x0008
#define USER_ID_START_ADDR 0x0200
typedef struct snakePartDir {
    char part;      // 'h'=head, 'b'=body, 't'=tail, 'a'=apple, '0'=empty
    int8_t direction; // movement direction
} snakePartDir;
typedef struct cell_t{
    char part;
} cell_t;
cell_t gameBoard_1[GRID_SIZE * GRID_SIZE];
typedef struct ball_t{
    int8_t x, y;
    int8_t dx, dy;
} ball_t;
// Game state
snakePartDir gameBoard[GRID_SIZE * GRID_SIZE];
int8_t snakeHead, snakeTail;
uint8_t score;
uint8_t scoreHistory[MAX_SCORES] = {0};
uint8_t currentScoreIndex = 0;
bool gameOver;
bool game_regime = false;
uint16_t speedCounter = 0;
uint8_t Identifier = 0;// the index of the name choosed
//variables for bounce game
ball_t ball;
int8_t paddleX; // Paddle leftmost position
uint8_t score_1;
uint8_t scoreHistory_1[MAX_SCORES] = {0};
uint8_t currentScoreIndex_1 = 0;
bool gameOver_1 = false;
uint8_t current_user_id = 0;  // Tracks which user is currently active
char current_name[NAME_LENGTH] = {0};
char* new_name;
//function prototypes
// Storage functions
//uint8_t is_page_used(uint16_t page_no); // check if page[x] is already used
uint8_t is_storage_initialized(void);   // check if already initialized data, aka init_status_data is set
// save opcode data to eeprom, paint 0 stored in page ?? (out of page 0 to 511)
//uint16_t calculate_page_no(uint16_t paint_no, uint8_t is_icon);
void init_storage(void);
//void save_paint(uint16_t paint_no, color_t *data, uint8_t is_icon);
//void load_paint(uint16_t paint_no, color_t *data, uint8_t is_icon);
//void set_page_status(uint16_t page_no, uint8_t status);
void reset_storage(void);
//void print_status_storage(void);
//uint8_t is_page_used(uint16_t page_no);
uint8_t is_storage_initialized(void);
//uint16_t calculate_page_no(uint16_t paint_no, uint8_t is_icon);

// Name handling functions
void display_letter(uint8_t letter_idx, color_t color, int delay_ms);
void display_full_message(const uint8_t* letters, uint8_t count, color_t color, uint16_t delay_ms);
void choose_your_name(void);
char* create_name(void);
void save_name(uint8_t user_id, const char* name);
void load_name(uint8_t user_id, char* buffer);
void load_all_names(char names[MAX_USERS][NAME_LENGTH]);

// Score handling functions
void reset_all_scores(void);
void save_currentScore_EEPROM(uint8_t score);
void load_scores(void);
void reveal_all_scores(void);
void show_name_and_highest_score(void);
uint8_t load_id(void);
void save_id(uint8_t user_id);
// Snake game functions
void generate_apple(void);
void game_init(void);
void clear_board(void);
void display(void);
int8_t get_new_direction(int8_t currentDirection);
bool check_apple(int8_t newHeadPos);
bool check_collision(int8_t newHeadPos);
void move_snake(int8_t newDirection, bool ateApple);
void show_current_score(void);
void show_score_history(void);
/*
// Breakout game functions
void clear_board_1(void);
void display_1(void);
void breakout_init(void);
void update_paddle(int8_t dir);
void save_score_1(void);
void show_current_score_1(void);
void show_score_history_1(void);
void move_ball(void);
uint8_t number_of_blocks(void);
bool bricks_remaining(void);
*/
/****************/
/****************/
/*****SNAKE Game********/
/****************/
/****************/
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
    //int8_t row = newHeadPos / GRID_SIZE;

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
//    font_draw(font_list[tenth_digit], scoreColor, 4);
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

/************************/
/************************/
/*****SNAKE Game********/
/***********************/
/***********************/
/***********************************************/
/***********************************************/
/***************Names Hadling*******************/
/***********************************************/
/***********************************************/
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
char* create_name(void) {
    int i = 0;
    new_name = malloc(NAME_LENGTH+1);
    if (!new_name) return NULL;

    int j = 0;
    uint8_t letter_list[NAME_LENGTH+1] = {0};

    while(j < NAME_LENGTH) {
            display_letter(i, letters_color[i], 600);

            if(JOY_5_pressed()) {
                while(JOY_5_pressed()) Delay_Ms(20);
                new_name[j] = 'A' + i;  // Store the actual letter
                letter_list[j] = i;     // Store the letter index for display
                j++;
                i = 0;
                continue;
            }

            // Handle letter cycling
            if(JOY_2_pressed()) {  // Up - next letter
                while(JOY_2_pressed()) Delay_Ms(20);
                i = (i + 1) % 26;
            }
            else if(JOY_8_pressed()) {  // Down - previous letter
                while(JOY_8_pressed()) Delay_Ms(20);
                i = (i - 1 + 26) % 26;
            }

            Delay_Ms(100);
        }

    new_name[NAME_LENGTH] = '\0'; // Null-terminate
    current_user_id = load_id();
    // Save to the next available slot
    if(current_user_id < MAX_USERS) {
        save_name(current_user_id, new_name);
        strncpy(current_name, new_name, NAME_LENGTH);
        current_user_id++;
        save_id(current_user_id);
    } else {
        // No more slots - overwrite the oldest
        current_user_id = 0;
        save_name(current_user_id, new_name);
        strncpy(current_name, new_name, NAME_LENGTH);
        current_user_id = 1;
        save_id(current_user_id);
    }

    display_full_message(letter_list, NAME_LENGTH, scoreColor, 500);
    return new_name;
}
void available_names(uint8_t num_name) {
char *name_to_display =  malloc(NAME_LENGTH+1);
            load_name(num_name, name_to_display);
            for(uint8_t i = 0;i<4;i++){
                uint8_t indx = name_to_display[i] - 'A';
                display_letter(indx, letters_color[indx], 500);
            }
}
/***********************************************/
/***********************************************/
/***************Names Hadling*******************/
/***********************************************/
/***********************************************/


/***********************************/
/***********************************/
/*****EEPROM Scores Handling********/
/***********************************/
/***********************************/
void reset_all_scores(void) {
    // Clear all scores for all users
    reset_storage();

    printf("All scores for all users reset\n");
}
// Save the current score to EEPROM
void save_currentScore_EEPROM(uint8_t score) {
    // Calculate base address for this user's scores
    uint16_t user_base_addr = SCORE_START_ADDR + (current_user_id * USER_SCORE_SPACE);

    // Find either an empty slot (score=0) or the smallest score to replace
    uint8_t slot = 0;
    uint8_t min_score = 255;
    bool found_empty = false;

    for (uint8_t i = 0; i < MAX_SCORES; i++) {
        uint16_t addr = user_base_addr + (i * SCORE_RECORD_SIZE);
        uint8_t stored_score;

        // Read existing score
        i2c_read(EEPROM_ADDR, addr + 1, I2C_REGADDR_2B, &stored_score, 1);

        if (stored_score == 0 && !found_empty) {
            slot = i;
            found_empty = true;
        }
        else if (stored_score < min_score) {
            min_score = stored_score;
            slot = i;
        }
    }

    // Only save if new score is higher than the smallest existing score or we found an empty slot
    if (found_empty || score > min_score) {
        uint16_t addr = user_base_addr + (slot * SCORE_RECORD_SIZE);
        uint8_t data[SCORE_RECORD_SIZE] = {slot, score};

        i2c_write(EEPROM_ADDR, addr, I2C_REGADDR_2B, data, SCORE_RECORD_SIZE);
        Delay_Ms(3); // EEPROM write delay

        printf("User %d: Score %d saved to slot %d at addr %d\n",
              current_user_id, score, slot, addr);
    }
}
void save_id(uint8_t user_id){
    uint16_t addr = USER_ID_START_ADDR;
    i2c_write(EEPROM_ADDR, addr, I2C_REGADDR_2B, &user_id, 1);
    Delay_Ms(3);
    printf("User count %d saved\n", user_id);
}
uint8_t load_id (void){
    uint16_t addr = USER_ID_START_ADDR;
    uint8_t user_count = 0;
    i2c_read(EEPROM_ADDR, addr, I2C_REGADDR_2B, &user_count, 1);
    printf("Loaded user count: %d\n", user_count);
    return user_count;
}
void save_name(uint8_t user_id, const char* name) {
    if (user_id >= MAX_USERS) return;

    uint16_t addr = NAME_START_ADDR + (user_id * NAME_LENGTH);
    uint8_t data[NAME_LENGTH];

    // Copy name to buffer
    memset(data, 0, NAME_LENGTH);
    strncpy((char*)data, name, NAME_LENGTH);

    // Write to EEPROM
    i2c_write(EEPROM_ADDR, addr, I2C_REGADDR_2B, data, NAME_LENGTH);
    Delay_Ms(3);

    printf("Name '%s' saved for user %d at addr %d\n", name, user_id, addr);
}

// Modified load_name function
void load_name(uint8_t user_id, char* buffer) {
    if (user_id >= MAX_USERS) {
        buffer[0] = '\0';
        return;
    }

    uint16_t addr = NAME_START_ADDR + (user_id * NAME_LENGTH);
    uint8_t data[NAME_LENGTH];

    // Read from EEPROM
    i2c_read(EEPROM_ADDR, addr, I2C_REGADDR_2B, data, NAME_LENGTH);

    // Copy to buffer
    strncpy(buffer, (char*)data, NAME_LENGTH);
    buffer[NAME_LENGTH] = '\0';  // Ensure null termination

    printf("Loaded name '%s' for user %d\n", buffer, user_id);
}

// New function to get current user's name
void get_current_user_name(char* buffer) {
    if(current_user_id == 0) {
        // No users yet, return default
        strcpy(buffer, "PLAYER");
        return;
    }
    load_name(current_user_id, buffer);  // Load last saved user
}

// Load all names from EEPROM
void load_all_names(char names[MAX_USERS][NAME_LENGTH]) {
    for (uint8_t i = 0; i < MAX_USERS; i++) {
        load_name(i, names[i]);
    }
}

void load_scores(void) {
    // Clear current score history
    memset(scoreHistory, 0, sizeof(scoreHistory));
    currentScoreIndex = 0;

    // Calculate base address for this user's scores
    uint16_t user_base_addr = SCORE_START_ADDR + (current_user_id * USER_SCORE_SPACE);

    // Read all score slots for this user
    for (uint8_t i = 0; i < MAX_SCORES; i++) {
        uint16_t addr = user_base_addr + (i * SCORE_RECORD_SIZE);
        uint8_t data[SCORE_RECORD_SIZE];

        i2c_read(EEPROM_ADDR, addr, I2C_REGADDR_2B, data, SCORE_RECORD_SIZE);

        // Validate the data (index should match slot)
        if (data[0] == i && data[1] != 0) {
            scoreHistory[i] = data[1];
            currentScoreIndex++;
        }
    }

    printf("Loaded %d scores for user %d\n", currentScoreIndex, current_user_id);
}
void reveal_all_scores(void) {
    // Load scores from EEPROM first to ensure we have current data
    load_scores();

    // Clear the display
    clear();
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
    Delay_Ms(500);

    // Display each score with its position
    for (uint8_t i = 0; i < MAX_SCORES; i++) {
        if (scoreHistory[i] == 0) continue; // Skip empty slots

        // First show which score this is (1-10)
        clear();
        set_color(63, scoreColor); // Indicator LED for score position

        // Display position number (1-10) on right side
        if (i < 9) {
            // Positions 1-9 (display single digit)
            font_draw(font_list[i+1], scoreColor, 0); // +1 because positions start at 1
        } else {
            // Position 10 (special case)
            font_draw(font_list[1], scoreColor, 4); // '1'
            font_draw(font_list[0], scoreColor, 0);  // '0'
        }
        WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
        Delay_Ms(1000);

        // Then show the actual score value
        clear();
        const uint8_t tenth_digit = scoreHistory[i] / 10;
        const uint8_t unit_digit = scoreHistory[i] % 10;

        // Display score value
        if (tenth_digit > 0) {
            font_draw(font_list[tenth_digit], scoreColor, 4); // Tens place
        }
        font_draw(font_list[unit_digit], scoreColor, 0);      // Units place
        WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
        Delay_Ms(1500);

        // Brief pause between scores
        clear();
        WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
        Delay_Ms(300);
    }

    // Final clear
    clear();
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
}
// this is shit code function that works, I will think how to rewrite
void show_name_and_highest_score(void) {
    char names[MAX_USERS][NAME_LENGTH];
    uint8_t highest_scores[MAX_USERS] = {0};
    uint8_t best_player = 0;
    uint8_t global_high_score = 0;

    // Load all names and find each player's highest score
    for (uint8_t player = 0; player < MAX_USERS; player++) {
        load_name(player, names[player]);

        // Temporarily set current_user_id to load this player's scores
        uint8_t prev_user = current_user_id;
        current_user_id = player;
        load_scores();

        // Find this player's highest score
        for (uint8_t i = 0; i < MAX_SCORES; i++) {
            if (scoreHistory[i] > highest_scores[player]) {
                highest_scores[player] = scoreHistory[i];
            }
        }

        // Check if this is the new global high score
        if (highest_scores[player] > global_high_score) {
            global_high_score = highest_scores[player];
            best_player = player;
        }

        // Restore current user
        current_user_id = prev_user;
    }

    // Display the best player and their score
    clear();
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
    Delay_Ms(500);

    // Display player name
    for (int i = 0; i < 3; i++) {
        uint8_t plind = names[best_player][i] - 'A'; // Convert char to letter index
        display_letter(plind, letters_color[plind], 500);
        Delay_Ms(500);
        clear();
    }

    // Display highest score
    const uint8_t tenth_digit = global_high_score / 10;
    const uint8_t unit_digit = global_high_score % 10;

    clear();
    if (tenth_digit > 0) {
        font_draw(font_list[tenth_digit], scoreColor, 4);
    }
    font_draw(font_list[unit_digit], scoreColor, 0);
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
    Delay_Ms(2000);

    // Clear display when done
    clear();
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
}

/*********************************************/
/*********************************************/
/***************Brekaout Score handling*******/
/*********************************************/
/*********************************************/
/*
void save_currentScore_EEPROM_1(uint8_t score) {
    // Calculate base address for this user's breakout scores
    // We'll use a different address space (0x0208) for breakout scores
    uint16_t user_base_addr = 0x0208 + (current_user_id * USER_SCORE_SPACE);

    // Find either an empty slot (score=0) or the smallest score to replace
    uint8_t slot = 0;
    uint8_t min_score = 255;
    bool found_empty = false;

    for (uint8_t i = 0; i < MAX_SCORES; i++) {
        uint16_t addr = user_base_addr + (i * SCORE_RECORD_SIZE);
        uint8_t stored_score;

        // Read existing score
        i2c_read(EEPROM_ADDR, addr + 1, I2C_REGADDR_2B, &stored_score, 1);

        if (stored_score == 0 && !found_empty) {
            slot = i;
            found_empty = true;
        }
        else if (stored_score < min_score) {
            min_score = stored_score;
            slot = i;
        }
    }

    // Only save if new score is higher than the smallest existing score or we found an empty slot
    if (found_empty || score > min_score) {
        uint16_t addr = user_base_addr + (slot * SCORE_RECORD_SIZE);
        uint8_t data[SCORE_RECORD_SIZE] = {slot, score};

        i2c_write(EEPROM_ADDR, addr, I2C_REGADDR_2B, data, SCORE_RECORD_SIZE);
        Delay_Ms(3); // EEPROM write delay

        printf("Breakout: User %d: Score %d saved to slot %d at addr %d\n",
              current_user_id, score, slot, addr);
    }
}

void load_scores_1(void) {
    // Clear current score history
    memset(scoreHistory_1, 0, sizeof(scoreHistory_1));
    currentScoreIndex_1 = 0;

    // Calculate base address for this user's breakout scores
    uint16_t user_base_addr = 0x0208 + (current_user_id * USER_SCORE_SPACE);

    // Read all score slots for this user
    for (uint8_t i = 0; i < MAX_SCORES; i++) {
        uint16_t addr = user_base_addr + (i * SCORE_RECORD_SIZE);
        uint8_t data[SCORE_RECORD_SIZE];

        i2c_read(EEPROM_ADDR, addr, I2C_REGADDR_2B, data, SCORE_RECORD_SIZE);

        // Validate the data (index should match slot)
        if (data[0] == i && data[1] != 0) {
            scoreHistory_1[i] = data[1];
            currentScoreIndex_1++;
        }
    }

    printf("Loaded %d breakout scores for user %d\n", currentScoreIndex_1, current_user_id);
}

void reveal_all_scores_1(void) {
    // Load scores from EEPROM first to ensure we have current data
    load_scores_1();

    // Clear the display
    clear();
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
    Delay_Ms(500);

    // Display each score with its position
    for (uint8_t i = 0; i < MAX_SCORES; i++) {
        if (scoreHistory_1[i] == 0) continue; // Skip empty slots

        // First show which score this is (1-10)
        clear();
        set_color(63, scoreColor); // Indicator LED for score position

        // Display position number (1-10) on right side
        if (i < 9) {
            // Positions 1-9 (display single digit)
            font_draw(font_list[i+1], scoreColor, 0); // +1 because positions start at 1
        } else {
            // Position 10 (special case)
            font_draw(font_list[1], scoreColor, 4); // '1'
            font_draw(font_list[0], scoreColor, 0);  // '0'
        }
        WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
        Delay_Ms(1000);

        // Then show the actual score value
        clear();
        const uint8_t tenth_digit = scoreHistory_1[i] / 10;
        const uint8_t unit_digit = scoreHistory_1[i] % 10;

        // Display score value
        if (tenth_digit > 0) {
            font_draw(font_list[tenth_digit], scoreColor, 4); // Tens place
        }
        font_draw(font_list[unit_digit], scoreColor, 0);      // Units place
        WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
        Delay_Ms(1500);

        // Brief pause between scores
        clear();
        WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
        Delay_Ms(300);
    }

    // Final clear
    clear();
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
}

void show_name_and_highest_score_1(void) {
    char names[MAX_USERS][NAME_LENGTH];
    uint8_t highest_scores[MAX_USERS] = {0};
    uint8_t best_player = 0;
    uint8_t global_high_score = 0;

    // Load all names and find each player's highest score
    for (uint8_t player = 0; player < MAX_USERS; player++) {
        load_name(player, names[player]);

        // Temporarily set current_user_id to load this player's scores
        uint8_t prev_user = current_user_id;
        current_user_id = player;
        load_scores_1();

        // Find this player's highest score
        for (uint8_t i = 0; i < MAX_SCORES; i++) {
            if (scoreHistory_1[i] > highest_scores[player]) {
                highest_scores[player] = scoreHistory_1[i];
            }
        }

        // Check if this is the new global high score
        if (highest_scores[player] > global_high_score) {
            global_high_score = highest_scores[player];
            best_player = player;
        }

        // Restore current user
        current_user_id = prev_user;
    }

    // Display the best player and their score
    clear();
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
    Delay_Ms(500);

    // Display player name
    for (int i = 0; i < 3; i++) {
        uint8_t plind = names[best_player][i] - 'A'; // Convert char to letter index
        display_letter(plind, letters_color[plind], 500);
        Delay_Ms(500);
        clear();
    }

    // Display highest score
    const uint8_t tenth_digit = global_high_score / 10;
    const uint8_t unit_digit = global_high_score % 10;

    clear();
    if (tenth_digit > 0) {
        font_draw(font_list[tenth_digit], scoreColor, 4);
    }
    font_draw(font_list[unit_digit], scoreColor, 0);
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
    Delay_Ms(2000);

    // Clear display when done
    clear();
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
}
*/
// Update the breakout game loop to use these functions:
// In the breakout game section (after gameOver_1 = true), replace:
/*********************************************/
/*********************************************/
/***************Brekaout Score handling*******/
/*********************************************/
/*********************************************/
/***********************************/
/***********************************/
/*****EEPROM Scores Handling********/
/***********************************/
/***********************************/

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
/*
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
*/
// Show all scores with flashing animation

//
// Save current score to history
/*****************************************/
/*****************************************/
/**************EEPROM*********************/
/*****************************************/
/*****************************************/
/*****************************************/

/***************************************************************************/
/***************************************************************************/
/***************************Brekout Game**************************************/
/***************************************************************************/
/***************************************************************************/
/*
void clear_board_1(void) {
    for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++)
        gameBoard_1[i].part = '0';
}

void display_1(void) {
    clear();
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            int i = IDX(x, y);
            switch (gameBoard_1[i].part) {
                case 'x': set_color(i, brickColor); break;//brick
                case 'p': set_color(i, paddleColor); break;//paddle
                case 'b': set_color(i, ballColor); break;//ball
                case '0': set_color(i, blankColor); break;//blank
                default: set_color(i, blankColor); break;
            }
        }
    }
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
}

void breakout_init(void) {
    clear_board_1();
    // Place bricks
    for (int y = 8; y >= 8-BRICK_ROWS; y--)
        for (int x = 0; x < GRID_SIZE; x++)
            gameBoard_1[IDX(x, y)].part = 'x';
    // Place paddle (bottom row)
    paddleX = 3;
    for (int x = paddleX; x < paddleX + PADDLE_WIDTH; x++)
        gameBoard_1[IDX(x, 0)].part = 'p';
    // Place ball (above paddle, center)
    ball.x = 4;
    ball.y = 2;
    ball.dx = 1;
    ball.dy = 1;
    gameBoard_1[IDX(ball.x, ball.y)].part = 'b';
    score_1 = 0;
    gameOver_1 = false;
}

void update_paddle(int8_t dir) {
    // Remove paddle from board
    for (int x = 0; x < GRID_SIZE; x++)
        gameBoard_1[IDX(x, 0)].part = '0';
    // Update paddleX
    paddleX += dir;
    if (paddleX <= 0) paddleX = 0;
    if (paddleX >=GRID_SIZE - PADDLE_WIDTH) paddleX = GRID_SIZE - PADDLE_WIDTH;
    // Draw paddle

    for (int x = paddleX; x < paddleX + PADDLE_WIDTH; x++)
        gameBoard_1[IDX(x, 0)].part = 'p';
}

void save_score_1(void) {
    score_1 = 16-number_of_blocks();
    scoreHistory_1[currentScoreIndex] = score_1;
    currentScoreIndex = (currentScoreIndex + 1) % MAX_SCORES;
}

void show_current_score_1(void) {
    clear();
    const uint8_t tenth_digit = score_1 / 10;
    const uint8_t unit_digit = score_1 % 10;
    font_draw(font_list[tenth_digit], scoreColor, 4);
    font_draw(font_list[unit_digit], scoreColor, 0);
    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
}

void show_score_history_1(void) {
    clear();
    for (uint8_t i = 0; i < MAX_SCORES; i++) {
        if (scoreHistory_1[i] == 0) continue;
        const uint8_t tenth_digit = scoreHistory_1[i] / 10;
        const uint8_t unit_digit = scoreHistory_1[i] % 10;
        set_color(63, scoreColor);
        set_color(62, scoreColor);
        font_draw(font_list[tenth_digit], scoreColor, 4);
        font_draw(font_list[unit_digit], scoreColor, 0);
        WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
        Delay_Ms(1200);
        clear();
        WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
        Delay_Ms(250);
    }
}

void move_ball(void) {
    // Remove ball from current position
    gameBoard_1[IDX(ball.x, ball.y)].part = '0';

    int8_t nx = ball.x + ball.dx;
    int8_t ny = ball.y + ball.dy;

    // Wall bounce (left/right)
    if (nx < 0 || nx >= GRID_SIZE) {
        ball.dx *= -1;
        nx = ball.x + ball.dx;
    }
    // Ceiling bounce
    if (ny == 8) {
        ball.dy *= -1;
        ny = ball.y + ball.dy;
    }
    // Brick collision
    if (ny > 8-BRICK_ROWS && gameBoard_1[IDX(nx, ny)].part == 'x') {
        gameBoard_1[IDX(nx, ny)].part = '0';
        ball.dy *= -1;
        ny = ball.y + ball.dy;
    }
    // Paddle collision
    if (ny == 0) {
        if (nx >= paddleX && nx < paddleX + PADDLE_WIDTH) {
            ball.dy *= -1;
            ny = ball.y + ball.dy;
            // (Optional) Control dx based on hit: edge = curve, center = straight
            if (nx == paddleX) ball.dx = -1;
            else if (nx == paddleX + PADDLE_WIDTH - 1) ball.dx = 1;
        } else {
            gameOver_1 = true;
            return;
        }
    }
    // Move ball
    ball.x = nx;
    ball.y = ny;
    gameBoard_1[IDX(ball.x, ball.y)].part = 'b';
}
uint8_t number_of_blocks(void) {
    uint8_t local_score = 0;
    const uint8_t total_bricks = GRID_SIZE * BRICK_ROWS;
    const uint8_t start_index = GRID_SIZE * GRID_SIZE - 1; // Last index (63 for 8x8 grid)

    // Calculate destroyed bricks (where part == '0')
    for(int i = 0; i < total_bricks; i++) {
        if (gameBoard_1[start_index - i].part == 'x') {
            local_score++;
        }
    }
    return local_score;
}
bool bricks_remaining(void) {
    for (int i = 0; i < GRID_SIZE * BRICK_ROWS; i++)
        if (gameBoard_1[(64-i)].part == 'x')
            return true;
    return false;
}
*/
/***************************************************************************/
/***************************************************************************/
/***************************Brekout Game************************************/
/***************************************************************************/
/***************************************************************************/


/***************************************************************************/
/***************************************************************************/
/***************************Scroling name************************************/
/***************************************************************************/
/***************************************************************************/
extern const uint8_t font_8x8[256][8];

typedef struct {
    uint8_t buffer[8][16];  // Double width buffer for scrolling (8 rows, 16 columns)
    uint16_t cursor;
    float frameWaitTime;
} ScrollingText8x8Reveal;

void initReveal(ScrollingText8x8Reveal *reveal) {
    memset(reveal->buffer, 0, sizeof(reveal->buffer));
    reveal->frameWaitTime = 1000.0 / (60 * 8);  // 60fps * 8 rows
    reveal->cursor = 0;
}

static void byteToBinary(uint8_t byteIn, uint8_t binaryOut[8]) {
    for (uint8_t n = 0; n < 8; n++) {
        binaryOut[7 - n] = (byteIn >> n) & 1;
    }
}

void revealCharacter(ScrollingText8x8Reveal *reveal, uint8_t character, uint32_t duration, color_t color) {
    uint8_t charMatrix[8][8];

    // Convert font data to binary matrix
    for (uint8_t row = 0; row < 8; row++) {
        uint8_t binary[8];
        byteToBinary(font_8x8[character][row], binary);
        for (uint8_t col = 0; col < 8; col++) {
            charMatrix[row][col] = binary[col];
        }
    }

    // Add character to buffer (right side)
    uint8_t bufferIndex = (reveal->cursor < 8) ? 1 : 0;
    for (uint8_t row = 0; row < 8; row++) {
        for (uint8_t col = 0; col < 8; col++) {
            reveal->buffer[row][col + (bufferIndex * 8)] = charMatrix[row][col];
        }
    }

    // Scroll animation
    for (uint8_t step = 0; step < 8; step++) {
        uint32_t currentDuration = 0;
        while (currentDuration < duration) {
            clear();

            // Display current window
            for (uint8_t row = 0; row < 8; row++) {
                for (uint8_t col = 0; col < 8; col++) {
                    uint8_t bufCol = (reveal->cursor + col) % 16;
                    if (reveal->buffer[row][bufCol]) {
                        set_color(row * 8 + col, color);
                    }
                }
            }

            WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
            Delay_Ms(reveal->frameWaitTime);
            currentDuration += reveal->frameWaitTime;
        }

        reveal->cursor = (reveal->cursor + 1) % 16;
    }
}

void revealText(ScrollingText8x8Reveal *reveal, const char *text, uint32_t speed, color_t color) {
    for (uint32_t i = 0; text[i] != '\0'; i++) {
        revealCharacter(reveal, text[i], speed, color);
    }
}
/***************************************************************************/
/***************************************************************************/
/***************************Scroling name************************************/
/***************************************************************************/
/***************************************************************************/
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
    current_user_id = load_id();
    if(current_user_id == 0) {
            // First time setup - initialize user counter
         save_id(0);
    }
        // Load current user's name if available
   if(current_user_id > 0) {
         load_name(current_user_id - 1, current_name);
   }
   ScrollingText8x8Reveal scroller;
   initReveal(&scroller);

   // Display scrolling text
   revealText(&scroller, "HELLO", 100, scoreColor);  // 100ms per character
    //choosing name
    while(1){
                choose_your_name();
                Delay_Ms(1000);
                while(1){
                        if(JOY_5_pressed()){
                            while(JOY_5_pressed()) Delay_Ms(20);
                            create_name();
                            break;
                        }
                        if(JOY_1_pressed()){
                                                  while(JOY_1_pressed()) Delay_Ms(20);
                                                  current_user_id = 0;
                                                  available_names(0);
                                                  break;
                        }
                        if(JOY_4_pressed()){
                                                  while(JOY_4_pressed()) Delay_Ms(20);
                                                  current_user_id = 1;
                                                  available_names(1);
                                                  break;
                        }
                        if(JOY_7_pressed()){
                                                  while(JOY_7_pressed()) Delay_Ms(20);
                                                  current_user_id = 2;
                                                  available_names(2);
                                                  break;
                        }

                }
                        clear();
                        Delay_Ms(1000);
                        break;

            }
    // Game loop
    while(1) {
        char player_name[NAME_LENGTH+1];
        get_current_user_name(player_name);
        printf("Current player: %s\n", player_name);
        if(JOY_3_pressed()){// we start snake game
//      load_scores();
        game_init();
        display();
        load_scores(); // Load saved scores at start of the game
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

            // Save to EEPROM
            save_currentScore_EEPROM(score);
            save_name(Identifier, new_name);

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
                else if (JOY_7_pressed()) {
                    while(JOY_7_pressed()) Delay_Ms(10); // Debounce
                    show_name_and_highest_score();
                    Delay_Ms(500); // Prevent immediate re-trigger
                }
                else if(JOY_5_pressed()) {
                    while(JOY_5_pressed()) Delay_Ms(10);
                    break; // Restart game
                }
                else if (JOY_9_pressed()) {
                    while(JOY_9_pressed()) Delay_Ms(10); // Debounce
                    reveal_all_scores();
                    Delay_Ms(500); // Prevent immediate re-trigger
                }

                Delay_Ms(10);
                timeout -= 10;
            }
        }
        }
     /*   if(JOY_9_pressed()){//breakout_game init
             while (JOY_9_pressed()) Delay_Ms(10);
                         breakout_init();
                         display_1();
                         Delay_Ms(1000);

                         int16_t speed = INITIAL_SPEED;
                         while (!gameOver_1) {
                             // Move paddle
                             if (JOY_4_pressed() && paddleX < GRID_SIZE - PADDLE_WIDTH) {
                                 update_paddle(1);
                                 while (JOY_4_pressed()) Delay_Ms(5);
                             }
                             if (JOY_6_pressed() && paddleX >0 ) {
                                 update_paddle(-1);
                                 while (JOY_6_pressed()) Delay_Ms(5);
                             }

                             move_ball();
                             display_1();

                             if (!bricks_remaining()) {
                                 // Win!
                                 gameOver_1 = true;
                             }

                             Delay_Ms(speed);
                             if (speed > 100 && score > 0) speed -= 2;
                         }

                         // Game Over
                         save_score_1();
                         show_current_score_1();
                         Delay_Ms(200);
                         if(gameOver_1) {
                             // Save the current score
                             if(currentScoreIndex_1 < MAX_SCORES) {
                                 scoreHistory_1[currentScoreIndex_1] = score_1;
                                 currentScoreIndex_1++;
                             }

                             // Save to EEPROM
                             save_currentScore_EEPROM_1(score_1);

                             // Show current score with flashing
                             for(uint8_t i = 0; i < 3; i++) {
                                 show_current_score_1();
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
                                 else if (JOY_7_pressed()) {
                                     while(JOY_7_pressed()) Delay_Ms(10); // Debounce
                                     show_name_and_highest_score_1();
                                     Delay_Ms(500); // Prevent immediate re-trigger
                                 }
                                 else if(JOY_5_pressed()) {
                                     while(JOY_5_pressed()) Delay_Ms(10);
                                     break; // Restart game
                                 }
                                 else if (JOY_9_pressed()) {
                                     while(JOY_9_pressed()) Delay_Ms(10); // Debounce
                                     reveal_all_scores_1();
                                     Delay_Ms(500); // Prevent immediate re-trigger
                                 }

                                 Delay_Ms(10);
                                 timeout -= 10;
                             }
                         }


         }
         */
    }
    return 0;
}
