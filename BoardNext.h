#ifndef BoardNext_h
#define BoardNext_h

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


class BoardNext: public Board {
private:
    uint8_t top_row_i_row;
    bool top_row_storing_annex;

    uint8_t i_row_in_memory;
    bool rows_same;

public:
    BoardNext(uint8_t _width, uint8_t _height);

    uint8_t getState(uint8_t x, uint8_t y);
    void print(bool verbose);

    void setState(uint8_t x, uint8_t y, uint8_t state);
    void setAlive(uint8_t x, uint8_t y);
    void setDead(uint8_t x, uint8_t y);

    void reset();

    bool copyRow(Board * other_board);
    void finishRow();
};

BoardNext::BoardNext(uint8_t _width, uint8_t _height): Board(_width, _height) {
    top_row_i_row = 0;
    i_row_in_memory = 0;

    rows_same = true;
}

uint8_t BoardNext::getState(uint8_t x, uint8_t y) {
    i_row_in_memory = y - top_row_i_row;

    return Board::getState(x, i_row_in_memory);
}

void BoardNext::print(bool verbose = false) {

    if (verbose) {
        Serial.println("Current Top Row: " + String(top_row_i_row));
    }

    for (i_col = 0; i_col < width; i_col++) {
        if (i_col == 0) {
            Serial.print("  ");

            if (height >= 10) Serial.print(" ");
        }

        Serial.print("  ");
        Serial.print(i_col);
    }
    Serial.print("\n");

    for (i_col = 0; i_col < width; i_col++) {
        if (i_col == 0) {
            Serial.print("  ");

            if (height >= 10) Serial.print(" ");
        }

        Serial.print("---");

        if (i_col >= 10) Serial.print("-");
    }
    Serial.print("\n");

    for (i_col_annex = top_row_i_row; i_col_annex < height + top_row_i_row; i_col_annex++) {
        if (height >= 10) {
            if (i_col_annex < 10) Serial.print(" ");
        }
        Serial.print(i_col_annex + top_row_i_row);
        Serial.print(" | ");

        for (uint8_t i_col = 0; i_col < width; i_col++) {
            if (i_col > 0) Serial.print("  ");

            if (i_col >= 10) Serial.print(" ");

            Serial.print(getState(i_col, i_col_annex));
        }
        Serial.print("\n");
    }
}

void BoardNext::setState(uint8_t x, uint8_t y, uint8_t state) {

    // Serial.println("y: " + String(y) + " == " + String(top_row_i_row) + " => " + String(y == top_row_i_row));

    i_row_in_memory = y - top_row_i_row;

    // Serial.println("( " + String(x) + ", " + String(y) + ") => (" + String(x) + ", " + String(i_row_in_memory) + " )");

    Board::setState(x, i_row_in_memory, state);
}

void BoardNext::setAlive(uint8_t x, uint8_t y) {
    i_row_in_memory = y - top_row_i_row;

    Board::setAlive(x, i_row_in_memory);
}

void BoardNext::setDead(uint8_t x, uint8_t y) {
    i_row_in_memory = y - top_row_i_row;

    Board::setDead(x, i_row_in_memory);
}


void BoardNext::reset() {
    top_row_i_row = 0;
    Board::reset();
}

/**
 * Copies over the row designated by `top_row_i_row`. Returns truth of the rows
 * being the same
 * @param  other_board Board to copy over to
 * @return             truth of the rows being the same
 */
bool BoardNext::copyRow(Board * other_board) {
    rows_same = true;

    for (uint8_t i_index = 0; i_index < other_board->getHeight(); i_index++) {
        // Serial.println("cell " + String(i_col) + " = " + String(Board::getState(i_col, 0)));

        /**
         * Check if row is same
         */
        if (rows_same) rows_same &= (other_board->getState(i_index, top_row_i_row) == getState(i_index, top_row_i_row));

        other_board->setState(i_index, top_row_i_row, getState(i_index, top_row_i_row));
    }

    finishRow();

    return rows_same;
}

void BoardNext::finishRow() {
    /**
     * Shift up by 1
     */
    for (i_col_annex = 0; i_col_annex < width; i_col_annex++) {
        setState(i_col_annex, top_row_i_row, getState(i_col_annex, top_row_i_row + 1));
        setState(i_col_annex, top_row_i_row + 1, CELL_STATE_DEAD);
    }

    top_row_i_row++;
}
#endif /* BoardNext_h */
