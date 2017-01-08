#ifndef Conway_h
#define Conway_h

/*******************************************************************************
 *                                  Includes                                   *
 *******************************************************************************/

#include "Game.h"

/*******************************************************************************
 *                              Conway Settings                                *
 *******************************************************************************/

/**
 * Set to above 0 to have a max number moves before automatic reset. Quick and
 * dirty way to not get stuck in a repeated state without having 2 full boards
 * or a hash map.
 */
#ifndef CONWAY_MAX_MOVES
    #define CONWAY_MAX_MOVES -1
#endif /* CONWAY_MAX_MOVES */

/**
 * Check history so that we can reset if in a "stuck" state, i.e. the same board
 * for more than 1 move
 */
#ifndef CONWAY_CHECK_HISTORY
    #define CONWAY_CHECK_HISTORY true
#endif /* CONWAY_CHECK_HISTORY */

/**
 * Check history of round - 2 ago. This way if in cycle we can terminate.
 * Requires much more memory.
 */
#ifndef CONWAY_CHECK_IF_IN_CYCLE
    #define CONWAY_CHECK_IF_IN_CYCLE true

    #undef GAME_COUNT_MOVES
    #ifndef GAME_COUNT_MOVES
        #define GAME_COUNT_MOVES true
    #endif /* GAME_COUNT_MOVES */
#endif /* CONWAY_CHECK_IF_IN_CYCLE */

/**
 * If cycle is found, run this number of moves before making new round
 */
#ifndef CxONWAY_CYCLE_DETECTED_BUFFER
    #define CONWAY_CYCLE_DETECTED_BUFFER 3
#endif /* CONWAY_CYCLE_DETECTED_BUFFER */


/**
 * Hold full 2D board to check or minimize to only 2 rows. This has large
 * effects on memory capacity.
 *
 *                       ------------------------------
 *                       | Setting |  Memory Capacity |
 *                       ------------------------------
 *                       |   false |       O(2wh)     |
 *                       |   true  |       O(wh + 2w) |
 *                       ------------------------------
 */
#ifndef CONWAY_CHECKING_BOARD_MINIMIZE
    #define CONWAY_CHECKING_BOARD_MINIMIZE true
#endif /* CONWAY_CHECKING_BOARD_MINIMIZE */

/**
 * Use stack to hold minimimized checking board. This is easier logic, but
 * requires more context switching and copying over rows, i.e. it is slower
 */
#ifndef CONWAY_CHECKING_BOARD_USE_STACK
    #define CONWAY_CHECKING_BOARD_USE_STACK false
#endif /* CONWAY_CHECKING_BOARD_USE_STACK */

/**
 * Assign monochrome or color
 */
#ifndef CONWAY_ASSIGN_MONOCHROME
 #define CONWAY_ASSIGN_MONOCHROME false
#endif /* CONWAY_ASSIGN_MONOCHROME */

/**
 * Use density for positions. Requires more space, but will show colors
 * depending on how many cells are active around a given cell. Note will require
 * at least 2 bits for a given cell
 */
#ifndef CONWAY_ASSIGN_DENSITY
    #define CONWAY_ASSIGN_DENSITY true
#endif /* CONWAY_ASSIGN_DENSITY */

/*******************************************************************************
 *                               Local Includes                                *
 *******************************************************************************/

#if (CONWAY_CHECKING_BOARD_MINIMIZE)
    #if (CONWAY_CHECKING_BOARD_USE_STACK)
        #include "Boards/BoardAnnexStack.h"
    #else
        #include "Boards/BoardAnnex.h"
    #endif /* CONWAY_CHECKING_BOARD_USE_STACK */
#endif /* CONWAY_CHECKING_BOARD_MINIMIZE */


/*******************************************************************************
 *                                   Conway                                    *
 *******************************************************************************/

class Conway: public Game {
private:

