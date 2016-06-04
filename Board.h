#ifndef Board_h
#define Board_h

/*******************************************************************************
 *                               Board Settings                                *
 *******************************************************************************/

/**
 * Debug mode enabled additional checking and printouts. May negatively affect
 * performance
 */
#ifndef BOARD_DEBUG
    #define BOARD_DEBUG false
#endif /* BOARD_DEBUG */

/**
 * Print state DEAD or not. When false requires two lookups to a given cell, but
 * prints a blank string to the Serial instead of the CELL_STATE_DEAD state.
 */
#ifndef BOARD_PRINT_CELL_STATE_DEAD
    #define BOARD_PRINT_CELL_STATE_DEAD false
#endif /* BOARD_PRINT_CELL_STATE_DEAD */


/*******************************************************************************
 *                                 Cell Settings                               *
 *******************************************************************************/

/**
 * How many cells are to be stored in a single 8-bites, i.e. 1 byte. The more
 * stored together will decrease memory usage and allow the CPU to run quicker
 * due to less context switching. The lower this number goes the more
 * information that we can store per cell, ie colors.
 *
 *              --------------------------------------------------
 *              | Cells Per Byte   |   Number of Possible States |
 *              |                  |        = 2 ^ (num bits)     |
 *              --------------------------------------------------
 *              |        8         |           2                 |
 *              |        4         |           4                 |
 *              |        2         |           16                |
 *              |        1         |           256               |
 *              --------------------------------------------------
 */
#ifndef CELLS_PER_BYTE
    #define CELLS_PER_BYTE (uint8_t)4
#endif /* CELLS_PER_BYTE */

/**
 * How large a cell is in bits
 */
#ifndef CELL_WIDTH
    #define CELL_WIDTH (uint8_t)8 / CELLS_PER_BYTE
#endif /* CELL_WIDTH */


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
 *                                   Board                                     *
 *******************************************************************************/

class Board {
private:
    uint8_t width;
    uint8_t height;
    uint8_t ** board;

    uint8_t num_columns;
    uint8_t i_col_count;

    uint8_t cell_col;
    uint8_t i_cell_col;
    uint8_t i_cell_col_value;
    uint8_t cell_state;

protected:
    uint8_t i_col;
    uint8_t i_row;

    uint8_t i_col_annex;
    uint8_t i_row_annex;

public:
    Board(uint8_t _width, uint8_t _height);

    uint8_t getWidth() const;
    uint8_t getHeight() const;
    uint8_t getState(uint8_t x, uint8_t y);
    void print(bool verbose);

    void setState(uint8_t x, uint8_t y, uint8_t state);
    void setAlive(uint8_t x, uint8_t y);
    void setDead(uint8_t x, uint8_t y);

    void reset();

    void copyBoard(Board * other_board);
};

/**
 * Init Board Annex
 *
 * @param {int} width
 * @param {int} height
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

uint8_t Board::getWidth() const {
    return width;
}

uint8_t Board::getHeight() const {
    return height;
}

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

void Board::print(bool verbose = false) {
    if (verbose) {
        Serial.println("Printing Board");
        Serial.println("Width " + String(width));
        Serial.println("Height " + String(height));
        // Serial.println("Any Active: " + String(any_cells_alive));
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

void Board::setAlive(uint8_t x, uint8_t y) {
    setState(x, y, CELL_STATE_ALIVE);
}
void Board::setDead(uint8_t x, uint8_t y) {
    setState(x, y, CELL_STATE_DEAD);
}

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

void Board::copyBoard(Board * other_board) {
    for (i_col_annex = 0; i_col_annex < width; i_col_annex++) {
        for (i_row_annex = 0; i_row_annex < height; i_row_annex++) {
            setState(i_col_annex, i_row_annex, other_board->getState(i_col_annex, i_row_annex));
        }
    }
}



#endif /* Board_h */
