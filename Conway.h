#ifndef Conway_h
#define Conway_h

/*******************************************************************************
 *                              Conway Settings                                *
 *******************************************************************************/

/**
 * Debug mode enables certain verbose messaging, including printing out the
 * current state. This may slow down the board speed
 */
#ifndef CONWAY_DEBUG
    #define CONWAY_DEBUG false
#endif /* CONWAY_DEBUG */

/**
 * Brightness for NeoMatrix
 */
#ifndef BRIGHTNESS
    #define BRIGHTNESS (uint8_t)40
#endif /* BRIGHTNESS */

/**
 * Check history so that we can reset if in a "stuck" state, i.e. the same board
 * for more than 1 move
 */
#ifndef CONWAY_CHECK_HISTORY
    #define CONWAY_CHECK_HISTORY true
#endif /* CONWAY_CHECK_HISTORY */

/**
 * Clear board on redraw
 */
#ifndef CLEAR_ON_REDRAW
    #define CLEAR_ON_REDRAW true
#endif CLEAR_ON_REDRAW

/**
 * Use density for positions. Requires more space, but will show colors
 * depending on how many cells are active around a given cell. Note will require
 * at least 2 bits for a given cell
 */
#ifndef CONWAY_ASSIGN_DENSITY
    #define CONWAY_ASSIGN_DENSITY true
#endif /* CONWAY_ASSIGN_DENSITY */

/**
 * Draw board spiral style
 */
#ifndef CONWAY_DRAW_SPIRAL
    #define CONWAY_DRAW_SPIRAL true
#endif CONWAY_DRAW_SPIRAL

/**
 * Draw board one row at a time.
 */
#ifndef CONWAY_WIPE_EFFECT
    #define CONWAY_WIPE_EFFECT true
#endif CONWAY_WIPE_EFFECT


/**
 * Redraws board one cell at a time. This will make the effect quite slow.
 */
#ifndef CONWAY_WIPE_EFFECT_PER_CELL
    #define CONWAY_WIPE_EFFECT_PER_CELL true
#endif CONWAY_WIPE_EFFECT_PER_CELL

/**
 * Shows a marker where it is drawing
 */
#ifndef CONWAY_WIPE_EFFECT_DRAW_MARKER
    #define CONWAY_WIPE_EFFECT_DRAW_MARKER true
#endif CONWAY_WIPE_EFFECT_DRAW_MARKER

/*******************************************************************************
 *                                Cell States                                  *
 *******************************************************************************/

#ifndef CELL_STATE_DEAD
    #define CELL_STATE_DEAD 0
#endif /* CELL_STATE_DEAD */

#ifndef CELL_STATE_ALIVE
    #define CELL_STATE_ALIVE 1
#endif /* CELL_STATE_ALIVE */

#ifndef CELL_STATE_ALIVE_LOW
    #define CELL_STATE_ALIVE_LOW  CELL_STATE_ALIVE + 1
#endif /* CELL_STATE_ALIVE_LOW */

#ifndef CELL_STATE_ALIVE_HIGH
    #define CELL_STATE_ALIVE_HIGH CELL_STATE_ALIVE_LOW + 1
#endif /* CELL_STATE_ALIVE_HIGH */

#ifndef CELL_STATE_WIPE
    #define CELL_STATE_WIPE CELL_STATE_ALIVE_HIGH + 1
#endif /* CELL_STATE_WIPE */

#ifndef CELL_STATE_MAX
    #define CELL_STATE_MAX CELL_STATE_WIPE
#endif /* CELL_STATE_MAX */


/*******************************************************************************
 *                                  Includes                                   *
 *******************************************************************************/

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#ifndef PSTR
    #define PSTR /* Make Arduino Due happy */
#endif

// uint8_t width = 0;
// uint8_t height = 0;
// uint8_t i_col = 0;
// uint8_t i_row = 0;

#include "Board.h"

class Conway {
private:
    Board * board;
    Board * board_next;

    uint16_t * colors;

    uint8_t i_col;
    uint8_t i_row;
    uint8_t width;
    uint8_t height;

    bool any_cells_alive;
    bool board_different;

    Adafruit_NeoMatrix * led_matrix;

    uint8_t num_cells_active_surrounding;
    uint8_t bound_col_min;
    uint8_t bound_col_max;
    uint8_t bound_row_min;
    uint8_t bound_row_max;
    uint8_t i_col_check;
    uint8_t i_row_check;

    void _print(bool current);
    void _randomize();
    void _assignNumberCellsActiveSurrounding(uint8_t x, uint8_t y);
    void _assignCurrentDensity();
    void _drawCell(uint8_t x, uint8_t y);

public:
    Conway(
        uint8_t num_pixels_width,
        uint8_t num_pixels_height,
        uint8_t num_boards_x,
        uint8_t num_boards_y,
        uint8_t pin
    );

