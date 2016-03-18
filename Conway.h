#define DEBUG false
#define BRIGHTNESS 10

#define CELL_STATE_ALIVE 1
#define CELL_STATE_DEAD 0

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#ifndef PSTR
    #define PSTR /* Make Arduino Due happy */
#endif


class Conway {
    private:
        unsigned char ** board;
        unsigned char ** board_next;
        unsigned char ** board_previous;

        unsigned char i_col_count;
        unsigned char i_col;
        unsigned char i_row;

        bool any_cells_alive;

        Adafruit_NeoMatrix * led_matrix;

        unsigned true_width;
        unsigned num_columns;
        unsigned width;
        unsigned height;

        void _print(bool current);
        void _randomize();

    public:
        Conway(int num_pixels_width, int num_pixels_height, int num_boards_x, int num_boards_y, int pin);

        void init();
        void update();
        void draw();
};

Conway::Conway(int num_pixels_width, int num_pixels_height, int num_boards_x, int num_boards_y, int pin){

    led_matrix = new Adafruit_NeoMatrix(
        num_pixels_width,
        num_pixels_height,
        num_boards_x,
        num_boards_y,
        pin,
        NEO_TILE_TOP + NEO_TILE_LEFT + NEO_TILE_ROWS    + NEO_TILE_PROGRESSIVE +
        NEO_TILE_TOP + NEO_TILE_LEFT + NEO_TILE_COLUMNS + NEO_TILE_ZIGZAG,
        NEO_GRB + NEO_KHZ800
    );

    true_width = led_matrix->width();
    num_columns = true_width / 8;
    width = 8;
    height = led_matrix->height();


    /**
     * Detect largest width of % 8
     */
    // if(width % 8 != 0){
    //     return;
    // }

    any_cells_alive = false;
    board = new unsigned char * [true_width / 8];
    board_next = new unsigned char * [true_width / 8];

    for (i_col_count = 0; i_col_count < num_columns; i_col_count++) {
        board[i_col_count] = new unsigned char [height];
        board_next[i_col_count] = new unsigned char [height];

        for(i_row = 0; i_row < height; i_row++){
            board[i_col_count][i_row] = NULL;
            board_next[i_col_count][i_row] = NULL;
        }
    }
    for (i_col_count = 0; i_col_count < num_columns; i_col_count++) {
        for(i_row = 0; i_row < height; i_row++) {
            board[i_col_count][i_row] = CELL_STATE_DEAD;
            board_next[i_col_count][i_row] = CELL_STATE_DEAD;
        }
    }
    // for (i_col_count = 0; i_col_count < num_columns; i_col_count++) {
    //     for (i_col = 0; i_col < width; i_col++) {
    //         for(i_row = 0; i_row < height; i_row++) {
    //             board[i_col_count][i_row] = (board[i_col_count][i_row] | (CELL_STATE_DEAD << i_col));
    //             board_next[i_col_count][i_row] = (board_next[i_col_count][i_row] | (CELL_STATE_DEAD << i_col));

    //             if (i_col % 2 == 0) {
    //                 board_next[i_col_count][i_row] = (board_next[i_col_count][i_row] | (CELL_STATE_ALIVE << i_col));
    //             }
    //         }
    //     }
    // }
}

void Conway::init() {
    led_matrix->begin();
    led_matrix->setBrightness(BRIGHTNESS);
    led_matrix->fillScreen(led_matrix->Color(0, 0, 0));
    led_matrix->show();

    _randomize();
}


