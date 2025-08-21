#define CH32V003_I2C_IMPLEMENTATION
#define WS2812BSIMPLE_IMPLEMENTATION
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./ch32v003fun/ch32v003_i2c.h"
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
//Storage defines
#define EEPROM_ADDR 0x52 // obtained from i2c_scan(), before shifting by 1 bit
#define init_status_addr_begin 0
#define init_status_addr_end 7
#define init_status_reg_size (init_status_addr_end - init_status_addr_begin + 1) // size  = 8
#define init_status_format "  %c "
#define init_status_data (uint8_t *)"IL000001"
#define page_status_addr_begin 8 // page 8
#define page_status_addr_end 511 // page 511
#define page_status_reg_size (page_status_addr_end - page_status_addr_begin + 1) // page size = 504
#define delay 1000
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
//variable for name savings
uint8_t current_user_id = 0;  // Tracks which user is currently active
char current_name[NAME_LENGTH] = {0};
char* new_name;
uint8_t is_storage_initialized(void);   // check if already initialized data, aka init_status_data is set
void init_storage(void);
void reset_storage(void);
uint8_t is_storage_initialized(void);


// Name handling functions
void display_letter(uint8_t letter_idx, color_t color, int delay_ms);
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
//scroling function prototypes
typedef struct {
    const char* text;
    color_t color;
    uint16_t pixel_offset;   // how many columns have shifted so far
    uint16_t total_pixels;   // total columns to scroll through
    bool running;
} Scroller;
static const uint8_t* get_font_data(uint8_t ascii);
static uint16_t text_pixel_length(const char* s);
static inline uint16_t led_index(uint8_t row, uint8_t col);
static uint8_t column_for_text(const char* s, uint16_t col_index);
static void init_scroller(Scroller* sc, const char* text, color_t color);
static bool update_scroller(Scroller* sc);
void scroll_text(const char* text, color_t color, uint32_t speed_ms);
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



