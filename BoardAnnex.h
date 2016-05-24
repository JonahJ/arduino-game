#ifndef BoardAnnex_h
#define BoardAnnex_h

#include "Board.h"

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
 *                                 Board Next                                  *
 *******************************************************************************/


class BoardAnnex: public Board {
private:
    uint8_t reader_i_row;
    bool top_row_reader;

    bool rows_same;

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

BoardAnnex::BoardAnnex(uint8_t _width, uint8_t _height): Board(_width, _height) {
    reader_i_row = 0;
    top_row_reader = false;

    rows_same = true;
}

void BoardAnnex::print(bool verbose = false) {
    if (verbose) {
        Serial.println("Current Top Row: " + String(reader_i_row));
        Serial.println("Reader: " + String(top_row_reader));
    }

    Board::print();
}

void BoardAnnex::setState(uint8_t x, uint8_t y, uint8_t state) {
    Board::setState(x, top_row_reader, state);
}

void BoardAnnex::setAlive(uint8_t x, uint8_t y) {
    Board::setAlive(x, top_row_reader);
}

void BoardAnnex::setDead(uint8_t x, uint8_t y) {
    Board::setDead(x, top_row_reader);
}


void BoardAnnex::reset() {
    reader_i_row = 0;
    top_row_reader = false;

    Board::reset();
}

/**
 * Copies over the row designated by `reader_i_row`. Returns truth of the rows
 * being the same
 * @param  other_board Board to copy over to
 * @return             truth of the rows being the same
 */
bool BoardAnnex::copyRow(Board * other_board) {
    rows_same = true;

    for (uint8_t i_index = 0; i_index < other_board->getHeight(); i_index++) {
        /**
         * Check if row is same
         */
        if (rows_same) rows_same &= (other_board->getState(i_index, reader_i_row) == getState(i_index, !top_row_reader));

        other_board->setState(i_index, reader_i_row, getState(i_index, !top_row_reader));
    }

    reader_i_row++;

    return rows_same;
}

void BoardAnnex::finishRow() {
    top_row_reader = !top_row_reader;

    /**
     * Shift up by 1
     */
    for (i_col_annex = 0; i_col_annex < width; i_col_annex++) {
        Board::setState(i_col_annex, top_row_reader, CELL_STATE_DEAD);
    }
}
#endif /* BoardAnnex_h */
