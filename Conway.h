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
    #define CONWAY_DEBUG true
#endif /* CONWAY_DEBUG */

/**
 * Brightness for NeoMatrix
 */
#ifndef BRIGHTNESS
    #define BRIGHTNESS (uint8_t)80
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
    #define CLEAR_ON_REDRAW false
#endif CLEAR_ON_REDRAW /* CLEAR_ON_REDRAW */

/**
 * Use density for positions. Requires more space, but will show colors
 * depending on how many cells are active around a given cell. Note will require
 * at least 2 bits for a given cell
 */
#ifndef CONWAY_ASSIGN_DENSITY
    #define CONWAY_ASSIGN_DENSITY false
#endif /* CONWAY_ASSIGN_DENSITY */

/**
 * Draw board spiral style. Assumes board is equal in width and height. Uses
 * a bit more memory than typical col -> row draw
 */
#ifndef CONWAY_DRAW_SPIRAL
    #define CONWAY_DRAW_SPIRAL false
#endif CONWAY_DRAW_SPIRAL /* CONWAY_DRAW_SPIRAL */

/**
 * Draw board one row at a time.
 */
#ifndef CONWAY_WIPE_EFFECT
    #define CONWAY_WIPE_EFFECT false
#endif CONWAY_WIPE_EFFECT /* CONWAY_WIPE_EFFECT */

/**
 * Additional delay between pixel draws. This is in addition to the delay
 * imposed by redrawing by the NeoMatrix library
 */
#ifndef CONWAY_WIPE_EFFECT_DELAY
    #define CONWAY_WIPE_EFFECT_DELAY (uint8_t)0
#endif CONWAY_WIPE_EFFECT_DELAY /* CONWAY_WIPE_EFFECT_DELAY */

/**
 * Redraws board one cell at a time. This will make the effect quite slow.
 */
#ifndef CONWAY_WIPE_EFFECT_PER_CELL
    #define CONWAY_WIPE_EFFECT_PER_CELL false
#endif CONWAY_WIPE_EFFECT_PER_CELL /* CONWAY_WIPE_EFFECT_PER_CELL */

/**
 * Shows a marker where it is drawing
 */
#ifndef CONWAY_WIPE_EFFECT_DRAW_MARKER
    #define CONWAY_WIPE_EFFECT_DRAW_MARKER true
#endif CONWAY_WIPE_EFFECT_DRAW_MARKER /* CONWAY_WIPE_EFFECT_DRAW_MARKER */

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
    #define CELL_STATE_MAX CELL_STATE_ALIVE_HIGH
#endif /* CELL_STATE_MAX */


/*******************************************************************************
 *                                  Includes                                   *
 *******************************************************************************/

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#ifndef PSTR
    #define PSTR /* Make Arduino Due happy */
#endif /* PSTR */

// uint8_t width = 0;
// uint8_t height = 0;
// uint8_t i_col = 0;
// uint8_t i_row = 0;

#include "Board.h"
// #include "BoardNext.h"
#include "BoardAnnex.h"


/*******************************************************************************
 *                                   Conway                                    *
 *******************************************************************************/

class Conway {
private:
    Board * board;
    BoardAnnex * board_next;

    bool row_wise_annex;

    uint8_t * colors;

    uint8_t i_col;
    uint8_t i_row;
    uint8_t width;
    uint8_t height;

    bool any_cells_alive;
    bool board_same;

    Adafruit_NeoMatrix * led_matrix;

    uint8_t num_cells_active_surrounding;
    uint8_t bound_col_min;
    uint8_t bound_col_max;
    uint8_t bound_row_min;
    uint8_t bound_row_max;
    uint8_t i_col_check;
    uint8_t i_row_check;

    #if (CONWAY_DRAW_SPIRAL)
        uint8_t spiral_spins;
        uint8_t i_spiral_spin;
        uint8_t spiral_width;
        uint8_t spiral_height;
    #endif

    void _initColors();

