#include "Board.h"

/**
 * Init Board Annex
 *
 * @param {uint8_t} width
 * @param {uint8_t} height
 */
Board::Board(uint8_t _width, uint8_t _height) {
    width = _width;
    height = _height;

    num_columns = width / CELLS_PER_BYTE;

    board = new uint8_t * [num_columns];

    for (i_col_count = 0; i_col_count < num_columns; i_col_count++) {
        board[i_col_count] = new uint8_t [height];
        for (i_row = 0; i_row < height; i_row++) {
            board[i_col_count][i_row] = NULL;
            board[i_col_count][i_row] = CELL_STATE_DEAD;
        }
    }
}

/**
 * Get width of board
 *
 * @return {uint8_t} width
 */
uint8_t Board::getWidth() const {
    return width;
}

/**
 * Get height of board
 *
 * @return {uint8_t} height
 */
uint8_t Board::getHeight() const {
    return height;
}

/**
 * Get state of cell
 *
 * @param  {uint8_t} x  x coordiante
 * @param  {uint8_t} y  y coordiante
 * @return {uint8_t}    state
 */
uint8_t Board::getState(uint8_t x, uint8_t y) {
    i_col_count = x / CELLS_PER_BYTE;
    i_col = x % CELLS_PER_BYTE;
    i_row = y;

    cell_state = CELL_STATE_DEAD;

    for (i_cell_col = 0; i_cell_col < CELL_WIDTH; i_cell_col++) {
        cell_col = i_col * CELL_WIDTH + i_cell_col;
        i_cell_col_value = ((board[i_col_count][i_row] >> cell_col) & 1);

        cell_state = (cell_state | (i_cell_col_value << i_cell_col));
    }

    #if (BOARD_DEBUG)
        if (cell_state != CELL_STATE_DEAD) {
            Serial.println("--------------");
            Serial.println("( " + String(x) + ", " + String(y) + " ) -> " + String(cell_state));
            Serial.println("G: " + String(i_col_count));
            Serial.println("C: " + String(i_col));
            Serial.println("R: " + String(i_row));
            Serial.println("V: " + String(cell_state));
        }
    #endif /* BOARD_DEBUG */

    return cell_state;
}

/**
 * Print board to Serial
 */
void Board::print() {

    #if (BOARD_DEBUG_VERBOSE)
        // Serial.println("Printing Board");
        Serial.println("Width " + String(width));
        Serial.println("Height " + String(height));
        // Serial.println("Any Active: " + String(any_cells_alive));
    #endif /* BOARD_DEBUG_VERBOSE */

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

    for (i_row_annex = 0; i_row_annex < height; i_row_annex++) {
        if (height >= 10) {
            if (i_row_annex < 10) Serial.print(" ");
        }
        Serial.print(i_row_annex);
        Serial.print(" | ");

        for (uint8_t i_col = 0; i_col < width; i_col++) {
            if (i_col > 0) Serial.print("  ");

            if (i_col >= 10) Serial.print(" ");

            #if (BOARD_PRINT_CELL_STATE_DEAD)
                Serial.print(getState(i_col, i_row_annex));
            #else
                if (getState(i_col, i_row_annex) == CELL_STATE_DEAD) Serial.print(" ");
                else Serial.print(getState(i_col, i_row_annex));
            #endif /* BOARD_PRINT_CELL_STATE_DEAD */
        }
        Serial.print("\n");
    }
}

/**
 * Set state of cell
 *
 * @param  {uint8_t} x      x coordiante
 * @param  {uint8_t} y      y coordiante
 * @param  {uint8_t} state  state
 */
void Board::setState(uint8_t x, uint8_t y, uint8_t state) {
    i_col_count = x / CELLS_PER_BYTE;
    i_col = x % CELLS_PER_BYTE;
    i_row = y;

    cell_state = CELL_STATE_DEAD;

    if (state > CELL_STATE_ALIVE) { /* Avoid taking pow */
        if (state > pow(2, (CELL_WIDTH)) - 1) {
            #if (BOARD_DEBUG)
                Serial.println("Warning, state is too high for CELL_WIDTH. Setting to max.");
            #endif /* BOARD_DEBUG */

            state = pow(2, (CELL_WIDTH)) - 1;
        }
    }

    for (i_cell_col = 0; i_cell_col < CELL_WIDTH; i_cell_col++) {
        cell_state = (state >> i_cell_col) & 1;
        cell_col = i_col * CELL_WIDTH + i_cell_col;

        board[i_col_count][i_row] ^= (-cell_state ^ board[i_col_count][i_row]) & (1 << cell_col);
    }

    return;
}

/**
 * Set state of cell to alive
 *
 * @param  {uint8_t} x  x coordiante
 * @param  {uint8_t} y  y coordiante
 */
void Board::setAlive(uint8_t x, uint8_t y) {
    setState(x, y, CELL_STATE_ALIVE);
}

/**
 * Set state of cell to dead
 *
 * @param  {uint8_t} x  x coordiante
 * @param  {uint8_t} y  y coordiante
 */
void Board::setDead(uint8_t x, uint8_t y) {
    setState(x, y, CELL_STATE_DEAD);
}

/**
 * Reset board
 */
void Board::reset() {
    for (i_col_count = 0; i_col_count < num_columns; i_col_count++) {
        for (i_row = 0; i_row < height; i_row++) {
            board[i_col_count][i_row] = NULL;
        }
    }

    for (i_col_count = 0; i_col_count < num_columns; i_col_count++) {
        for (i_col = 0; i_col < width; i_col++) {
            for (i_row = 0; i_row < height; i_row++) {
                board[i_col_count][i_row] = (board[i_col_count][i_row] | (CELL_STATE_DEAD << i_col));
            }
        }
    }
}

/**
 * Copy this board to other board
 *
 * @param  {Board *} other_board    other board
 */
void Board::copyBoard(Board * other_board) {
    for (i_col_annex = 0; i_col_annex < width; i_col_annex++) {
        for (i_row_annex = 0; i_row_annex < height; i_row_annex++) {
            setState(i_col_annex, i_row_annex, other_board->getState(i_col_annex, i_row_annex));
        }
    }
}