    void init();
    void update();
    void draw();
};

Conway::Conway(
    uint8_t num_pixels_width,
    uint8_t num_pixels_height,
    uint8_t num_boards_x,
    uint8_t num_boards_y,
    uint8_t pin
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
    // if(width % 8 != 0) {
    //     return;
    // }

    any_cells_alive = false;
    board = new Board(width, height);
    board_next = new Board(width, height);

    /**
     * Init Colors
     */
    colors = new uint16_t [CELL_STATE_MAX];
    colors[CELL_STATE_DEAD]         = led_matrix->Color(0, 0, 0);
    colors[CELL_STATE_ALIVE]        = led_matrix->Color(255, 255, 255);
    colors[CELL_STATE_ALIVE_LOW]    = led_matrix->Color(200, 50, 0);
    colors[CELL_STATE_ALIVE_HIGH]   = led_matrix->Color(255, 0, 0);
    colors[CELL_STATE_WIPE]         = led_matrix->Color(255, 255, 0);
}

void Conway::_print(bool current = true) {

    if(current) Serial.println("Printing Current");
    else Serial.println("Printing Next");

    Serial.println("Printing Board");
    Serial.println("Width " + String(width));
    Serial.println("Height " + String(height));
    Serial.println("Any Active: " + String(any_cells_alive));

    for (i_col = 0; i_col < width; i_col++) {
        if(i_col == 0) {
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

    for(i_row = 0; i_row < height; i_row++) {
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

void Conway::_randomize() {
    board->reset();

    /**
     * Short line
     */
    // board->setAlive(7, 2);
    // board->setAlive(7, 3);
    // board->setAlive(7, 4);
    // any_cells_alive = true;
    // return;

    /**
     * Glider
     */
    // board->setAlive(1, 0);
    // board->setAlive(2, 1);
    // board->setAlive(0, 2);
    // board->setAlive(1, 2);
    // board->setAlive(2, 2);
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
    randomSeed(analogRead(random(0, 5)));

    long num_random_seeds = random(0, width * height);
    long num_skip = random(0, 1);
    uint8_t max_width_height = max(width, height);

    for(i_col = 0; i_col < width; i_col++) {
        randomSeed(analogRead(random(0, 5)));
        for(i_row = 0; i_row < height; i_row++) {
            num_skip = random(0, max_width_height);
            if(i_col % num_skip && num_skip % 2) {
                board->setAlive(i_col, i_row);
                any_cells_alive = true;
            }
        }
    }
}

void Conway::_assignNumberCellsActiveSurrounding(uint8_t x, uint8_t y) {
    num_cells_active_surrounding = 0;
    bound_col_min = x;
    bound_col_max = x;
    bound_row_min = y;
    bound_row_max = y;

    /**
     * Check left, right, up, down
     */
    if(x > 0)           bound_col_min -= 1;
    if(x < width - 1)   bound_col_max += 1;
    if(y > 0)           bound_row_min -= 1;
    if(y < height - 1)  bound_row_max += 1;

    /**
     * Check Sorrounding
     */
    for(i_col_check = bound_col_min; i_col_check <= bound_col_max; i_col_check++) {
        for(i_row_check = bound_row_min; i_row_check <= bound_row_max; i_row_check++) {

            /**
             * If not this spot
             */
            if(i_col_check == x && i_row_check == y) continue;

            /**
             * If active
             */
            if(board->getState(i_col_check, i_row_check) >= CELL_STATE_ALIVE) num_cells_active_surrounding += 1;

            if(num_cells_active_surrounding > CELL_STATE_ALIVE_HIGH) break;
        }

        if(num_cells_active_surrounding > CELL_STATE_ALIVE_HIGH) break;
    }
}

void Conway::_assignCurrentDensity() {
    /**
     * Update states to heatmap
     */
    if(!CONWAY_ASSIGN_DENSITY) return;

    for(i_col = 0; i_col < width; i_col++) {
        for(i_row = 0; i_row < height; i_row++) {
            /**
             * Update state
             */
            if(board->getState(i_col, i_row) >= CELL_STATE_ALIVE) {
                _assignNumberCellsActiveSurrounding(i_col, i_row);
                board->setState(i_col, i_row, num_cells_active_surrounding + CELL_STATE_ALIVE_LOW);
            }
        }
    }
}

void Conway::_drawCell(uint8_t x, uint8_t y) {
    if(CONWAY_WIPE_EFFECT && CONWAY_WIPE_EFFECT_PER_CELL) {
        if(CONWAY_WIPE_EFFECT_DRAW_MARKER) {
            led_matrix->drawPixel(x, y, colors[CELL_STATE_WIPE]);
            led_matrix->show();
        }
        led_matrix->drawPixel(x, y, colors[board->getState(x, y)]);
        if(!CONWAY_WIPE_EFFECT_DRAW_MARKER) led_matrix->show();
    } else {
        led_matrix->drawPixel(x, y, colors[board->getState(x, y)]);
    }
}

void Conway::init() {
    led_matrix->begin();
    led_matrix->setBrightness(BRIGHTNESS);
    led_matrix->fillScreen(led_matrix->Color(0, 0, 0));
    led_matrix->show();

    _randomize();
    _assignCurrentDensity();
}

void Conway::update() {

    /**
     * Check if anything on board
     */
    if(!any_cells_alive) {
        if(CONWAY_DEBUG) Serial.println("NO MORE CELLS CELL_STATE_ALIVE");
        _randomize();
        _assignCurrentDensity();
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
    for(i_col = 0; i_col < width; i_col++) {
        for(i_row = 0; i_row < height; i_row++) {
            _assignNumberCellsActiveSurrounding(i_col, i_row);

            /**
             * Decide if alive or CELL_STATE_DEAD
             */
            if(board->getState(i_col, i_row) >= CELL_STATE_ALIVE) {
                if(num_cells_active_surrounding == 2 || num_cells_active_surrounding == 3) {
                    if(CONWAY_ASSIGN_DENSITY) board_next->setState(i_col, i_row, num_cells_active_surrounding);
                    else board_next->setAlive(i_col, i_row);

                    any_cells_alive = true;
                }
            }
            else if(num_cells_active_surrounding == 3) {
                if(CONWAY_ASSIGN_DENSITY) board_next->setState(i_col, i_row, num_cells_active_surrounding);
                else board_next->setAlive(i_col, i_row);

                any_cells_alive = true;
            }
        }
    }

    if (CONWAY_CHECK_HISTORY) {
        board_different = false;

        for(i_col = 0; i_col < width; i_col++) {
            for(i_row = 0; i_row < height; i_row++) {
                if(board->getState(i_col, i_row) != board_next->getState(i_col, i_row)) {
                    board_different = true;
                    break;
                }
            }
            if(board_different) break;
        }

        if(!board_different) {
            Serial.println("Board Stuck in same state");
            _randomize();
            _assignCurrentDensity();
            return;
        }
    }

    /**
     * Copy Board
     */
    board->copyBoard(board_next);

    return;
}

void Conway::draw() {
    if(CONWAY_DEBUG) {
        Serial.println("MOVE");
        // Serial.println("\n\n\n\n\n\n");
        // _print();
        _print(false);
    }

    if(CONWAY_WIPE_EFFECT && CLEAR_ON_REDRAW) {
        led_matrix->clear();

        // if(CONWAY_DRAW_SPIRAL) {
        //     for(i_col = 0; i_col < width; i_col++) {
        //         for(i_row = 0; i_row < height; i_row++) {
        //             // led_matrix->drawPixel(i_col, i_row, colors[CELL_STATE_DEAD]);
        //         }
        //     }
        // }
        led_matrix->show();
    }

    if(CONWAY_DRAW_SPIRAL) {

        uint8_t spiral_spins = width / 2;

        uint8_t spiral_width = width;
        uint8_t spiral_height = width;

        for(uint8_t i_spiral_spin = 0; i_spiral_spin < spiral_spins; i_spiral_spin++) {

            for(i_col = width - spiral_width; i_col < spiral_width; i_col++) {
                _drawCell(i_col, i_row);

                if(i_col == spiral_width - 1) {
                    for(i_row = height - spiral_height; i_row < spiral_height; i_row++) {
                        _drawCell(i_col, i_row);
                    }
                }
            }

            i_row--;

            for(; i_col > width - spiral_width; i_col--) {
                _drawCell(i_col, i_row);
            }


            for(; i_row > height - spiral_height; i_row--){
                _drawCell(i_col, i_row);
            }

            spiral_height--;
            spiral_width--;
        }

        led_matrix->show();




    } else {
        for(i_col = 0; i_col < width; i_col++) {
            for(i_row = 0; i_row < height; i_row++) {
                _drawCell(i_col, i_row);
            }

            if(CONWAY_WIPE_EFFECT) {
                if(CONWAY_WIPE_EFFECT_DRAW_MARKER) {
                    for(i_row = 0; i_row < height; i_row++) {
                        led_matrix->drawPixel(i_col, i_row, colors[CELL_STATE_WIPE]);
                    }
                }

                led_matrix->show();

                 if(CONWAY_WIPE_EFFECT_DRAW_MARKER) {
                    for(i_row = 0; i_row < height; i_row++) {
                        led_matrix->drawPixel(i_col, i_row, colors[board->getState(i_col, i_row)]);
                    }
                }
            }
        }
    }

    led_matrix->show();
}

#endif /* Conway_h */