    uint8_t num_skip;
    uint8_t max_width_height;

    void _randomize();
    void _assignNumberCellsActiveSurrounding(uint8_t x, uint8_t y);
    void _assignCurrentDensity();
    void _drawCell(uint8_t x, uint8_t y);

    void _newRound();

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
    // if (width % 8 != 0) {
    //     return;
    // }

    any_cells_alive = false;
    board = new Board(width, height);

    // row_wise_annex = !(width <= height);
    // if (row_wise_annex) board_next = new BoardAnnex(2, height);
    // else

        board_next = new BoardAnnex(width, 2);

    /**
     * Init Colors
     */
    colors = new uint8_t [CELL_STATE_MAX];
    colors[CELL_STATE_DEAD]         = led_matrix->Color(0, 0, 0);
    colors[CELL_STATE_ALIVE]        = led_matrix->Color(255, 255, 255);
    colors[CELL_STATE_ALIVE_LOW]    = led_matrix->Color(200, 50, 0);
    colors[CELL_STATE_ALIVE_HIGH]   = led_matrix->Color(255, 255, 255);
    colors[CELL_STATE_WIPE]         = led_matrix->Color(255, 255, 0);

}

void Conway::_initColors() {

    return;

    delete colors;

    colors = new uint8_t [CELL_STATE_MAX];

    randomSeed(analogRead(0));
    randomSeed(analogRead(random(0, 5)));

    num_skip = random(0, 3);

    colors[CELL_STATE_DEAD]         = led_matrix->Color(0, 0, 0);
    colors[CELL_STATE_ALIVE]        = led_matrix->Color(255, 255, 255);
    colors[CELL_STATE_ALIVE_LOW]    = led_matrix->Color(200, 50, 0);
    colors[CELL_STATE_ALIVE_HIGH]   = led_matrix->Color(255, 200, 0);
    colors[CELL_STATE_WIPE]         = led_matrix->Color(255, 255, 0);

        // return;

    if (num_skip == 0) {
        colors[CELL_STATE_ALIVE_LOW]    = led_matrix->Color(200, 50, 0);
        colors[CELL_STATE_ALIVE_HIGH]   = led_matrix->Color(255, 200, 0);
    } else if (num_skip == 1) {
        colors[CELL_STATE_ALIVE_LOW]    = led_matrix->Color(0, 200, 50);
        colors[CELL_STATE_ALIVE_HIGH]   = led_matrix->Color(0, 255, 0);
    } else if (num_skip == 2) {
        colors[CELL_STATE_ALIVE_LOW]    = led_matrix->Color(50, 0, 200);
        colors[CELL_STATE_ALIVE_HIGH]   = led_matrix->Color(0, 0, 255);
    } else {
        colors[CELL_STATE_ALIVE_LOW]    = led_matrix->Color(200, 50, 0);
        colors[CELL_STATE_ALIVE_HIGH]   = led_matrix->Color(255, 200, 0);
    }
}

