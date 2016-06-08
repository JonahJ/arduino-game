#ifndef Conway_h
#define Conway_h

/*******************************************************************************
 *                                  Includes                                   *
 *******************************************************************************/

#include <Arduino.h>

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#ifndef PSTR
    #define PSTR /* Make Arduino Due happy */
#endif /* PSTR */


/*******************************************************************************
 *                              Conway Settings                                *
 *******************************************************************************/

/**
 * Debug mode enables certain verbose messaging, including printing out the
 * current state. This may slow down the board speed if board is large
 */
#ifndef CONWAY_DEBUG
    #define CONWAY_DEBUG false
#endif /* CONWAY_DEBUG */

/**
 * Set to above 0 to have a max number moves before automatic reset. Quick and
 * dirty way to not get stuck in a repeated state without having 2 full boards
 * or a hash map.
 */
#ifndef CONWAY_MAX_MOVES
    #define CONWAY_MAX_MOVES -1
#endif /* CONWAY_MAX_MOVES */

/**
 * Count moves
 */
#ifndef CONWAY_COUNT_MOVES
    #define CONWAY_COUNT_MOVES true
#endif /* CONWAY_COUNT_MOVES */

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
 * Check history of round - 2 ago. This way if in cycle we can terminate.
 * Requires much more memory.
 */
#ifndef CONWAY_CHECK_IF_IN_CYCLE
    #define CONWAY_CHECK_IF_IN_CYCLE true

    #undef CONWAY_COUNT_MOVES
    #ifndef CONWAY_COUNT_MOVES
        #define CONWAY_COUNT_MOVES true
    #endif /* CONWAY_COUNT_MOVES */
#endif /* CONWAY_CHECK_IF_IN_CYCLE */

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
 * Clear board on redraw
 */
#ifndef CLEAR_ON_REDRAW
    #define CLEAR_ON_REDRAW false
#endif CLEAR_ON_REDRAW /* CLEAR_ON_REDRAW */

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

/**
 * Draw in simple mode, i.e. less logic to follow and less context switching
 */
#ifndef CONWAY_DRAW_MODE_SIMPLE
    #define CONWAY_DRAW_MODE_SIMPLE 0
#endif /* CONWAY_DRAW_MODE_SIMPLE */

/**
 * Draw board row wise, with additional features optionally enabled
 */
#ifndef CONWAY_DRAW_MODE_ROW_WISE
    #define CONWAY_DRAW_MODE_ROW_WISE CONWAY_DRAW_MODE_SIMPLE + 1
#endif CONWAY_DRAW_MODE_ROW_WISE /* CONWAY_DRAW_MODE_ROW_WISE */

/**
 * Draw board spiral style. Assumes board is equal in width and height. Uses
 * a bit more memory than typical col -> row draw
 */
#ifndef CONWAY_DRAW_MODE_SPIRAL
    #define CONWAY_DRAW_MODE_SPIRAL CONWAY_DRAW_MODE_ROW_WISE + 1
#endif CONWAY_DRAW_MODE_SPIRAL /* CONWAY_DRAW_MODE_SPIRAL */

/**
 * Draw board spiral style. Assumes board is equal in width and height. Uses
 * a bit more memory than typical col -> row draw
 */
#ifndef CONWAY_DRAW_MODE
    #define CONWAY_DRAW_MODE CONWAY_DRAW_MODE_ROW_WISE
#endif CONWAY_DRAW_MODE /* CONWAY_DRAW_MODE */

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
    #define CONWAY_WIPE_EFFECT_DELAY 0
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
    #define CELL_STATE_MAX CELL_STATE_WIPE
#endif /* CELL_STATE_MAX */


/*******************************************************************************
 *                               Local Includes                                *
 *******************************************************************************/

#include "boards/Board.h"

#if (CONWAY_CHECKING_BOARD_MINIMIZE)
    #if (CONWAY_CHECKING_BOARD_USE_STACK)
        #include "boards/BoardAnnexStack.h"
    #else
        #include "boards/BoardAnnex.h"
    #endif /* CONWAY_CHECKING_BOARD_USE_STACK */
#endif /* CONWAY_CHECKING_BOARD_MINIMIZE */


/*******************************************************************************
 *                                   Conway                                    *
 *******************************************************************************/

class Conway {
private:
    Board * board;

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

    #if (CONWAY_COUNT_MOVES > 0)
        uint16_t number_of_rounds_running;
    #endif /* CONWAY_COUNT_MOVES */

    bool row_wise_annex;

    uint16_t * colors;

    uint8_t i_col;
    uint8_t i_row;
    uint8_t width;
    uint8_t height;

    bool any_cells_alive;

    #if (CONWAY_CHECK_HISTORY)
        bool board_same;
    #endif

    Adafruit_NeoMatrix * led_matrix;

    uint8_t num_cells_active_surrounding;

    uint8_t bound_col_min;
    uint8_t bound_col_max;
    uint8_t bound_row_min;
    uint8_t bound_row_max;

    uint8_t i_col_check;
    uint8_t i_row_check;

    #if (CONWAY_DRAW_MODE_SPIRAL)
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
    #if (CONWAY_ASSIGN_DENSITY)
        void _assignCurrentDensity();
    #endif /* CONWAY_ASSIGN_DENSITY */
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

#endif /* Conway_h */