    #if (CONWAY_CHECKING_BOARD_MINIMIZE)
        #if (CONWAY_CHECKING_BOARD_USE_STACK)
            BoardAnnexStack * board_next;
        #else
            BoardAnnex * board_next;
        #endif /* CONWAY_CHECKING_BOARD_USE_STACK */
    #else
        Board * board_next;
    #endif /* CONWAY_CHECKING_BOARD_MINIMIZE */

    #if (CONWAY_CHECK_IF_IN_CYCLE)
        Board * board_two_ago;
    #endif /* CONWAY_CHECK_IF_IN_CYCLE */

    #if (CONWAY_CYCLE_DETECTED_BUFFER > 0)
        uint8_t number_of_moves_since_cycle_detected;
    #endif /* CONWAY_CYCLE_DETECTED_BUFFER */

    bool any_cells_alive;

    #if (CONWAY_CHECK_HISTORY)
        bool board_same;
    #endif /* CONWAY_CHECK_HISTORY */

    uint8_t num_cells_active_surrounding;

    uint8_t bound_col_min;
    uint8_t bound_col_max;
    uint8_t bound_row_min;
    uint8_t bound_row_max;

    uint8_t i_col_check;
    uint8_t i_row_check;

    void _initColors();

    uint8_t num_skip;

    void _assignPattern();
    void _assignNumberCellsActiveSurrounding(uint8_t x, uint8_t y);

    #if (CONWAY_ASSIGN_DENSITY)
        void _assignCurrentDensity();
    #endif /* CONWAY_ASSIGN_DENSITY */

protected:
    virtual void _newRound();

public:
    Conway(
        uint8_t num_pixels_width,
        uint8_t num_pixels_height,
        uint8_t num_boards_x,
        uint8_t num_boards_y,
        uint8_t pin
    );

    virtual void update();
};


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
): Game(
    num_pixels_width,
    num_pixels_height,
    num_boards_x,
    num_boards_y,
    pin
) {
    #if (CONWAY_CYCLE_DETECTED_BUFFER > 0)
        number_of_moves_since_cycle_detected = 0;
    #endif /* CONWAY_CYCLE_DETECTED_BUFFER */

    any_cells_alive = false;

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

            #if (GAME_DEBUG)
                Serial.println("ERROR HOW DID COLORS GET HERE");
                Serial.println("num_skip: " + String(num_skip));
            #endif /* GAME_DEBUG */
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
 * Assign pattern to grid
 */
void Conway::_assignPattern() {

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
    Game::_newRound();

    #if (CONWAY_CYCLE_DETECTED_BUFFER > 0)
        number_of_moves_since_cycle_detected = 0;
    #endif  /* CONWAY_CYCLE_DETECTED_BUFFER */

    // _assignPattern();
    board->randomize();
    any_cells_alive = true;

    #if (CONWAY_ASSIGN_DENSITY)
        _assignCurrentDensity();
    #endif /* CONWAY_ASSIGN_DENSITY */

    _initColors();
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
        #if (GAME_DEBUG)
            Serial.println("All cells are dead. Initing new life");
        #endif /* GAME_DEBUG */

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
            #if (GAME_DEBUG)
                Serial.println("Board Stuck in same state");
            #endif /* GAME_DEBUG */

            _newRound();
            return;
        }
    #endif /* CONWAY_CHECK_HISTORY*/

    #if (!CONWAY_CHECKING_BOARD_MINIMIZE)
        board->copyBoard(board_next);
    #endif /* !CONWAY_CHECKING_BOARD_MINIMIZE */

    #if (CONWAY_CHECK_IF_IN_CYCLE)
        if (!(number_of_rounds_running % 2)) {

            #if (GAME_DEBUG)
                Serial.println("Print board two moves ago");

                board_two_ago->print();
                board->print();
            #endif /* GAME_DEBUG */

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

                #if (GAME_DEBUG)
                    Serial.println("Board Stuck in cycle");
                #endif /* GAME_DEBUG */

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
}

#endif /* Conway_h */
