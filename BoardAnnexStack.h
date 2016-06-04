#ifndef BoardAnnexStack_h
#define BoardAnnexStack_h

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
 *                                 Board Next                                  *
 *******************************************************************************/

/**
 * Uses a stack of height = 2 to represent a full checking Board. Top row
 * represents stored final state of row back by 1, row 2 represents the temp
 * row to store current row's next state.
 */
class BoardAnnex: public Board {
private:
    uint8_t top_row_i_row;
    bool top_row_storing_annex;

    uint8_t i_row_in_memory;

    #if (CONWAY_CHECK_HISTORY)
        bool all_i_assigning_same;
    #endif /* CONWAY_CHECK_HISTORY */

public:
    BoardAnnex(uint8_t _width, uint8_t _height);

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

BoardAnnex::BoardAnnex(uint8_t _width, uint8_t _height): Board(_width, _height) {
    top_row_i_row = 0;
    i_row_in_memory = 0;

    #if (CONWAY_CHECK_HISTORY)
        all_i_assigning_same = true;
    #endif
}

uint8_t BoardAnnex::getState(uint8_t x, uint8_t y) {
    i_row_in_memory = y - top_row_i_row;

    return Board::getState(x, i_row_in_memory);
}

void BoardAnnex::print() {

    #if (BOARD_DEBUG_VERBOSE)
        Serial.println("Current Top Row: " + String(top_row_i_row));
    #endif /* BOARD_DEBUG_VERBOSE */

    for (i_col = 0; i_col < getWidth(); i_col++) {
        if (i_col == 0) {
            Serial.print("  ");

            if (getHeight() >= 10) Serial.print(" ");
        }

        Serial.print("  ");
        Serial.print(i_col);
    }
    Serial.print("\n");

    for (i_col = 0; i_col < getWidth(); i_col++) {
        if (i_col == 0) {
            Serial.print("  ");

            if (getHeight() >= 10) Serial.print(" ");
        }

        Serial.print("---");

        if (i_col >= 10) Serial.print("-");
    }
    Serial.print("\n");

    for (i_col_annex = top_row_i_row; i_col_annex < getHeight() + top_row_i_row; i_col_annex++) {
        if (getHeight() >= 10) {
            if (i_col_annex < 10) Serial.print(" ");
        }
        Serial.print(i_col_annex + top_row_i_row);
        Serial.print(" | ");

        for (uint8_t i_col = 0; i_col < getWidth(); i_col++) {
            if (i_col > 0) Serial.print("  ");

            if (i_col >= 10) Serial.print(" ");

            #if (BOARD_PRINT_CELL_STATE_DEAD)
                Serial.print(getState(i_col, i_col_annex));
            #else
                if (getState(i_col, i_col_annex) == CELL_STATE_DEAD) Serial.print(" ");
                else Serial.print(getState(i_col, i_col_annex));
            #endif /* BOARD_PRINT_CELL_STATE_DEAD */
        }
        Serial.print("\n");
    }
}

void BoardAnnex::setState(uint8_t x, uint8_t y, uint8_t state) {
    i_row_in_memory = y - top_row_i_row;

    // Serial.println("( " + String(x) + ", " + String(y) + ") => (" + String(x) + ", " + String(i_row_in_memory) + " )");

    Board::setState(x, i_row_in_memory, state);
}

void BoardAnnex::setAlive(uint8_t x, uint8_t y) {
    i_row_in_memory = y - top_row_i_row;

    Board::setAlive(x, i_row_in_memory);
}

void BoardAnnex::setDead(uint8_t x, uint8_t y) {
    i_row_in_memory = y - top_row_i_row;

    Board::setDead(x, i_row_in_memory);
}


void BoardAnnex::reset() {
    top_row_i_row = 0;
    Board::reset();
}

/**
 * Copies over the row designated by `top_row_i_row`. Returns truth of the rows
 * being the same
 * @param  other_board Board to copy over to
 * @return             truth of the rows being the same
 */

#if (CONWAY_CHECK_HISTORY)
    bool BoardAnnex::copyRow(Board * other_board) {
        all_i_assigning_same = true;

        for (uint8_t i_index = 0; i_index < other_board->getHeight(); i_index++) {
            // Serial.println("cell " + String(i_col) + " = " + String(Board::getState(i_col, 0)));

            /**
             * Check if row is same
             */
            if (all_i_assigning_same) all_i_assigning_same &= (other_board->getState(i_index, top_row_i_row) == getState(i_index, top_row_i_row));

            other_board->setState(i_index, top_row_i_row, getState(i_index, top_row_i_row));
        }

        finishRow();

        return all_i_assigning_same;
    }
#else
    void BoardAnnex::copyRow(Board * other_board) {
        for (uint8_t i_index = 0; i_index < other_board->getHeight(); i_index++) {
            other_board->setState(i_index, top_row_i_row, getState(i_index, top_row_i_row));
        }

        finishRow();
    }
#endif /* CONWAY_CHECK_HISTORY */

void BoardAnnex::finishRow() {
    /**
     * Shift memory up by 1. NULL out bottom row
     */
    for (i_col_annex = 0; i_col_annex < getWidth(); i_col_annex++) {
        setState(i_col_annex, top_row_i_row, getState(i_col_annex, top_row_i_row + 1));
        setState(i_col_annex, top_row_i_row + 1, CELL_STATE_DEAD);
    }

    top_row_i_row++;
}
#endif /* BoardAnnexStack_h */
