#include "Conway.h"

/**
 * Constructor
 *
 * @param  {uint8_t} num_pixels_width   Width of individual NeoMatrix
 * @param  {uint8_t} num_pixels_height  Height of individual NeoMatrix
 * @param  {uint8_t} num_boards_x       Number of boards in X direction
 * @param  {uint8_t} num_boards_y       Number of boards in Y direction
 * @param  {uint8_t} pin                Pin out on Arduino
 */
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

    #if (CONWAY_COUNT_MOVES > 0)
        number_of_rounds_running = 0;
    #endif /* CONWAY_COUNT_MOVES */

    #if (CONWAY_CYCLE_DETECTED_BUFFER > 0)
        number_of_moves_since_cycle_detected = 0;
    #endif /* CONWAY_CYCLE_DETECTED_BUFFER */

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

    #if (CONWAY_CHECKING_BOARD_MINIMIZE)
        #if (CONWAY_CHECKING_BOARD_USE_STACK)
            board_next = new BoardAnnexStack(width, 2);
        #else
            board_next = new BoardAnnex(width, 2);
        #endif /* CONWAY_CHECKING_BOARD_USE_STACK */
    #else
        board_next = new Board(width, height);
    #endif /* CONWAY_CHECKING_BOARD_MINIMIZE */

    #if (CONWAY_CHECK_IF_IN_CYCLE)
        board_two_ago =  new Board(width, height);
    #endif /* CONWAY_CHECK_IF_IN_CYCLE */

    // delete colors;

    colors = new uint16_t [CELL_STATE_MAX - 1];
    colors[CELL_STATE_DEAD] = led_matrix->Color(0, 0, 0);
    colors[CELL_STATE_WIPE] = led_matrix->Color(255, 255, 255);

}

/**
 * Inits colors for grid. If CONWAY_ASSIGN_MONOCHROME is false then randomly
 * choose a color scheme.
 */
void Conway::_initColors() {

    #if (CONWAY_ASSIGN_MONOCHROME)
        colors[CELL_STATE_ALIVE]                = led_matrix->Color(255, 255, 255);

        #if (CONWAY_ASSIGN_DENSITY)
            colors[CELL_STATE_ALIVE_LOW]        = led_matrix->Color(150, 150, 150);
            colors[CELL_STATE_ALIVE_HIGH]       = led_matrix->Color(255, 255, 255);
        #endif /* CONWAY_ASSIGN_DENSITY */
    #else
        /**
         * Compute a random color
         */
        // TODO minimize, but get more random
        randomSeed(analogRead(0));
        randomSeed(analogRead(random(0, 5)));
        randomSeed(analogRead(num_skip)); // maybe this should be moved up one line?

        num_skip = random(0, 3);

        if (num_skip == 0) {
            colors[CELL_STATE_ALIVE]            = led_matrix->Color(255, 200, 0);

            #if (CONWAY_ASSIGN_DENSITY)
                colors[CELL_STATE_ALIVE_LOW]    = led_matrix->Color(200, 50, 0);
                colors[CELL_STATE_ALIVE_HIGH]   = led_matrix->Color(255, 200, 0);
            #endif /* CONWAY_ASSIGN_DENSITY */
        } else if (num_skip == 1) {
            colors[CELL_STATE_ALIVE]            = led_matrix->Color(0, 255, 0);

            #if (CONWAY_ASSIGN_DENSITY)
                colors[CELL_STATE_ALIVE_LOW]    = led_matrix->Color(0, 200, 50);
                colors[CELL_STATE_ALIVE_HIGH]   = led_matrix->Color(0, 255, 0);
            #endif /* CONWAY_ASSIGN_DENSITY */
        } else if (num_skip == 2) {
            colors[CELL_STATE_ALIVE]            = led_matrix->Color(0, 0, 255);

            #if (CONWAY_ASSIGN_DENSITY)
                colors[CELL_STATE_ALIVE_LOW]    = led_matrix->Color(50, 0, 200);
                colors[CELL_STATE_ALIVE_HIGH]   = led_matrix->Color(0, 0, 255);
            #endif /* CONWAY_ASSIGN_DENSITY */
        } else {

            #if (CONWAY_DEBUG)
                Serial.println("ERROR HOW DID COLORS GET HERE");
                Serial.println("num_skip: " + String(num_skip));
            #endif /* CONWAY_DEBUG */
        }
    #endif /* CONWAY_ASSIGN_MONOCHROME */

    // colors[CELL_STATE_ALIVE_LOW]    = led_matrix->Color(255, 255, 255);
    // colors[CELL_STATE_ALIVE_HIGH]   = led_matrix->Color(255, 255, 255);
    // colors[CELL_STATE_WIPE]         = led_matrix->Color(255, 255, 255);
    // colors[CELL_STATE_ALIVE_LOW]    = led_matrix->Color(200, 50, 0);
    // colors[CELL_STATE_ALIVE_HIGH]   = led_matrix->Color(255, 200, 0);
    // colors[CELL_STATE_WIPE]         = led_matrix->Color(255, 255, 0);

    // return;
}