void Conway::update(){

    /**
     * Check if anything on board
     */
    if(!any_cells_alive){
        if(DEBUG) Serial.println("NO MORE CELLS CELL_STATE_ALIVE");
        _randomize();
        return;
    }


    /**
     * Reset board next
     */
    for (i_col_count = 0; i_col_count < num_columns; i_col_count++) {
        for(i_row = 0; i_row < height; i_row++) {
            board_next[i_col_count][i_row] = 0;
        }
    }

    int num_active_surrounding = 0;
    int bound_col_min = 0;
    int bound_col_max = 0;
    int bound_row_min = 0;
    int bound_row_max = 0;
    int i_col_check = 0;
    int i_row_check = 0;

    any_cells_alive = false;


    /**
     * Compute what is alive in the next round
     */
    for (i_col_count = 0; i_col_count < num_columns; i_col_count++) {
        for(i_col = 0; i_col < width; i_col++){
            for(i_row = 0; i_row < height; i_row++){


                num_active_surrounding = 0;
                bound_col_min = i_col;
                bound_col_max = i_col;
                bound_row_min = i_row;
                bound_row_max = i_row;

                /**
                 * Check left, right, up, down
                 */
                if(i_col > 0)           bound_col_min -= 1;
                if(i_col < width - 1)   bound_col_max += 1;
                if(i_row > 0)           bound_row_min -= 1;
                if(i_row < height - 1)  bound_row_max += 1;

                /**
                 * Check Sorrounding
                 */
                for(i_col_check = bound_col_min; i_col_check <= bound_col_max; i_col_check++){

                    if (num_active_surrounding > 3) break;

                    for(i_row_check = bound_row_min; i_row_check <= bound_row_max; i_row_check++){

                        /**
                         * If not this spot
                         */
                        if(i_col_check == i_col && i_row_check == i_row) {}
                        else {
                            /**
                             * If active
                             */
                            if(((board[i_col_count][i_row_check] >> i_col_check) & 1) == CELL_STATE_ALIVE) num_active_surrounding += 1;

                            if (num_active_surrounding > 3) break;
                        }
                    }

                    if (num_active_surrounding > 3) break;
                }

                // if(num_active_surrounding > 2){
                //     Serial.println("FOUND " + String(num_active_surrounding));
                //     Serial.println(String(i_col + i_col_count * width) + " , " + String(i_row));
                //     Serial.println(((board[i_col_count][i_row] >> i_col) & 1));
                // }


                /**
                 * Decide if alive or CELL_STATE_DEAD
                 */
                if(((board[i_col_count][i_row] >> i_col) & CELL_STATE_ALIVE) == CELL_STATE_ALIVE) {
                    if(num_active_surrounding == 2 || num_active_surrounding == 3){
                        board_next[i_col_count][i_row] = (board_next[i_col_count][i_row] | (CELL_STATE_ALIVE << i_col));
                        any_cells_alive = true;
                    }
                }
                else if(num_active_surrounding == 3) {
                    board_next[i_col_count][i_row] = (board_next[i_col_count][i_row] | (CELL_STATE_ALIVE << i_col));
                    any_cells_alive = true;
                }
            }
        }
    }

    bool board_different = false;

    for (i_col_count = 0; i_col_count < num_columns; i_col_count++) {
        for(i_col = 0; i_col < width; i_col++){
            for(i_row = 0; i_row < height; i_row++){

                if(!board_different)
                    if(((board_next[i_col_count][i_row] >> i_col) & CELL_STATE_ALIVE) != ((board[i_col_count][i_row] >> i_col) & CELL_STATE_ALIVE)){
                        board_different = true;
                        break;
                    }

                if(board_different) break;
            }
            if(board_different) break;
        }
        if(board_different) break;
    }

    /**
     * Copy Board
     */
    for (i_col_count = 0; i_col_count < num_columns; i_col_count++) {
        for(i_row = 0; i_row < height; i_row++) {
            board[i_col_count][i_row] = CELL_STATE_DEAD;
        }
    }

    for (i_col_count = 0; i_col_count < num_columns; i_col_count++) {
        for(i_col = 0; i_col < width; i_col++){
            for(i_row = 0; i_row < height; i_row++){
                board[i_col_count][i_row] = (board[i_col_count][i_row] | (((board_next[i_col_count][i_row] >> i_col) & CELL_STATE_ALIVE) << i_col));
            }
        }
    }

    if(!board_different) {
        Serial.println("Board Stuck in same state");
        _randomize();
        return;
    }

    return;
}

void Conway::_print(bool current = true){

    if(current) Serial.println("Print Current");
    else Serial.println("Print Next");

    Serial.println("Printing Board");
    Serial.println("Width " + String(true_width));
    Serial.println("Height " + String(height));
    Serial.println("Any Active: " + String(any_cells_alive));

    for (i_col_count = 0; i_col_count < num_columns; i_col_count++) {
        if(i_col_count == 0){
            Serial.print("  ");

            if(height >= 10) Serial.print(" ");
        }

        for (i_col = 0; i_col < width; i_col++) {
            Serial.print("  ");
            Serial.print(i_col + i_col_count * width);
        }
    }
    Serial.print("\n");
    for (i_col_count = 0; i_col_count < num_columns; i_col_count++) {
        if(i_col_count == 0){
            Serial.print("  ");

            if(height >= 10) Serial.print(" ");
        }

        for (i_col = 0; i_col < width; i_col++) {
            Serial.print("---");

            if(i_col + i_col_count * width >= 10) Serial.print("-");
        }
    }
    Serial.print("\n");

    for(i_row = 0; i_row < height; i_row++){
        if(height >= 10){
            if(i_row < 10){
                Serial.print(" ");
            }
        }
        Serial.print(i_row);
        Serial.print(" | ");

        for (i_col_count = 0; i_col_count < num_columns; i_col_count++) {
            for (i_col = 0; i_col < width; i_col++) {

                if(i_col + i_col_count * width > 0) Serial.print("  ");

                if(i_col + i_col_count * width >= 10) Serial.print(" ");

                if(current) Serial.print(((board[i_col_count][i_row] >> i_col) & CELL_STATE_ALIVE));
                else Serial.print(((board_next[i_col_count][i_row] >> i_col) & CELL_STATE_ALIVE));
            }
        }
        Serial.print("\n");
    }
}

