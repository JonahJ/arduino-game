#ifndef BoardAnnex_h
#define BoardAnnex_h

#include "Board.h"

/*******************************************************************************
 *                                 Board Next                                  *
 *******************************************************************************/

class BoardAnnex: public Board {
private:
    uint8_t size_wise;
    bool width_wise;

    uint8_t reader_i;
    bool i_writer;

    bool all_i_assigning_same;

public:
    BoardAnnex(uint8_t _width, uint8_t _height);

    void print(bool verbose);

    void setState(uint8_t x, uint8_t y, uint8_t state);
    void setAlive(uint8_t x, uint8_t y);
    void setDead(uint8_t x, uint8_t y);

    void reset();

    bool copyRow(Board * other_board);
    void finishRow();
};

/**
 * Init Board Annex
 *
 * @param {int} width
 * @param {int} height
 */
BoardAnnex::BoardAnnex(uint8_t _width, uint8_t _height): Board(_width, _height) {
    size_wise = max(_width, _height);
    width_wise = (_height <= _width);

    reader_i = 0;
    i_writer = false;

    all_i_assigning_same = true;
}

/**
 * Print board and optionally diagnostic info
 *
 * @param {Bool} verbose mode for more diagnostic info
 */
void BoardAnnex::print(bool verbose = false) {
    if (verbose) {
        Serial.println("Current Top Index: " + String(reader_i));
        Serial.println("Reader: " + String(i_writer));
    }

    Board::print();
}

/**
 * Set (x, i_writer) to state
 *
 * @param {int} x
 * @param {int} y, but we will ignore and use i_writer
 * @param {int} state state of cell
 */
void BoardAnnex::setState(uint8_t x, uint8_t y, uint8_t state) {
    if (width_wise) Board::setState(x, i_writer, state);
    else Board::setState(i_writer, y, state);
}

/**
 * Set (x, i_writer) to alive
 *
 * @param {int} x
 * @param {int} y, but we will ignore and use i_writer
 */
void BoardAnnex::setAlive(uint8_t x, uint8_t y) {
    if (width_wise) Board::setAlive(x, i_writer);
    else Board::setAlive(i_writer, y);
}

/**
 * Set (x, i_writer) to dead
 *
 * @param {int} x
 * @param {int} y, but we will ignore and use i_writer
 */
void BoardAnnex::setDead(uint8_t x, uint8_t y) {
    if (width_wise) Board::setDead(x, i_writer);
    else Board::setDead(i_writer, y);
}

/**
 * Resets Board
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
 * @param  {Board} other_board  Board to copy over to
 * @return {bool}               truth of the rows being the same
 */
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
#endif /* BoardAnnex_h */