/**
 * Randomize grid
 */
void Conway::_randomize() {

    #if (CONWAY_DEBUG)
        Serial.println("Resetting");
    #endif /* CONWAY_DEBUG */

    board->reset();


    // board->setAlive(1, 0);
    // board->setAlive(1, 1);
    // board->setAlive(1, 2);

    // board->setAlive(1, 2);
    // board->setAlive(2, 2);
    // board->setAlive(3, 2);
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

/**
 * Assign `num_cells_active_surrounding` for `x` and `y`
 *
 * @param  {uint8_t} x  x coordiante
 * @param  {uint8_t} y  y coordiante
 */
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

/**
 * Assigns and sets the density for a grid. Intended to be used only after
 * `_randomize()` is called. Only is enabled if CONWAY_ASSIGN_DENSITY is true
 */
#if (CONWAY_ASSIGN_DENSITY)
    void Conway::_assignCurrentDensity() {
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
#endif /* CONWAY_ASSIGN_DENSITY */

/**
 * Begin a new round of conway. Reset grid and init colors.
 */
void Conway::_newRound() {
    #if (CONWAY_COUNT_MOVES)
        number_of_rounds_running = 0;
    #endif /* CONWAY_COUNT_MOVES */

    #if (CONWAY_CYCLE_DETECTED_BUFFER > 0)
        number_of_moves_since_cycle_detected = 0;
    #endif /* CONWAY_CYCLE_DETECTED_BUFFER */

    _randomize();

    #if (CONWAY_ASSIGN_DENSITY)
        _assignCurrentDensity();
    #endif /* CONWAY_ASSIGN_DENSITY */

    _initColors();
}

/**
 * Draw an individual cell in the grid
 *
 * @param  {uint8_t} x  x coordiante
 * @param  {uint8_t} y  y coordiante
 */
void Conway::_drawCell(uint8_t x, uint8_t y) {
    #if (CONWAY_WIPE_EFFECT && CONWAY_WIPE_EFFECT_PER_CELL)
        #if (CONWAY_WIPE_EFFECT_DRAW_MARKER)
            led_matrix->drawPixel(x, y, colors[CELL_STATE_WIPE]);
            led_matrix->show();

            #if (CONWAY_WIPE_EFFECT_DELAY > 0)
                delay(CONWAY_WIPE_EFFECT_DELAY);
            #endif /* CONWAY_WIPE_EFFECT_DELAY > 0 */
        #endif /* CONWAY_WIPE_EFFECT_DRAW_MARKER */

        led_matrix->drawPixel(x, y, colors[board->getState(x, y)]);

        #if (!CONWAY_WIPE_EFFECT_DRAW_MARKER)
            led_matrix->show();
        #endif /* !CONWAY_WIPE_EFFECT_DRAW_MARKER */
    #else
        led_matrix->drawPixel(x, y, colors[board->getState(x, y)]);
    #endif /* CONWAY_WIPE_EFFECT && CONWAY_WIPE_EFFECT_PER_CELL */
}

/**
 * Init for `setup()` function
 */
void Conway::init() {
    led_matrix->setBrightness(BRIGHTNESS);
    led_matrix->begin();
    led_matrix->fillScreen(colors[CELL_STATE_DEAD]);
    led_matrix->show();

    _newRound();
}

/**
 * Update grid and do necessary checks. If no cells alive, create a new round.
 * Optional checking is enabled with CONWAY_CHECK_HISTORY
 */
void Conway::update() {

    /**
     * Check if anything on board
     */
    if (!any_cells_alive) {
        #if (CONWAY_DEBUG)
            Serial.println("All cells are dead. Initing new life");
        #endif /* CONWAY_DEBUG */

        _newRound();
        return;
    }

    #if (CONWAY_CHECK_IF_IN_CYCLE)
        if (number_of_rounds_running % 2) {
            board_two_ago->reset();

            board_two_ago->copyBoard(board);
        }
    #endif /* CONWAY_CHECK_IF_IN_CYCLE */

    /**
     * Reset board next
     */
    board_next->reset();
    any_cells_alive = false;

    #if (CONWAY_CHECK_HISTORY)
        board_same = true;
    #endif /* CONWAY_CHECK_HISTORY */

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

                    #if (CONWAY_ASSIGN_DENSITY)
                        board_next->setState(i_col, i_row, num_cells_active_surrounding);
                    #else
                        board_next->setAlive(i_col, i_row);
                    #endif /* CONWAY_ASSIGN_DENSITY */

                    // Serial.println("Alive: " + String(i_col) + " , " + String(i_row));

                    any_cells_alive = true;
                }
            }
            else if (num_cells_active_surrounding == 3) {
                #if (CONWAY_ASSIGN_DENSITY)
                    board_next->setState(i_col, i_row, num_cells_active_surrounding);
                #else
                    board_next->setAlive(i_col, i_row);
                #endif /* CONWAY_ASSIGN_DENSITY */

                // Serial.println("Alive: " + String(i_col) + " , " + String(i_row));

                any_cells_alive = true;
            }

            #if (CONWAY_CHECK_HISTORY)
                #if (!CONWAY_CHECKING_BOARD_MINIMIZE)
                    if(!board_same) continue;

                    if (board->getState(i_col, i_row) != board_next->getState(i_col, i_row)) board_same = false;
                #endif /* !CONWAY_CHECKING_BOARD_MINIMIZE */
            #endif /* CONWAY_CHECK_HISTORY */
        }

        /**
         * Assign top row of board_next to board. Then shift board next's rows.
         * Check to see if top row is the same. Clear top, assign bottom to top,
         * then clear bottom.
         */
        #if (CONWAY_CHECKING_BOARD_MINIMIZE)
            #if (CONWAY_CHECK_HISTORY)
                if(i_row > 0) board_same &= board_next->copyRow(board);
            #else
                if(i_row > 0) board_next->copyRow(board);
            #endif /* CONWAY_CHECK_HISTORY */

            #if (!CONWAY_CHECKING_BOARD_USE_STACK)
                board_next->finishRow();
            #endif /* CONWAY_CHECKING_BOARD_USE_STACK */
        #endif /* CONWAY_CHECKING_BOARD_MINIMIZE */
    }

    #if (CONWAY_CHECKING_BOARD_MINIMIZE)
        #if (CONWAY_CHECK_HISTORY)
            board_same &= board_next->copyRow(board);

            #if (CONWAY_CHECKING_BOARD_USE_STACK)
                board_same &= board_next->copyRow(board);
            #endif /* CONWAY_CHECKING_BOARD_USE_STACK */
        #else
            board_next->copyRow(board);

            #if (CONWAY_CHECKING_BOARD_USE_STACK)
                board_next->copyRow(board);
            #endif /* CONWAY_CHECKING_BOARD_USE_STACK */
        #endif /* CONWAY_CHECK_HISTORY */
    #endif /* CONWAY_CHECKING_BOARD_MINIMIZE */

    #if (CONWAY_CHECK_HISTORY)
        if (board_same) {
            #if (CONWAY_DEBUG)
                Serial.println("Board Stuck in same state");
            #endif /* CONWAY_DEBUG */

            _newRound();
            return;
        }
    #endif /* CONWAY_CHECK_HISTORY*/

    #if (!CONWAY_CHECKING_BOARD_MINIMIZE)
        board->copyBoard(board_next);
    #endif /* !CONWAY_CHECKING_BOARD_MINIMIZE */


    #if (CONWAY_CHECK_IF_IN_CYCLE)
        if (!(number_of_rounds_running % 2)) {

            #if (CONWAY_DEBUG)
                Serial.println("Print board two moves ago");

                board_two_ago->print();
                board->print();
            #endif /* CONWAY_DEBUG */

            #if (CONWAY_CHECK_HISTORY)
                board_same = true;
            #endif /* CONWAY_CHECK_HISTORY */

            for (i_row = 0; i_row < height; i_row++) {
                if (!board_same) break;

                for (i_col = 0; i_col < width; i_col++) {
                    if (board_two_ago->getState(i_col, i_row) != board->getState(i_col, i_row)){
                        board_same = false;
                        break;
                    }
                }

                if (!board_same) break;
            }

            if (board_same) {

                #if (CONWAY_DEBUG)
                    Serial.println("Board Stuck in cycle");
                #endif /* CONWAY_DEBUG */

                #if (CONWAY_CYCLE_DETECTED_BUFFER > 0)
                    number_of_moves_since_cycle_detected++;

                    if (number_of_moves_since_cycle_detected >= CONWAY_CYCLE_DETECTED_BUFFER) _newRound();

                    return;
                #endif /* CONWAY_CYCLE_DETECTED_BUFFER */

                _newRound();
                return;
            }
        }
    #endif /* CONWAY_CHECK_IF_IN_CYCLE */

    return;
}

