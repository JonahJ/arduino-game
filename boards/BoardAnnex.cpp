#include "BoardAnnex.h"

/**
 * Init Board Annex
 *
 * @param {uint8_t} width
 * @param {uint8_t} height
 */
BoardAnnex::BoardAnnex(uint8_t _width, uint8_t _height): Board(_width, _height) {
    size_wise = max(_width, _height);
    width_wise = (_height <= _width);

    reader_i = 0;
    i_writer = false;

    #if (CONWAY_CHECK_HISTORY)
        all_i_assigning_same = true;
    #endif
}

/**
 * Print board to Serial
 */
void BoardAnnex::print() {

    #if (BOARD_DEBUG_VERBOSE)
        Serial.println("Current Top Index: " + String(reader_i));
        Serial.println("Reader: " + String(i_writer));
    #endif /* BOARD_DEBUG_VERBOSE */

    Board::print();
}

/**
 * Set state of cell
 *
 * @param  {uint8_t} x      x coordiante
 * @param  {uint8_t} y      y coordiante
 * @param  {uint8_t} state  state
 */
void BoardAnnex::setState(uint8_t x, uint8_t y, uint8_t state) {
    if (width_wise) Board::setState(x, i_writer, state);
    else Board::setState(i_writer, y, state);
}

/**
 * Set state of cell to alive
 *
 * @param  {uint8_t} x  x coordiante
 * @param  {uint8_t} y  y coordiante
 */
void BoardAnnex::setAlive(uint8_t x, uint8_t y) {
    if (width_wise) Board::setAlive(x, i_writer);
    else Board::setAlive(i_writer, y);
}

/**
 * Set state of cell to dead
 *
 * @param  {uint8_t} x  x coordiante
 * @param  {uint8_t} y  y coordiante
 */
void BoardAnnex::setDead(uint8_t x, uint8_t y) {
    if (width_wise) Board::setDead(x, i_writer);
    else Board::setDead(i_writer, y);
}

/**
 * Reset board
 */
void BoardAnnex::reset() {
    reader_i = 0;
    i_writer = false;

    Board::reset();
}

/**
 * Copies over the row designated by `reader_i`. Returns truth of the rows
 * being the same
 *
 * @param  {Board *}        other_board     Board to copy over to
 * @return {bool or void}                   truth of the rows being the same if
 *                                          CONWAY_CHECK_HISTORY is true
 */
#if (CONWAY_CHECK_HISTORY)
    bool BoardAnnex::copyRow(Board * other_board) {
        all_i_assigning_same = true;

        for (i_col_annex = 0; i_col_annex < size_wise; i_col_annex++) {
            /**
             * Check if row is same
             */
            if (all_i_assigning_same) all_i_assigning_same &= (other_board->getState(i_col_annex, reader_i) == getState(i_col_annex, !i_writer));

            other_board->setState(i_col_annex, reader_i, getState(i_col_annex, !i_writer));
        }

        reader_i++;

        return all_i_assigning_same;
    }
#else
    void BoardAnnex::copyRow(Board * other_board) {
        for (i_col_annex = 0; i_col_annex < size_wise; i_col_annex++) {
            other_board->setState(i_col_annex, reader_i, getState(i_col_annex, !i_writer));
        }

        reader_i++;
    }
#endif /* CONWAY_CHECK_HISTORY */

/**
 * Reset reading row to NULL. Then set that row to be writable
 */
void BoardAnnex::finishRow() {
    i_writer = !i_writer;

    for (i_col_annex = 0; i_col_annex < size_wise; i_col_annex++) {
        if (width_wise) Board::setState(i_col_annex, i_writer, CELL_STATE_DEAD);
        else Board::setState(i_writer, i_col_annex, CELL_STATE_DEAD);
    }
}
