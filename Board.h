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
    uint8_t ** board;

    uint8_t width;
    uint8_t height;
    uint8_t num_columns;

    uint8_t i_col_count;
    uint8_t i_col;
    uint8_t i_row;

    uint8_t cell_col;
    uint8_t i_cell_col;
    uint8_t i_cell_col_value;
    uint8_t cell_state;

public:
    Board(uint8_t _width, uint8_t _height);

    uint8_t getState(uint8_t x, uint8_t y);

    void setState(uint8_t x, uint8_t y, uint8_t state);
    void setAlive(uint8_t x, uint8_t y);
    void setDead(uint8_t x, uint8_t y);

    void reset();

    void copyBoard(Board * other_board);
};

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

    // for (i_col_count = 0; i_col_count < num_columns; i_col_count++) {
    //     for (i_col = 0; i_col < width; i_col++) {
    //         for (i_row = 0; i_row < height; i_row++) {
    //             // board[i_col_count][i_row] = (board[i_col_count][i_row] | (CELL_STATE_ALIVE << i_col));
    //             if (i_col_count == 0 && i_col == 1 && i_row == 1) {
    //                 // board[i_col_count][i_row] = 3;
    //                 board[i_col_count][i_row] = (board[i_col_count][i_row] | (CELL_STATE_ALIVE << i_col));
    //             }
    //         }
    //     }
    // }
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

    if (BOARD_DEBUG) {
        if (cell_state != CELL_STATE_DEAD) {
            Serial.println("--------------");
            Serial.println("( " + String(x) + ", " + String(y) + " ) -> " + String(cell_state));
            Serial.println("G: " + String(i_col_count));
            Serial.println("C: " + String(i_col));
            Serial.println("R: " + String(i_row));
            Serial.println("V: " + String(cell_state));
        }
    }

    return cell_state;
}

void Board::setState(uint8_t x, uint8_t y, uint8_t state) {
    i_col_count = x / CELLS_PER_BYTE;
    i_col = x % CELLS_PER_BYTE;
    i_row = y;

    cell_state = CELL_STATE_DEAD;

    if (state > CELL_STATE_ALIVE) { /* Avoid taking pow */
        if (state > pow(2, (CELL_WIDTH)) - 1) {
            if (BOARD_DEBUG) Serial.println("Warning, state is too high for CELL_WIDTH. Setting to max.");
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
    /**
     * Requires redeclaration of i_col and i_row because this function uses
     * internal functions
     */
    for (uint8_t i_col = 0; i_col < width; i_col++) {
        for (uint8_t i_row = 0; i_row < height; i_row++) {
            setState(i_col, i_row, other_board->getState(i_col, i_row));
        }
    }
}



#endif /* Board_h */
