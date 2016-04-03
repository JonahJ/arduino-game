#ifndef Conway_h
#define Conway_h

#ifndef CONWAY_DEBUG
    #define CONWAY_DEBUG true
#endif /* CONWAY_DEBUG */

#ifndef BRIGHTNESS
    #define BRIGHTNESS 40
#endif /* BRIGHTNESS */

#ifndef CELL_STATE_ALIVE
    #define CELL_STATE_ALIVE 1
#endif /* CELL_STATE_ALIVE */

#ifndef CELL_STATE_DEAD
    #define CELL_STATE_DEAD 0
#endif /* CELL_STATE_DEAD */


#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#ifndef PSTR
    #define PSTR /* Make Arduino Due happy */
#endif

// unsigned char width = 0;
// unsigned char height = 0;
// unsigned char i_col = 0;
// unsigned char i_row = 0;

#include "Board.h"

class Conway {
private:
    Board * board;
    Board * board_next;

    Board ** boards;
    // Board ** boards_density;

    unsigned char i_col;
    unsigned char i_row;

    unsigned char width;
    unsigned char height;

    bool any_cells_alive;

    Adafruit_NeoMatrix * led_matrix;

    unsigned char num_active_surrounding;
    unsigned char bound_col_min;
    unsigned char bound_col_max;
    unsigned char bound_row_min;
    unsigned char bound_row_max;
    unsigned char i_col_check;
    unsigned char i_row_check;

    void _print(bool current);
    void _randomize();

public:
    Conway(
        int num_pixels_width,
        int num_pixels_height,
        int num_boards_x,
        int num_boards_y,
        int pin
    );

    void init();
    void update();
    void draw();
};

Conway::Conway(
    int num_pixels_width,
    int num_pixels_height,
    int num_boards_x,
    int num_boards_y,
    int pin
) {

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

    width = led_matrix->width();
    height = led_matrix->height();


    /**
     * Detect largest width of % 8
     */
    // if(width % 8 != 0){
    //     return;
    // }

    any_cells_alive = false;
    board = new Board(width, height);
    board_next = new Board(width, height);

    int num = 46;

    // boards = new Board * [num];

    // for (int i = 0; i < num; ++i){
    //     boards[i] = new Board(width, height);
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
        if(CONWAY_DEBUG) Serial.println("NO MORE CELLS CELL_STATE_ALIVE");
        _randomize();
        return;
    }


    /**
     * Reset board next
     */
    board_next->reset();

    any_cells_alive = false;

    /**
     * Compute what is alive in the next round
     */
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
                for(i_row_check = bound_row_min; i_row_check <= bound_row_max; i_row_check++){

                    /**
                     * If not this spot
                     */
                    if(i_col_check == i_col && i_row_check == i_row) continue;

                    /**
                     * If active
                     */
                    if(board->getState(i_col_check, i_row_check) == CELL_STATE_ALIVE) num_active_surrounding += 1;

                    if (num_active_surrounding > 3) break;
                }

                if (num_active_surrounding > 3) break;
            }

            /**
             * Decide if alive or CELL_STATE_DEAD
             */
            if(board->getState(i_col, i_row) == CELL_STATE_ALIVE) {
                if(num_active_surrounding == 2 || num_active_surrounding == 3){
                    board_next->setAlive(i_col, i_row);
                    // board_next->setState(i_col, i_row, num_active_surrounding);
                    any_cells_alive = true;
                }
            }
            else if(num_active_surrounding == 3) {
                board_next->setAlive(i_col, i_row);
                // board_next->setState(i_col, i_row, num_active_surrounding);
                any_cells_alive = true;
            }
        }
    }


    bool board_different = false;

    for(i_col = 0; i_col < width; i_col++){
        for(i_row = 0; i_row < height; i_row++){
            if(!board_different)
                if(board->getState(i_col, i_row) != board_next->getState(i_col, i_row)){
                    board_different = true;
                    break;
                }

            if(board_different) break;
        }
        if(board_different) break;
    }

    if(!board_different) {
        Serial.println("Board Stuck in same state");
        _randomize();
        return;
    }


    /**
     * Copy Board
     */
    // TODO remove need for
    board->reset();
    for(i_col = 0; i_col < width; i_col++){
        for(i_row = 0; i_row < height; i_row++){
            // TODO internalize, only do if char not 0
            board->setState(i_col, i_row, board_next->getState(i_col, i_row));
        }
    }

    return;
}

