#ifndef BoardAnnex_h
#define BoardAnnex_h

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
 *                                Board Annex                                  *
 *******************************************************************************/

class BoardAnnex: public Board {
private:
    uint8_t size_wise;
    bool width_wise;

    uint8_t reader_i;
    bool i_writer;

    #if (CONWAY_CHECK_HISTORY)
        bool all_i_assigning_same;
    #endif /* CONWAY_CHECK_HISTORY */

public:
    BoardAnnex(uint8_t _width, uint8_t _height);

    #if (CONWAY_DEBUG)
        void print();
    #endif /* CONWAY_DEBUG */

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

#endif /* BoardAnnex_h */