void Conway::_randomize(){

    for (i_col_count = 0; i_col_count < num_columns; i_col_count++) {
        for(i_row = 0; i_row < height; i_row++){
            board[i_col_count][i_row] = CELL_STATE_DEAD;
        }
    }

    // board[0][0] = 3;
    // board[0][1] = 3;
    // any_cells_alive = true;
    // return;

    /**
     * Get Stuck
     */
    // board[4][4] = CELL_STATE_ALIVE;
    // board[4][5] = CELL_STATE_ALIVE;
    // board[4][6] = CELL_STATE_ALIVE;
    // num_alive = 3;
    // any_cells_alive = true;
    // return;

    /**
     * Glider
     */
    // board[1][0] = CELL_STATE_ALIVE;
    // board[2][1] = CELL_STATE_ALIVE;
    // board[0][2] = CELL_STATE_ALIVE;
    // board[1][2] = CELL_STATE_ALIVE;
    // board[2][2] = CELL_STATE_ALIVE;
    // num_alive = 5;
    // any_cells_alive = true;
    // return;


    /**
     * Small Exploder
     */
    // i_col = i_row = 3;
    // if (width == height) {
    //     i_col = width / 2;
    //     i_row = height / 2;
    // }

    // board[i_col    ][i_row - 1] = CELL_STATE_ALIVE;
    // board[i_col - 1][i_row    ] = CELL_STATE_ALIVE;
    // board[i_col    ][i_row    ] = CELL_STATE_ALIVE;
    // board[i_col + 1][i_row    ] = CELL_STATE_ALIVE;
    // board[i_col - 1][i_row + 1] = CELL_STATE_ALIVE;
    // board[i_col + 1][i_row + 1] = CELL_STATE_ALIVE;
    // board[i_col    ][i_row + 2] = CELL_STATE_ALIVE;
    // // board[i_col    ][i_row - 1] = CELL_STATE_ALIVE;
    // // board[i_col - 1][i_row    ] = CELL_STATE_ALIVE;
    // // board[i_col    ][i_row    ] = CELL_STATE_ALIVE;
    // // board[i_col + 1][i_row    ] = CELL_STATE_ALIVE;
    // // board[i_col - 1][i_row + 1] = CELL_STATE_ALIVE;
    // // board[i_col + 1][i_row + 1] = CELL_STATE_ALIVE;
    // // board[i_col    ][i_row + 2] = CELL_STATE_ALIVE;
    // // num_alive = 7;
    // any_cells_alive = true;
    // return;


    /**
     * Psuedo Random
     */

    randomSeed(analogRead(0));

    long num_random_seeds = random(0, width * height);
    long num_skip = 0;
    int max_width_height = max(width, height);

    for (i_col_count = 0; i_col_count < num_columns; i_col_count++) {
        for(i_col = 0; i_col < width; i_col++){
            for(i_row = 0; i_row < height; i_row++){
                num_skip = random(0, max_width_height);

                if((i_col + i_col_count * width) % num_skip && num_skip % 2){
                    board[i_col_count][i_row] = (board[i_col_count][i_row] | (CELL_STATE_ALIVE << i_col));
                    any_cells_alive = true;
                } else board[i_col_count][i_row] = (board[i_col_count][i_row] | (CELL_STATE_DEAD << i_col));

            }
        }
    }
}

void Conway::draw(){

    // const uint16_t colors[] = {
    //     led_matrix->Color(255, 0, 0),
    //     led_matrix->Color(0, 255, 0),
    //     led_matrix->Color(0, 0, 255),
    //     led_matrix->Color(255, 255, 255)
    // };
    // led_matrix->drawPixel(2, 1, colors[0]);
    // led_matrix->drawPixel(10, 2, colors[1]);
    // led_matrix->drawPixel(1, 15, colors[2]);
    // led_matrix->drawPixel(13, 13, colors[3]);


    if(DEBUG){
        Serial.println("\n\n\n\n\n\n");
        _print();
        _print(false);
    }

    for(i_col_count = 0; i_col_count < num_columns; i_col_count++){
        for(i_col = 0; i_col < width; i_col++){
            for(i_row = 0; i_row < height; i_row++){
                // led_matrix->drawPixel(i_col + i_col_count * width, i_row, led_matrix->Color(0, 255, 0));
                if(((board[i_col_count][i_row] >> i_col) & CELL_STATE_ALIVE) == CELL_STATE_ALIVE) led_matrix->drawPixel(i_col + i_col_count * width, i_row, led_matrix->Color(255, 0, 255));
                else led_matrix->drawPixel(i_col + i_col_count * width, i_row, led_matrix->Color(0, 0, 0));
            }
        }
    }

    led_matrix->show();
}
