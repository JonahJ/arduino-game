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



    // if(y == top_row_i_row) {
    //     Serial.println("FROM TOP! " + String(x) + " = " + String(Board::getState(x, 0)));
    // } else {
    //     Serial.println("FROM Bottom! " + String(x) + " = " + String(Board::getState(x, 1)));

    // }

    i_row_in_memory = y - top_row_i_row;

    return Board::getState(x, i_row_in_memory);
}

void BoardNext::print(bool verbose) {
    Serial.println("Current Top Row: " + String(top_row_i_row));
    Board::print(verbose);
}

void BoardNext::setState(uint8_t x, uint8_t y, uint8_t state) {

    // Serial.println("y: " + String(y) + " == " + String(top_row_i_row) + " => " + String(y == top_row_i_row));


    i_row_in_memory = y - top_row_i_row;

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
    // Serial.println("Copying over " + String(top_row_i_row));

    for (uint8_t i_col = 0; i_col < width; i_col++) {
        // other_board->setState(i_col, top_row_i_row, BoardNext::getState(i_col, top_row_i_row));


        // Serial.println("cell " + String(i_col) + " = " + String(Board::getState(i_col, 0)));

        /**
         * Check if row is same
         */
        if (rows_same) rows_same &= (other_board->getState(i_col, top_row_i_row) == Board::getState(i_col, 0));



        other_board->setState(i_col, top_row_i_row, Board::getState(i_col, 0));

        // other_board->setState(i_col, top_row_i_row, CELL_STATE_ALIVE);


    }

    return rows_same;
}

void BoardNext::finishRow() {
    for (uint8_t i_col = 0; i_col < width; i_col++) {
        for (uint8_t i_row = 0; i_row < height; i_row++) {

            if (i_row == 0) Board::setState(i_col, 0, Board::getState(i_col, 1));
            else Board::setState(i_col, i_row, CELL_STATE_DEAD);
            // if (i_row < height - 1) Board::setState(i_col, i_row, getState(i_col, i_row + 1));
            // else Board::setState(i_col, i_row, CELL_STATE_DEAD);
        }
    }

    // Serial.println("Deleting row: " + String(top_row_i_row));

    top_row_i_row++;
}
#endif /* BoardNext_h */
