#define WS2812BSIMPLE_IMPLEMENTATION
#include "./data/colors.h"
#include "./ch32v003fun/driver.h"
#include "./ch32v003fun/ws2812b_simple.h"
#include "./data/fonts.h"
#include <stdbool.h>

#define LED_PINS GPIOA, 2

// Game constants for snake game
#define INITIAL_SPEED 500  // ms between moves
#define GRID_SIZE 8        // 8x8 grid
#define MAX_SCORES 10      // Number of high scores to keep
//constanst for brekout game
#define INITIAL_SPEED 350
#define MAX_SCORES 8
#define PADDLE_WIDTH 2
#define BRICK_ROWS 2
#define IDX(x, y) ((y) * GRID_SIZE + (x))
typedef struct snakePartDir {
    char part;      // 'h'=head, 'b'=body, 't'=tail, 'a'=apple, '0'=empty
    int8_t direction; // movement direction
} snakePartDir;
typedef struct cell_t{
    char part;
} cell_t;
cell_t gameBoard_1[GRID_SIZE * GRID_SIZE];
typedef struct {
    int8_t x, y;
    int8_t dx, dy;
} ball_t;
// Colors
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
// Game state
snakePartDir gameBoard[GRID_SIZE * GRID_SIZE];
int8_t snakeHead, snakeTail;
uint8_t score;
uint8_t scoreHistory[MAX_SCORES] = {0};
uint8_t currentScoreIndex = 0;
bool gameOver;
bool game_regime = false;
uint16_t speedCounter = 0;
//variables for bounce game
ball_t ball;
int8_t paddleX; // Paddle leftmost position
uint8_t score_1;
uint8_t scoreHistory_1[MAX_SCORES] = {0};
uint8_t currentScoreIndex_1 = 0;
bool gameOver_1 = false;
bool exit = false;
uint8_t number_of_blocks(void);
/***************************************************************************/
/***************************************************************************/
/***************************SNAKE Game**************************************/
/***************************************************************************/
/***************************************************************************/
// Generate a new apple at random empty position
void generate_apple(void) {
    uint8_t applePos;
    do {
        applePos = JOY_random() % (GRID_SIZE * GRID_SIZE);
    } while (gameBoard[applePos].part != '0');
    gameBoard[applePos] = (snakePartDir){'a', 0};
}

// Initialize game board and snake
void snake_game_init() {
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

// Save current score to history
void save_score() {
    // Add score to history
    scoreHistory[currentScoreIndex] = score;
    currentScoreIndex = (currentScoreIndex + 1) % MAX_SCORES;
}
/***************************************************************************/
/***************************************************************************/
/***************************SNAKE Game**************************************/
/***************************************************************************/
/***************************************************************************/




/***************************************************************************/
/***************************************************************************/
/***************************Brekout Game**************************************/
/***************************************************************************/
/***************************************************************************/
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
/***************************************************************************/
/***************************************************************************/
/***************************Brekout Game************************************/
/***************************************************************************/
/***************************************************************************/
int main(void) {
    // Initialize hardware
    SystemInit();
    ADC_init();
    JOY_setseed_default();
while(1){
 if(JOY_3_pressed()){// we start snake game
    // Game loop
    while(1) {
        snake_game_init();
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


        // Game over - save and show score
        save_score();
        show_current_score();
        Delay_Ms(2000);

        // Wait for button press
        while(1) {

            if (JOY_7_pressed()) { // Show score history
                while(JOY_7_pressed()) Delay_Ms(10);
                show_score_history();
                Delay_Ms(1000);
            }
            Delay_Ms(10);

            if (JOY_5_pressed()) { // Restart game
                          while(JOY_5_pressed()) Delay_Ms(10);
                          break;
            }

        }
    }
 }
 if(JOY_9_pressed()){//breakout_game init
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

                 // Wait for action
                 while (1) {
                     if (JOY_7_pressed()) {
                         while (JOY_7_pressed()) Delay_Ms(10);
                         show_score_history_1();
                         Delay_Ms(800);
                         break;
                     }
                     if (JOY_5_pressed()) {
                         while (JOY_5_pressed()) Delay_Ms(10);
                         break;
                     }
                     Delay_Ms(10);
                 }


 }
}
    return 0;
}
