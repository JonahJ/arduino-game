#ifndef BoardAnnexStack_h
#define BoardAnnexStack_h

/*******************************************************************************
 *                                  Includes                                   *
 *******************************************************************************/

#include <Arduino.h>


/*******************************************************************************
 *                               Local Includes                                *
 *******************************************************************************/

#include "Board.h"


/*******************************************************************************
 *                               Board Settings                                *
 *******************************************************************************/

/**
 * Check history so that we can reset if in a "stuck" state, i.e. the same board
 * for more than 1 move
 */
#ifndef CONWAY_CHECK_HISTORY
    #define CONWAY_CHECK_HISTORY true
#endif /* CONWAY_CHECK_HISTORY */


/*******************************************************************************
 *                              Board Annex Stack                              *
 *******************************************************************************/

/**
 * Uses a stack of height = 2 to represent a full checking Board. Top row
 * represents stored final state of row back by 1, row 2 represents the temp
 * row to store current row's next state.
 */
class BoardAnnexStack: public Board {
private:
    uint8_t top_row_i_row;
    bool top_row_storing_annex;

    uint8_t i_row_in_memory;

    #if (CONWAY_CHECK_HISTORY)
        bool all_i_assigning_same;
    #endif /* CONWAY_CHECK_HISTORY */

public:
    BoardAnnexStack(uint8_t _width, uint8_t _height);

    uint8_t getState(uint8_t x, uint8_t y);
    void print();

    void setState(uint8_t x, uint8_t y, uint8_t state);
    void setAlive(uint8_t x, uint8_t y);
    void setDead(uint8_t x, uint8_t y);

    void reset();

    #if (CONWAY_CHECK_HISTORY)
        bool copyRow(Board * other_board);
    #else
        void copyRow(Board * other_board);
    #endif /* CONWAY_CHECK_HISTORY */

    void finishRow();
};

#endif /* BoardAnnexStack_h */