void Conway::_print(bool current = true){

    if(current) Serial.println("Print Current");
    else Serial.println("Print Next");

    Serial.println("Printing Board");
    Serial.println("Width " + String(width));
    Serial.println("Height " + String(height));
    Serial.println("Any Active: " + String(any_cells_alive));

    for (i_col = 0; i_col < width; i_col++) {
        if(i_col == 0){
            Serial.print("  ");

            if(height >= 10) Serial.print(" ");
        }

        Serial.print("  ");
        Serial.print(i_col);
    }
    Serial.print("\n");

    for (i_col = 0; i_col < width; i_col++) {
        if(i_col == 0) {
            Serial.print("  ");

            if(height >= 10) Serial.print(" ");
        }

        Serial.print("---");

        if(i_col >= 10) Serial.print("-");
    }
    Serial.print("\n");

    for(i_row = 0; i_row < height; i_row++){
        if(height >= 10) {
            if(i_row < 10) {
                Serial.print(" ");
            }
        }
        Serial.print(i_row);
        Serial.print(" | ");

        for (i_col = 0; i_col < width; i_col++) {

            if(i_col > 0) Serial.print("  ");

            if(i_col >= 10) Serial.print(" ");

            if(current) Serial.print(board->getState(i_col, i_row));
            else Serial.print(board_next->getState(i_col, i_row));
        }
        Serial.print("\n");
    }
}

void Conway::_randomize(){

    board->reset();

    board->setAlive(7, 2);
    board->setAlive(7, 3);
    board->setAlive(7, 4);
    any_cells_alive = true;
    return;

    board->setAlive(7, 2);
    board->setAlive(7, 3);
    board->setAlive(7, 4);
    any_cells_alive = true;
    return;


    /**
     * Glider
     */
    board->setAlive(1, 0);
    board->setAlive(2, 1);
    board->setAlive(0, 2);
    board->setAlive(1, 2);
    board->setAlive(2, 2);
    any_cells_alive = true;
    return;


    /**
     * Small Exploder
     */
    // i_col = i_row = 3;
    // if (width == height) {
    //     i_col = width / 2;
    //     i_row = height / 2;
    // }
    // board->setAlive(i_col    , i_row - 1);
    // board->setAlive(i_col - 1, i_row    );
    // board->setAlive(i_col    , i_row    );
    // board->setAlive(i_col + 1, i_row    );
    // board->setAlive(i_col - 1, i_row + 1);
    // board->setAlive(i_col + 1, i_row + 1);
    // board->setAlive(i_col    , i_row + 2);
    // any_cells_alive = true;
    // return;


    /**
     * Psuedo Random
     */
    randomSeed(analogRead(0));

    long num_random_seeds = random(0, width * height);
    long num_skip = 0;
    int max_width_height = max(width, height);

    for(i_col = 0; i_col < width; i_col++){
        for(i_row = 0; i_row < height; i_row++){
            num_skip = random(0, max_width_height);
            if(i_col % num_skip && num_skip % 2){
                board->setAlive(i_col, i_row);
                any_cells_alive = true;
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


    if(CONWAY_DEBUG){
        Serial.println("MOVE");
        // Serial.println("\n\n\n\n\n\n");
        // _print();
        _print(false);
    }

    for(i_col = 0; i_col < width; i_col++){
        for(i_row = 0; i_row < height; i_row++){
            // led_matrix->drawPixel(i_col + i_col_count * width, i_row, led_matrix->Color(0, 255, 0));
            // if(board->getState(i_col, i_row) > CELL_STATE_ALIVE) led_matrix->drawPixel(i_col, i_row, led_matrix->Color(255, 0, 0));
            if(board->getState(i_col, i_row) >= CELL_STATE_ALIVE) led_matrix->drawPixel(i_col, i_row, led_matrix->Color(255, 0, 255));
            else led_matrix->drawPixel(i_col, i_row, led_matrix->Color(0, 0, 0));
        }
    }

    led_matrix->show();
}

#endif /* Conway_h */