/**
 * Draw board.
 */
void Conway::draw() {

    #if (CONWAY_DEBUG)
        Serial.println("MOVE");
        // Serial.println("\n\n\n\n\n\n");

        board->print();
    #endif /* CONWAY_DEBUG */

    #if (CONWAY_COUNT_MOVES)
        number_of_rounds_running++;

        #if (CONWAY_MAX_MOVES > 0)
            if(number_of_rounds_running > CONWAY_MAX_MOVES) {
                #if (CONWAY_DEBUG)
                    Serial.println("Maximum Number of Moves Ran, resetting board");
                #endif /* CONWAY_DEBUG */

                _newRound();
            }
        #endif /* CONWAY_MAX_MOVES */
    #endif /* CONWAY_COUNT_MOVES */

    #if (CONWAY_DRAW_MODE == CONWAY_DRAW_MODE_SIMPLE)
        for (i_col = 0; i_col < width; i_col++) {
            for (i_row = 0; i_row < height; i_row++) {
                led_matrix->drawPixel(i_col, i_row, colors[board->getState(i_col, i_row)]);
            }
        }

        led_matrix->show();

        return;
    #endif /* CONWAY_DRAW_MODE == CONWAY_DRAW_MODE_SIMPLE */

    #if (CONWAY_WIPE_EFFECT && CLEAR_ON_REDRAW)
        #if (CONWAY_DRAW_MODE_SPIRAL)
            for (i_col = 0; i_col < width; i_col++) {
                for (i_row = 0; i_row < height; i_row++) {
                    led_matrix->drawPixel(i_col, i_row, colors[CELL_STATE_DEAD]);
                }
            }
        #else
            led_matrix->clear();
        #endif /* CONWAY_DRAW_MODE_SPIRAL */

        led_matrix->show();
    #endif /* CONWAY_WIPE_EFFECT && CLEAR_ON_REDRAW */

    #if (CONWAY_DRAW_MODE == CONWAY_DRAW_MODE_ROW_WISE)
        for (i_col = 0; i_col < width; i_col++) {
            for (i_row = 0; i_row < height; i_row++) {
                _drawCell(i_col, i_row);
            }

            #if (CONWAY_WIPE_EFFECT && (!CONWAY_WIPE_EFFECT_PER_CELL))
                #if (CONWAY_WIPE_EFFECT_DRAW_MARKER)
                    for (i_row = 0; i_row < height; i_row++) {
                        led_matrix->drawPixel(i_col, i_row, colors[CELL_STATE_WIPE]);
                    }
                #endif /* CONWAY_WIPE_EFFECT_DRAW_MARKER */

                led_matrix->show();

                #if (CONWAY_WIPE_EFFECT_DRAW_MARKER)
                    for (i_row = 0; i_row < height; i_row++) {
                        led_matrix->drawPixel(i_col, i_row, colors[board->getState(i_col, i_row)]);
                    }
                #endif /* CONWAY_WIPE_EFFECT_DRAW_MARKER */

                #if (CONWAY_WIPE_EFFECT_DELAY > 0)
                    delay(CONWAY_WIPE_EFFECT_DELAY);
                #endif /* CONWAY_WIPE_EFFECT_DELAY > 0 */
            #endif /* CONWAY_WIPE_EFFECT */
        }
    #elif (CONWAY_DRAW_MODE == CONWAY_DRAW_MODE_SPIRAL)
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
    #endif /* (CONWAY_DRAW_MODE == CONWAY_DRAW_MODE_ROW_WISE) */

    led_matrix->show();
}
