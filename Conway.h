#define DEBUG false
#define BRIGHTNESS 10

#define CELL_STATE_ALIVE true
#define CELL_STATE_DEAD false

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#ifndef PSTR
    #define PSTR /* Make Arduino Due happy */
#endif


class Conway {
    private:
        // bool board [16][16];
        // bool board_next [16][16];
        // bool board_previous [16][16];

        unsigned char ** board;
        unsigned char ** board_next;
        unsigned char ** board_previous;

        unsigned char i_col;
        unsigned char i_row;

        // int num_alive;
        bool any_cells_alive;

        Adafruit_NeoMatrix * led_matrix;

        unsigned char width;
        unsigned char height;

        void _print();
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

    width = led_matrix->width();
    height = led_matrix->height();

    // num_alive = 0;
    any_cells_alive = false;

    // for(i_col=0; i_col < width; i_col++) {
    //     for(i_row=0; i_row < height; i_row++) {
    //         board[i_col][i_row] = CELL_STATE_DEAD;
    //         board_next[i_col][i_row] = CELL_STATE_DEAD;
    //         board_previous[i_col][i_row] = CELL_STATE_DEAD;
    //     }
    // }

    board = new unsigned char * [width];
    board_next = new unsigned char * [width];
    // board_previous = new char * [width];

    for(i_col=0; i_col < width; i_col++) {
        board[i_col] = new unsigned char [height];
        board_next[i_col] = new unsigned char [height];
        // board_previous[i_col] = new char [height];

        for(i_row=0; i_row < height; i_row++) {
            board[i_col][i_row] = CELL_STATE_DEAD;
            board_next[i_col][i_row] = CELL_STATE_DEAD;
            // board_previous[i_col][i_row] = CELL_STATE_DEAD;
        }
    }
}

void Conway::init() {
    led_matrix->begin();
    led_matrix->setBrightness(BRIGHTNESS);
    led_matrix->fillScreen(led_matrix->Color(0, 0, 0));
    led_matrix->show();

    _randomize();
}


void Conway::update(){

    // led_matrix->show();

    // if(num_alive == 0){
    if(!any_cells_alive){
        if(DEBUG) Serial.println("NO MORE CELLS CELL_STATE_ALIVE");
        _randomize();
        return;
    }

    for(i_col=0; i_col < width; i_col++) {
        for(i_row=0; i_row < height; i_row++) {
            board_next[i_col][i_row] = CELL_STATE_DEAD;
        }
    }



    int num_active_surrounding = 0;
    int bound_col_min = 0;
    int bound_col_max = 0;
    int bound_row_min = 0;
    int bound_row_max = 0;
    int i_col_check = 0;
    int i_row_check = 0;

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

                if (num_active_surrounding > 3) break;

                for(i_row_check = bound_row_min; i_row_check <= bound_row_max; i_row_check++){

                    /**
                     * If not this spot
                     */
                    if(i_col_check == i_col && i_row_check == i_row){
                    }
                    else{
                        /**
                         * If active
                         */
                        if(board[i_col_check][i_row_check] == CELL_STATE_ALIVE) num_active_surrounding += 1;

                        if (num_active_surrounding > 3) break;
                    }
                }

                if (num_active_surrounding > 3) break;
            }



            /**
             * Decide if alive or CELL_STATE_DEAD
             */
            if(board[i_col][i_row] == CELL_STATE_ALIVE) {
                if(num_active_surrounding == 2 || num_active_surrounding == 3){
                    board_next[i_col][i_row] = CELL_STATE_ALIVE;
                }
            }
            else if(num_active_surrounding == 3) {
                board_next[i_col][i_row] = CELL_STATE_ALIVE;
            }
        }
    }

    /**
     * Copy Board
     */
    // num_alive = 0;
    any_cells_alive = false;

    bool board_different = false;

    for(i_col = 0; i_col < width; i_col++){

        for(i_row = 0; i_row < height; i_row++){

            if(board_next[i_col][i_row] != board[i_col][i_row]){
                board_different = true;
                // break;
            }

            board[i_col][i_row] = board_next[i_col][i_row];

            // if(board[i_col][i_row] != CELL_STATE_DEAD) num_alive += 1;
            if(board[i_col][i_row] != CELL_STATE_DEAD) any_cells_alive = true;

        }
    }

    if(!board_different) {
        if(DEBUG) Serial.println("Board Stuck in same state");
        _randomize();
        return;
    }

    return;
}

void Conway::_print(){


    Serial.println("Printing Board");

    Serial.println("Width " + String(width));
    Serial.println("Height " + String(height));
    // Serial.println("Number Active " + String(num_alive));
    Serial.println("Any Active: " + String(any_cells_alive));


    Serial.print("    ");

    if(height > 10){
        Serial.print(" ");
    }
    for(i_col = 0; i_col < width; i_col++){
        // Serial.print(i_col) ;
        Serial.print(" ");


        if(i_col < 10){
          Serial.print(" ");
        }
        Serial.print(i_col);
      }
      Serial.print("\n    ");
      for(i_col = 0; i_col < width; i_col++){
        Serial.print("--");

        if(i_col < 10){
          Serial.print("-");
        }
      }
      Serial.print("\n");


      // Serial.println(String(board[1][1]));


      for(i_row = 0; i_row < height; i_row++){

        // delay(1000);
        Serial.print(i_row);
        if(i_row < 10){
          Serial.print(" ");
        }
        Serial.print(" |");
        for(i_col = 0; i_col < width; i_col++){
            Serial.print("  ");

            Serial.print(board[i_col][i_row]);
        }
        Serial.print("\n");
      }
}

void Conway::_randomize(){

    for(i_col = 0; i_col < width; i_col++){
        for(i_row = 0; i_row < height; i_row++){
            board[i_col][i_row] = CELL_STATE_DEAD;
        }
    }

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

    for(i_col = 0; i_col < width; i_col++){
        for(i_row = 0; i_row < height; i_row++){


            num_skip = random(0, max_width_height);

            if(i_col % num_skip > 2){
                board[i_col][i_row] = CELL_STATE_ALIVE;
                // num_alive += 1;
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


    if(DEBUG) _print();

    for(i_col = 0; i_col < width; i_col++){
        for(i_row = 0; i_row < height; i_row++){
            if(board[i_col][i_row] == CELL_STATE_ALIVE) led_matrix->drawPixel(i_col, i_row, led_matrix->Color(255, 0, 255));
            else led_matrix->drawPixel(i_col, i_row, led_matrix->Color(0, 0, 0));
        }
    }

    led_matrix->show();
}