char* create_name(void) {
    int i = 0;
    new_name = malloc(NAME_LENGTH+1);
    if (!new_name) return NULL;

    int j = 0;

    while(j < NAME_LENGTH) {
            display_letter(i, letter_color, 600);

            if(JOY_5_pressed()) {
                while(JOY_5_pressed()) Delay_Ms(20);
                new_name[j] = 'A' + i;  // Store the actual letter
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
    if(current_user_id < MAX_USERS+1) {
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
    scroll_text(new_name,scroll_name,200);
    return new_name;
}
void available_names(uint8_t num_name) {
char *name_to_display =  malloc(NAME_LENGTH+1);
            load_name(num_name, name_to_display);
            scroll_text(name_to_display,scoreColor,200);
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
        i2c_read(EEPROM_ADDR, addr+1, I2C_REGADDR_2B, &stored_score, 1);

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
        if (data[0] == i) {
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
    for (uint8_t i = 0; i < MAX_SCORES;i++) {
        if (scoreHistory[i] == 0) continue; // Skip empty slots and the first slot as it always 0 idk why

        // First show which score this is (1-10)
        clear();
        set_color(63, scoreColor); // Indicator LED for score position

        // Display position number (1-10) on right side
        if (i < 9) {
            // Positions 1-9 (display single digit)
            font_draw(font_list[i], scoreColor, 0);
            WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
            Delay_Ms(1000);
        } else {
            // Position 10 (special case)
            font_draw(font_list[1], scoreColor, 4); // '1'
            font_draw(font_list[0], scoreColor, 0);  // '0'
            WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);
            Delay_Ms(1000);
        }


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

/***********************************/
/***********************************/
/*****EEPROM Scores Handling********/
/***********************************/
/***********************************/

/***********************************/
/***********************************/
/*****EEPROM Names Handling********/
/***********************************/
/***********************************/
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

    uint16_t addr = NAME_START_ADDR + ((user_id*20) * NAME_LENGTH);
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

    uint16_t addr = NAME_START_ADDR + ((user_id*20) * NAME_LENGTH);
    uint8_t data[NAME_LENGTH];

    // Read from EEPROM
    i2c_read(EEPROM_ADDR, addr, I2C_REGADDR_2B, data, NAME_LENGTH);

    // Copy to buffer
    strncpy(buffer, (char*)data, NAME_LENGTH);
    buffer[NAME_LENGTH] = '\0';  // Ensure null termination

    printf("Loaded name '%s' for user %d\n", buffer, user_id);
}
// Load all names from EEPROM
void load_all_names(char names[MAX_USERS][NAME_LENGTH]) {
    for (uint8_t i = 0; i < MAX_USERS; i++) {
        load_name(i, names[i]);
    }
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
        for (uint8_t i = 1; i < MAX_SCORES; i++) {
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
       scroll_text(names[best_player], scroll_name, 200);
        Delay_Ms(500);
        clear();


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
/***********************************/
/***********************************/
/*****EEPROM Names Handling********/
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
/*****************************************/
/*****************************************/
/**************EEPROM*********************/
/*****************************************/
/*****************************************/
/*****************************************/
/**************************************************************************/
/***************************************************************************/
/***************************Scroling name************************************/
/***************************************************************************/
/***************************************************************************/

const uint8_t font_char_count = sizeof(font_chars) / sizeof(FontChar);

// Return pointer to 8-column glyph for ASCII, space if not found
static const uint8_t* get_font_data(uint8_t ascii) {
    for (uint8_t i = 0; i < font_char_count; i++) {
        if (font_chars[i].ascii == ascii) return font_chars[i].data;
    }
    return font_chars[0].data; // space
}

// Map (row, col) to LED index. Adjust if your matrix wiring differs.
// Here we assume row-major, left-to-right per row: index = row*8 + col.
// Each glyph is 8 columns + 1 column space. Add 8 trailing blanks so message fully exits.
static uint16_t text_pixel_length(const char* s) {
    if (!s || !*s) return 8;
    uint16_t units = 0;
    for (const char* p = s; *p; ++p) units++;
    return (uint16_t)(units * 9 + 8);
}

// Map row/col to LED index. Adjust if your matrix wiring is different.
static inline uint16_t led_index(uint8_t row, uint8_t col) {
    return (uint16_t)(8-row) * 8u + col; // row-major, left-to-right
}

// Get column bits from the text stream (8-bit column, bit7=top row)
static uint8_t column_for_text(const char* s, uint16_t col_index) {
    uint16_t unit = col_index / 9;   // which character
    uint16_t within = col_index % 9; // 0..7 glyph columns, 8 = spacer
    uint16_t len = 0;
    for (const char* p = s; *p; ++p) len++;
    if (unit >= len) return 0x00;

    const uint8_t* glyph = get_font_data((uint8_t)s[unit]);
    return (within < 8) ? glyph[within] : 0x00;
}

static void init_scroller(Scroller* sc, const char* text, color_t color) {
    sc->text = text ? text : "";
    sc->color = color;
    sc->pixel_offset = 0;
    sc->total_pixels = text_pixel_length(sc->text);
    sc->running = true;
}
static bool update_scroller(Scroller* sc) {
    if (!sc->running) return false;

    clear();

    // Render 8 columns, left-to-right display columns 0..7
    // Left-to-right scroll: new content enters from the left,
    // so display col x pulls from text column (pixel_offset + (7 - x))
    for (uint8_t x = 0; x < 8; ++x) {
        uint16_t text_col = sc->pixel_offset + (uint16_t)(7 - x);
        uint8_t col_bits = column_for_text(sc->text, text_col);
        for (uint8_t row = 0; row < 8; ++row) {
            if ((col_bits >> (7 - row)) & 0x01) {
                set_color(led_index(row, x), sc->color);
            }
        }
    }

    WS2812BSimpleSend(LED_PINS, (uint8_t *)led_array, NUM_LEDS * 3);

    // Advance one column per call
    sc->pixel_offset++;

    // Stop after message completely exits the 8x8 area
    if (sc->pixel_offset >= sc->total_pixels + 8) {
        sc->running = false;
    }

    return sc->running;
}

// speed_ms: delay per column shift (e.g., 40 -> ~25 columns/sec)
void scroll_text(const char* text, color_t color, uint32_t speed_ms) {
    Scroller sc;
    init_scroller(&sc, text, color);

    while (update_scroller(&sc)) {
        Delay_Ms(speed_ms);
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
    //choosing name
    while(1){
        scroll_text("YOUR NAME", appleColor, 120);
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
                                                  available_names(current_user_id);
                                                  break;
                        }
                        if(JOY_4_pressed()){
                                                  while(JOY_4_pressed()) Delay_Ms(20);
                                                  current_user_id = 1;
                                                  available_names(current_user_id);
                                                  break;
                        }
                        if(JOY_7_pressed()){
                                                  while(JOY_7_pressed()) Delay_Ms(20);
                                                  current_user_id = 2;
                                                  available_names(current_user_id);
                                                  break;
                        }

                }
                        clear();
                        Delay_Ms(100);
                        break;

            }
    // Game loop
    while(1) {
        char player_name[NAME_LENGTH+1];
        get_current_user_name(player_name);
        printf("Current player: %s\n", player_name);
        game_init();
        display();
        Delay_Ms(500); // Initial delay

        int8_t currentDirection = -1; // Start moving left
        gameOver = false;

        while(!gameOver) {

            if (JOY_1_pressed()) { // changing game mode
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

            }

            // Save to EEPROM
            save_currentScore_EEPROM(score);

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
                load_scores();
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
                    currentScoreIndex++;
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
            currentScoreIndex++;
        }
    }
    return 0;
}