void Conway::_randomize() {

    Serial.print("Resetting");

    board->reset();


    // board->setAlive(1, 0);
    // board->setAlive(1, 1);
    // board->setAlive(1, 2);

    // board->setAlive(1, 0);
    // board->setAlive(2, 0);
    // board->setAlive(3, 0);
    // any_cells_alive = true;
    // return;


    /**
     * Short line
    //  */
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
    i_col = i_row = 3;
    if (width == height) {
        i_col = width / 2;
        i_row = height / 2;
    }
    board->setAlive(i_col    , i_row - 1);
    board->setAlive(i_col - 1, i_row    );
    board->setAlive(i_col    , i_row    );
    board->setAlive(i_col + 1, i_row    );
    board->setAlive(i_col - 1, i_row + 1);
    board->setAlive(i_col + 1, i_row + 1);
    board->setAlive(i_col    , i_row + 2);
    any_cells_alive = true;
    return;


    /**
     * Psuedo Random
     */
    randomSeed(analogRead(0));
    randomSeed(analogRead(random(0, 5)));

    num_skip = random(0, 1);
    max_width_height = max(width, height);

    for (i_col = 0; i_col < width; i_col++) {
        randomSeed(analogRead(random(0, 5)));
        for (i_row = 0; i_row < height; i_row++) {
            num_skip = random(0, max_width_height);
            if (i_col % num_skip && num_skip % 2) {
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
    if (x > 0)           bound_col_min -= 1;
    if (x < width - 1)   bound_col_max += 1;
    if (y > 0)           bound_row_min -= 1;
    if (y < height - 1)  bound_row_max += 1;

    /**
     * Check Sorrounding
     */
    for (i_col_check = bound_col_min; i_col_check <= bound_col_max; i_col_check++) {
        for (i_row_check = bound_row_min; i_row_check <= bound_row_max; i_row_check++) {

            /**
             * If not this spot
             */
            if (i_col_check == x && i_row_check == y) continue;

            /**
             * If active
             */
            if (board->getState(i_col_check, i_row_check) >= CELL_STATE_ALIVE) num_cells_active_surrounding += 1;

            if (num_cells_active_surrounding > CELL_STATE_ALIVE_HIGH) break;
        }

        if (num_cells_active_surrounding > CELL_STATE_ALIVE_HIGH) break;
    }
}

void Conway::_assignCurrentDensity() {
    /**
     * Update states to heatmap
     */
    if (!CONWAY_ASSIGN_DENSITY) return;

    /**
     * Update state
     */
    for (i_col = 0; i_col < width; i_col++) {
        for (i_row = 0; i_row < height; i_row++) {
            if (board->getState(i_col, i_row) >= CELL_STATE_ALIVE) {
                _assignNumberCellsActiveSurrounding(i_col, i_row);
                board->setState(i_col, i_row, num_cells_active_surrounding + CELL_STATE_ALIVE_LOW);
            }
        }
    }
}

void Conway::_newRound() {
    _randomize();
    _assignCurrentDensity();
    _initColors();
}

void Conway::_drawCell(uint8_t x, uint8_t y) {
    if (CONWAY_WIPE_EFFECT && CONWAY_WIPE_EFFECT_PER_CELL) {
        if (CONWAY_WIPE_EFFECT_DRAW_MARKER) {
            led_matrix->drawPixel(x, y, colors[CELL_STATE_WIPE]);
            led_matrix->show();

            if (CONWAY_WIPE_EFFECT_DELAY > 0) delay(CONWAY_WIPE_EFFECT_DELAY);
        }

        led_matrix->drawPixel(x, y, colors[board->getState(x, y)]);

        if (!CONWAY_WIPE_EFFECT_DRAW_MARKER) led_matrix->show();
    } else {
        led_matrix->drawPixel(x, y, colors[board->getState(x, y)]);
    }
}

void Conway::init() {
    led_matrix->begin();
    led_matrix->setBrightness(BRIGHTNESS);
    led_matrix->fillScreen(colors[CELL_STATE_DEAD]);
    led_matrix->show();

    _newRound();
}

void Conway::update() {

    /**
     * Check if anything on board
     */
    if (!any_cells_alive) {
        if (CONWAY_DEBUG) Serial.println("NO MORE CELLS CELL_STATE_ALIVE");
        _newRound();
        return;
    }

    /**
     * Reset board next
     */
    board_next->reset();
    // any_cells_alive = false;
    board_same = true;


    /**
     * Compute what is alive in the next round
     */
    for (i_row = 0; i_row < height; i_row++) {
        for (i_col = 0; i_col < width; i_col++) {
            _assignNumberCellsActiveSurrounding(i_col, i_row);

            /**
             * Decide if alive or CELL_STATE_DEAD
             */
            if (board->getState(i_col, i_row) >= CELL_STATE_ALIVE) {
                if (num_cells_active_surrounding == 2 || num_cells_active_surrounding == 3) {
                    if (CONWAY_ASSIGN_DENSITY) board_next->setState(i_col, i_row, num_cells_active_surrounding);
                    else board_next->setAlive(i_col, i_row);

                    // Serial.println("Alive: " + String(i_col) + " , " + String(i_row));

                    any_cells_alive = true;
                }
            }
            else if (num_cells_active_surrounding == 3) {
                if (CONWAY_ASSIGN_DENSITY) board_next->setState(i_col, i_row, num_cells_active_surrounding);
                else board_next->setAlive(i_col, i_row);

                // Serial.println("Alive: " + String(i_col) + " , " + String(i_row));

                any_cells_alive = true;
            }

        }

        /**
         * Assign top row of board_next to board. Then shift board next's rows.
         * Check to see if top row is the same. Clear top, assign bottom to top,
         * then clear bottom.
         */
        if(i_row > 0) board_same &= board_next->copyRow(board);

        board_next->finishRow();
    }

    board_same &= board_next->copyRow(board);

    // Serial.println("Same: " + String(board_same));

    if (CONWAY_CHECK_HISTORY) {
        if (board_same) {
            Serial.println("Board Stuck in same state");
            _newRound();
            return;
        }
    }

    return;
}

void Conway::draw() {
    if (CONWAY_DEBUG) {
        Serial.println("MOVE");
        // Serial.println("\n\n\n\n\n\n");
        // _print();

        board->print(false);
    }

    for (i_col = 0; i_col < width; i_col++) {
        for (i_row = 0; i_row < height; i_row++) {
            led_matrix->drawPixel(i_col, i_row, colors[board->getState(i_col, i_row)]);
        }
    }
    led_matrix->show();
    return;

    if (CONWAY_WIPE_EFFECT && CLEAR_ON_REDRAW) {
        if (CONWAY_DRAW_SPIRAL) {
            for (i_col = 0; i_col < width; i_col++) {
                for (i_row = 0; i_row < height; i_row++) {
                    led_matrix->drawPixel(i_col, i_row, colors[CELL_STATE_DEAD]);
                }
            }
        } else led_matrix->clear();

        led_matrix->show();
    }

    #if (CONWAY_DRAW_SPIRAL)
        spiral_spins = (width / 2) + 1;
        spiral_width = width;
        spiral_height = height;

        i_col = 0;
        i_row = 0;

        for (i_spiral_spin = 0; i_spiral_spin < spiral_spins; i_spiral_spin++) {
            for (i_col = max(width - spiral_width - 1, 0); i_col < spiral_width; i_col++) {
                if (i_spiral_spin != 0) _drawCell(i_col, i_row);

                if (i_col == spiral_width - 1) {
                    for (i_row = height - spiral_height; i_row < spiral_height; i_row++) {
                        _drawCell(i_col, i_row);
                    }
                }
            }

            i_row--;
            i_col--;

            for (; i_col > width - spiral_width; i_col--) {
                _drawCell(i_col, i_row);
            }

            for (; i_row > height - spiral_height; i_row--) {
                _drawCell(i_col, i_row);
            }

            spiral_width--;
            spiral_height--;
        }

        led_matrix->show();
    #else
        for (i_col = 0; i_col < width; i_col++) {
            for (i_row = 0; i_row < height; i_row++) {
                _drawCell(i_col, i_row);
            }

            if (CONWAY_WIPE_EFFECT) {
                if (CONWAY_WIPE_EFFECT_DRAW_MARKER) {
                    for (i_row = 0; i_row < height; i_row++) {
                        led_matrix->drawPixel(i_col, i_row, colors[CELL_STATE_WIPE]);
                    }
                }

                led_matrix->show();

                 if (CONWAY_WIPE_EFFECT_DRAW_MARKER) {
                    for (i_row = 0; i_row < height; i_row++) {
                        led_matrix->drawPixel(i_col, i_row, colors[board->getState(i_col, i_row)]);
                    }
                }
            }
        }

        // _print();

    #endif

    led_matrix->show();
}

#endif /* Conway_h */
