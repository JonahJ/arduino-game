#ifndef Board_h
#define Board_h

/*******************************************************************************
 *                                  Includes                                   *
 *******************************************************************************/

#include <Arduino.h>


/*******************************************************************************
 *                               Board Settings                                *
 *******************************************************************************/

/**
 * Debug mode enabled additional checking and printouts. May negatively affect
 * performance
 */
#ifndef BOARD_DEBUG
    #define BOARD_DEBUG false
#endif /* BOARD_DEBUG */

/**
 * Debug in verbose mode. Additional print statements
 */
#ifndef BOARD_DEBUG_VERBOSE
    #define BOARD_DEBUG_VERBOSE false
#endif /* BOARD_DEBUG_VERBOSE */

/**
 * Print state DEAD or not. When false requires two lookups to a given cell, but
 * prints a blank string to the Serial instead of the CELL_STATE_DEAD state.
 */
#ifndef BOARD_PRINT_CELL_STATE_DEAD
    #define BOARD_PRINT_CELL_STATE_DEAD false
#endif /* BOARD_PRINT_CELL_STATE_DEAD */


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


/*******************************************************************************
 *                                 Cell Settings                               *
 *******************************************************************************/

/**
 * How many cells are to be stored in a single 8-bites, i.e. 1 byte. The more
 * stored together will decrease memory usage and allow the CPU to run quicker
 * due to less context switching. The lower this number goes the more
 * information that we can store per cell, ie colors.
 *
 *              --------------------------------------------------
 *              | Cells Per Byte   |   Number of Possible States |
 *              |                  |        = 2 ^ (num bits)     |
 *              --------------------------------------------------
 *              |        8         |           2                 |
 *              |        4         |           4                 |
 *              |        2         |           16                |
 *              |        1         |           256               |
 *              --------------------------------------------------
 */
#ifndef CELLS_PER_BYTE
    #define CELLS_PER_BYTE (uint8_t)4
#endif /* CELLS_PER_BYTE */

/**
 * How large a cell is in bits
 */
#ifndef CELL_WIDTH
    #define CELL_WIDTH (uint8_t)8 / CELLS_PER_BYTE
#endif /* CELL_WIDTH */


/*******************************************************************************
 *                                Cell States                                  *
 *******************************************************************************/

#ifndef CELL_STATE_DEAD
    #define CELL_STATE_DEAD 0
#endif /* CELL_STATE_DEAD */

#ifndef CELL_STATE_ALIVE
    #define CELL_STATE_ALIVE 1
#endif /* CELL_STATE_ALIVE */

/*******************************************************************************
 *                                   Board                                     *
 *******************************************************************************/

class Board {
private:
    uint8_t width;
    uint8_t height;
    uint8_t ** board;

    uint8_t num_columns;
    uint8_t i_col_count;

    uint8_t cell_col;
    uint8_t i_cell_col;
    uint8_t i_cell_col_value;
    uint8_t cell_state;

protected:
    uint8_t i_col;
    uint8_t i_row;

    uint8_t i_col_annex;
    uint8_t i_row_annex;

public:
    Board(uint8_t _width, uint8_t _height);

    uint8_t getWidth() const;
    uint8_t getHeight() const;
    uint8_t getState(uint8_t x, uint8_t y);

    #if (CONWAY_DEBUG)
        void print();
    #endif /* CONWAY_DEBUG */

    void setState(uint8_t x, uint8_t y, uint8_t state);
    void setAlive(uint8_t x, uint8_t y);
    void setDead(uint8_t x, uint8_t y);

    void reset();

    void copyBoard(Board * other_board);
};

#endif /* Board_h */
