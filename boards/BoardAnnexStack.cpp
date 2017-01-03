#include "BoardAnnexStack.h"

/**
 * Init Board Annex Stack
 *
 * @param {uint8_t} width
 * @param {uint8_t} height
 */
BoardAnnexStack::BoardAnnexStack(uint8_t _width, uint8_t _height): Board(_width, _height) {
    top_row_i_row = 0;
    i_row_in_memory = 0;

    #if (CONWAY_CHECK_HISTORY)
        all_i_assigning_same = true;
    #endif /* CONWAY_CHECK_HISTORY */
}

/**
 * Get state of cell
 *
 * @param  {uint8_t} x  x coordiante
 * @param  {uint8_t} y  y coordiante
 * @return {uint8_t}    state
 */
uint8_t BoardAnnexStack::getState(uint8_t x, uint8_t y) {
    i_row_in_memory = y - top_row_i_row;

    return Board::getState(x, i_row_in_memory);
}

/**
 * Print board to Serial
 */
#if (CONWAY_DEBUG)
    void BoardAnnexStack::print() {

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
#endif /* CONWAY_DEBUG */

/**
 * Set state of cell
 *
 * @param  {uint8_t} x      x coordiante
 * @param  {uint8_t} y      y coordiante
 * @param  {uint8_t} state  state
 */
void BoardAnnexStack::setState(uint8_t x, uint8_t y, uint8_t state) {
    i_row_in_memory = y - top_row_i_row;

    // Serial.println("( " + String(x) + ", " + String(y) + ") => (" + String(x) + ", " + String(i_row_in_memory) + " )");

    Board::setState(x, i_row_in_memory, state);
}

/**
 * Set state of cell to alive
 *
 * @param  {uint8_t} x  x coordiante
 * @param  {uint8_t} y  y coordiante
 */
void BoardAnnexStack::setAlive(uint8_t x, uint8_t y) {
    i_row_in_memory = y - top_row_i_row;

    Board::setAlive(x, i_row_in_memory);
}

/**
 * Set state of cell to dead
 *
 * @param  {uint8_t} x  x coordiante
 * @param  {uint8_t} y  y coordiante
 */
void BoardAnnexStack::setDead(uint8_t x, uint8_t y) {
    i_row_in_memory = y - top_row_i_row;

    Board::setDead(x, i_row_in_memory);
}


/**
 * Reset board
 */
void BoardAnnexStack::reset() {
    top_row_i_row = 0;
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
    bool BoardAnnexStack::copyRow(Board * other_board) {
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
    void BoardAnnexStack::copyRow(Board * other_board) {
        for (uint8_t i_index = 0; i_index < other_board->getHeight(); i_index++) {
            other_board->setState(i_index, top_row_i_row, getState(i_index, top_row_i_row));
        }

        finishRow();
    }
#endif /* CONWAY_CHECK_HISTORY */

/**
 * Shift memory up by 1. NULL out bottom row
 */
void BoardAnnexStack::finishRow() {
    for (i_col_annex = 0; i_col_annex < getWidth(); i_col_annex++) {
        setState(i_col_annex, top_row_i_row, getState(i_col_annex, top_row_i_row + 1));
        setState(i_col_annex, top_row_i_row + 1, CELL_STATE_DEAD);
    }

    top_row_i_row++;
}
