//
//  Board.h
//  char
//
//  Created by Jonah Joselow on 3/23/16.
//  Copyright © 2016 Jonah Joselow. All rights reserved.
//

// unsigned char i_col = 0;
// unsigned char i_row = 0;

#ifndef Board_h
#define Board_h

#ifndef CELL_STATE_ALIVE
    #define CELL_STATE_ALIVE 1
#endif /* CELL_STATE_ALIVE */

#ifndef CELL_STATE_DEAD
    #define CELL_STATE_DEAD 0
#endif /* CELL_STATE_DEAD */

#ifndef CELLS_PER_BYTE
    #define CELLS_PER_BYTE 8
#endif /* CELLS_PER_BYTE */

#ifndef CELL_WIDTH
    #define CELL_WIDTH 8 / CELLS_PER_BYTE
#endif /* CELL_WIDTH */

class Board {
private:
    unsigned char ** board;

    unsigned char width;
    unsigned char height;
    unsigned char num_columns;

    unsigned char i_col_count;
    unsigned char i_col;
    unsigned char i_row;


public:
    Board(unsigned char _width, unsigned char _height);

    int getState(unsigned char x, unsigned char y);

    void setState(unsigned char x, unsigned char y, int state);
    void setAlive(unsigned char x, unsigned char y);
    void setDead(unsigned char x, unsigned char y);

    void reset();
};

Board::Board(unsigned char _width, unsigned char _height){
    width = _width;
    height = _height;

    num_columns = width / CELLS_PER_BYTE;


    board = new unsigned char * [num_columns];

    for (i_col_count = 0; i_col_count < num_columns; i_col_count++) {
        board[i_col_count] = new unsigned char [height];
        for(i_row = 0; i_row < height; i_row++){
            board[i_col_count][i_row] = NULL;
            board[i_col_count][i_row] = CELL_STATE_DEAD;
        }
    }

    // for (i_col_count = 0; i_col_count < num_columns; i_col_count++) {
    //     for (i_col = 0; i_col < width; i_col++) {
    //         for(i_row = 0; i_row < height; i_row++) {
    //             // board[i_col_count][i_row] = (board[i_col_count][i_row] | (CELL_STATE_ALIVE << i_col));
    //             if(i_col_count == 0 && i_col == 1 && i_row == 1){
    //                 // board[i_col_count][i_row] = 3;
    //                 board[i_col_count][i_row] = (board[i_col_count][i_row] | (CELL_STATE_ALIVE << i_col));
    //             }
    //         }
    //     }
    // }
}

int Board::getState(unsigned char x, unsigned char y){
    i_col_count = x / CELLS_PER_BYTE;
    i_col = x % CELLS_PER_BYTE;
    i_row = y;

    int state = 0;

    for (int i = 0; i < CELL_WIDTH; i++) {

        int i_bit = i_col * CELL_WIDTH + i;
        int i_value = ((board[i_col_count][i_row] >> i_bit) & 1);

        int value_at_index = (i_value << i);

        state = (state | value_at_index);
    }

    // if(state != 0){
    //     Serial.println("--------------");
    //     Serial.println("( " + String(x) + ", " + String(y) + " ) -> " + String(state));
    //     Serial.println("G: " + String(i_col_count));
    //     Serial.println("C: " + String(i_col));
    //     Serial.println("R: " + String(i_row));
    //     Serial.println(" V: " + String(state));
    // }

    return state;
}

void Board::setState(unsigned char x, unsigned char y, int state){
    i_col_count = x / CELLS_PER_BYTE;
    i_col = x % CELLS_PER_BYTE;
    i_row = y;

    int i_value_to_store = 0;
    int i_bit = 0;

    for (int i = 0; i < CELL_WIDTH; i++) {
        i_value_to_store = (state >> i) & 1;
        i_bit = i_col * CELL_WIDTH + i;

        board[i_col_count][i_row] ^= (-i_value_to_store ^ board[i_col_count][i_row]) & (1 << i_bit);
    }

    return;


}

void Board::setAlive(unsigned char x, unsigned char y){
    setState(x, y, CELL_STATE_ALIVE);
}
void Board::setDead(unsigned char x, unsigned char y){
    setState(x, y, CELL_STATE_DEAD);
}

void Board::reset(){
    for (i_col_count = 0; i_col_count < num_columns; i_col_count++) {
        for(i_row = 0; i_row < height; i_row++) {
            board[i_col_count][i_row] = NULL;
        }
    }

    for (i_col_count = 0; i_col_count < num_columns; i_col_count++) {
        for (i_col = 0; i_col < width; i_col++) {
            for(i_row = 0; i_row < height; i_row++) {
                board[i_col_count][i_row] = (board[i_col_count][i_row] | (CELL_STATE_DEAD << i_col));
            }
        }
    }
}



#endif /* Board_h */
